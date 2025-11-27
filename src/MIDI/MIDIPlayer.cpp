//
// Created by grpaschoal on 26/09/2025.
//

#include "MIDIPlayer.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>

std::vector<Channel> MIDIPlayer::channels = std::vector<Channel>(16);

bool MIDIPlayer::paused = true;
double MIDIPlayer::time = 0.0f;
double MIDIPlayer::song_length = 0.0f;
double MIDIPlayer::song_speed = 1.0f;
bool MIDIPlayer::loop_song = true;

std::thread MIDIPlayer::midiThread;
std::atomic<bool> MIDIPlayer::threadRunning(false);
std::mutex MIDIPlayer::midiMutex;

std::queue<NoteCallbackEvent> MIDIPlayer::eventQueue;
std::mutex MIDIPlayer::eventQueueMutex;

std::vector<int> MIDIPlayer::registeredChannels;
std::mutex MIDIPlayer::registeredChannelsMutex;

void MIDIPlayer::loadSong(const char *filename, bool loop_enabled) {
  std::lock_guard<std::mutex> lock(midiMutex);

  for (int i = 0; i < 16; ++i) {
    if (!channels[i].notes.empty())
      fluid_synth_all_notes_off(SynthEngine::synth, i);
  }

  channels.clear();
  channels = std::vector<Channel>(16);
  paused = true;
  time = 0.0f;
  song_length = 0.0f;
  song_speed = 1.0f;
  loop_song = loop_enabled;

  Midi f{filename};

  auto &header = f.getHeader();
  if (header.getFormat() != 1)
    throw std::runtime_error("Incompatible MIDI type!");

  auto &tracks = f.getTracks();

  double ppq = header.getDivision();
  double mpqn = 500000;

  for (const auto &track : tracks) {

    // inside track loop, before iterating events
    double current_seconds = 0.0;
    int tick_acc = 0;

    auto &events = track.getEvents();

    for (const auto &trackEvent : events) {
      int delta = trackEvent.getDeltaTime().getData();
      tick_acc += delta;

      double seconds_per_tick = mpqn / (1e6 * ppq);
      current_seconds += delta * seconds_per_tick;
      song_length = std::max(song_length, current_seconds);

      auto *event = trackEvent.getEvent();
      uint8_t *data;

      if (event->getType() == MidiType::EventType::MidiEvent) {
        auto *midiEvent = (MidiEvent *)event;

        auto status = midiEvent->getStatus();
        uint8_t channel = midiEvent->getChannel();

        if (status == MidiType::MidiMessageStatus::NoteOn) {
          channels.at(channel).active = true;
          channels.at(channel).notes.push_back({current_seconds, true,
                                                midiEvent->getNote(),
                                                midiEvent->getVelocity()});
        } else if (status == MidiType::MidiMessageStatus::NoteOff) {
          channels.at(channel).active = true;
          channels.at(channel).notes.push_back({current_seconds, false,
                                                midiEvent->getNote(),
                                                midiEvent->getVelocity()});
        } else if (status == MidiType::MidiMessageStatus::ProgramChange) {
          // Program change: set instument - disabled for now
          // fluid_synth_program_change(SynthEngine::synth, channel, program);
        } else if (status == MidiType::MidiMessageStatus::ControlChange) {
          // Control change: handles pan, volume, etc.
          uint16_t data = midiEvent->getData();
          uint8_t controller = (data >> 8) & 0xFF; // First data byte
          uint8_t value = data & 0xFF;             // Second data byte

          if (controller == 10) { // Pan
            // std::cout << "Channel " << (int)channel << ": Pan set to " <<
            // (int)value << std::endl;
            SynthEngine::setPan(channel, value);
          } else if (controller == 7) { // Volume
            // std::cout << "Channel " << (int)channel << ": Volume set to " <<
            // (int)value << std::endl;
            fluid_synth_cc(SynthEngine::synth, channel, 7, value);
          } else {
            // Other control changes
            fluid_synth_cc(SynthEngine::synth, channel, controller, value);
          }
        } else if (status == MidiType::MidiMessageStatus::PitchBend) {
          int data = midiEvent->getData();
          int lsb = data >> 8;
          int msb = data & 0xFF;
          int value = (msb << 7) | lsb;
          channels.at(channel).pitchBends.push_back({current_seconds, value});
        }
      } else if (event->getType() == MidiType::EventType::MetaEvent) {
        MetaEvent *metaEvent = (MetaEvent *)event;
        if (metaEvent->getStatus() == MidiType::MetaMessageStatus::SetTempo) {
          const uint8_t *d = metaEvent->getData();
          int len = metaEvent->getLength();
          if (d && len >= 3) {
            mpqn = static_cast<float>((static_cast<uint32_t>(d[0]) << 16) |
                                      (static_cast<uint32_t>(d[1]) << 8) |
                                      static_cast<uint32_t>(d[2]));
          }
        }
      }
    }
  }

  for (auto &channel : channels) {
    std::stable_sort(channel.notes.begin(), channel.notes.end(),
                     [](const NoteEvent &a, const NoteEvent &b) {
                       return a.start < b.start;
                     });
    std::stable_sort(channel.pitchBends.begin(), channel.pitchBends.end(),
                     [](const PitchBendEvent &a, const PitchBendEvent &b) {
                       return a.time < b.time;
                     });
  }

  std::cout << "Active channels: ";
  for (int i = 0; i < 16; ++i) {
    channels[i].pos = 0;
    channels[i].pitchBendPos = 0;
    channels[i].transpose = 0;
    fluid_synth_pitch_bend(SynthEngine::synth, i,
                           8192); // Reset pitch bend to center
    channels[i].active = !channels[i].notes.empty();
    if (channels[i].active)
      std::cout << i << " ";
  }
  std::cout << "\nLoaded " << filename << ", song length: " << song_length
            << std::endl;
}

void MIDIPlayer::setSpeed(double speed) {
  std::lock_guard<std::mutex> lock(midiMutex);
  song_speed = speed;
}

void MIDIPlayer::clearEventQueue() {
  std::lock_guard<std::mutex> lock(eventQueueMutex);
  while (!eventQueue.empty()) {
    eventQueue.pop();
  }
}

void MIDIPlayer::update(float dt) {
  std::lock_guard<std::mutex> lock(midiMutex);

  if (!paused) {
    if (time < song_length)
      time += dt * song_speed;
  }

  for (int i = 0; i < 16; ++i) {
    Channel &channel = channels[i];

    while (channel.pos < channel.notes.size() &&
           time >= channel.notes.at(channel.pos).start) {
      if (!paused && channel.active) {
        // Apply transpose to the note
        int transposed_note =
            channel.notes.at(channel.pos).note + channel.transpose;
        // Clamp to valid MIDI range (0-127)
        if (transposed_note < 0)
          transposed_note = 0;
        if (transposed_note > 127)
          transposed_note = 127;

        bool noteOn = channel.notes.at(channel.pos).on;
        int velocity = channel.notes.at(channel.pos).velocity;

        if (noteOn)
          SynthEngine::startNote(i, transposed_note, velocity);
        else
          SynthEngine::stopNote(i, transposed_note);

        // Look ahead for the next noteOn event in this channel
        bool hasNextNote = false;
        int nextNote = -1;
        double nextNoteTime = -1.0;

        for (unsigned int lookAhead = channel.pos + 1;
             lookAhead < channel.notes.size(); ++lookAhead) {
          if (channel.notes[lookAhead].on) {
            // Found the next noteOn event
            hasNextNote = true;
            nextNote = channel.notes[lookAhead].note + channel.transpose;
            // Clamp next note to valid MIDI range
            if (nextNote < 0)
              nextNote = 0;
            if (nextNote > 127)
              nextNote = 127;
            nextNoteTime = channel.notes[lookAhead].start - time;
            break;
          }
        }

        if (!hasNextNote && loop_song) {
          // Find the first noteOn in the channel for looping
          for (const auto &note : channel.notes) {
            if (note.on) {
              nextNote = note.note + channel.transpose;
              // Clamp next note to valid MIDI range
              if (nextNote < 0)
                nextNote = 0;
              if (nextNote > 127)
                nextNote = 127;
              nextNoteTime = note.start + song_length - time;
              break;
            }
          }
        }

        // Push event to queue for game loop consumption
        pushNoteEvent(i, transposed_note, velocity, noteOn, hasNextNote,
                      nextNote, nextNoteTime);
      }

      channel.pos++;
      if (channel.pos >= channel.notes.size()) {
        channel.pos = channel.notes.size();
        break;
      }
    }

    // Send pitch bend events
    while (channel.pitchBendPos < channel.pitchBends.size() &&
           time >= channel.pitchBends[channel.pitchBendPos].time) {
      fluid_synth_pitch_bend(SynthEngine::synth, i,
                             channel.pitchBends[channel.pitchBendPos].value);
      channel.pitchBendPos++;
    }
  }

  if (time >= song_length) {

    if (loop_song) {
      time = fmod(time, song_length);
      for (int i = 0; i < 16; ++i) {
        if (!channels[i].notes.empty()) {
          fluid_synth_all_notes_off(SynthEngine::synth, i);
          channels[i].pos = 0;
          channels[i].pitchBendPos = 0;
          fluid_synth_pitch_bend(SynthEngine::synth, i,
                                 8192); // Reset pitch bend to center
        }
      }
    }
  }
}

void MIDIPlayer::play() {
  std::lock_guard<std::mutex> lock(midiMutex);

  if (time >= song_length) {
    time = fmod(time, song_length);
    for (int i = 0; i < 16; ++i) {
      channels[i].pos = 0;
      channels[i].pitchBendPos = 0;
      fluid_synth_pitch_bend(SynthEngine::synth, i,
                             8192); // Reset pitch bend to center
      if (channels[i].active) {
      }
    }
  }
  paused = false;
}

void MIDIPlayer::pause() {
  std::lock_guard<std::mutex> lock(midiMutex);

  for (int i = 0; i < 16; ++i) {
    if (!channels[i].notes.empty()) {
      fluid_synth_all_notes_off(SynthEngine::synth, i);
    }
  }
  paused = true;
}

void MIDIPlayer::jumpTo(float seconds) {
  std::lock_guard<std::mutex> lock(midiMutex);

  paused = true;
  time = seconds;
  // Reset channel positions to match new time
  for (int i = 0; i < 16; ++i) {
    if (!channels[i].notes.empty()) {
      fluid_synth_all_notes_off(SynthEngine::synth, i);
      channels[i].pos = 0;
      channels[i].pitchBendPos = 0;
      fluid_synth_pitch_bend(SynthEngine::synth, i,
                             8192); // Reset pitch bend to center
      // Find the correct position for this time
      while (channels[i].pos < channels[i].notes.size() &&
             channels[i].notes[channels[i].pos].start < time) {
        channels[i].pos++;
      }
      while (channels[i].pitchBendPos < channels[i].pitchBends.size() &&
             channels[i].pitchBends[channels[i].pitchBendPos].time < time) {
        channels[i].pitchBendPos++;
      }
      // Apply the current pitch bend state
      if (channels[i].pitchBendPos > 0) {
        fluid_synth_pitch_bend(
            SynthEngine::synth, i,
            channels[i].pitchBends[channels[i].pitchBendPos - 1].value);
      }
    }
  }
  paused = false;
}

void MIDIPlayer::muteChannel(unsigned int channel) {
  std::lock_guard<std::mutex> lock(midiMutex);
  fluid_synth_all_notes_off(SynthEngine::synth, channel);
  channels[channel].active = false;
}

void MIDIPlayer::unmuteChannel(unsigned int channel) {
  std::lock_guard<std::mutex> lock(midiMutex);
  if (channels[channel].notes.size() > 0)
    channels[channel].active = true;
}

void MIDIPlayer::setChannelTranspose(unsigned int channel, int semitones) {
  std::lock_guard<std::mutex> lock(midiMutex);
  if (channel < 16) {
    channels[channel].transpose = semitones;
  }
}

void MIDIPlayer::setChannelVolume(unsigned int channel, int volume) {
  std::lock_guard<std::mutex> lock(midiMutex);
  if (channel < 16) {
    // Clamp volume to valid range (0-127)
    if (volume < 0)
      volume = 0;
    if (volume > 127)
      volume = 127;
    fluid_synth_cc(SynthEngine::synth, channel, 7, volume);
    fluid_synth_cc(SynthEngine::synth, channel, 11, volume); // expression too
  }
}

void MIDIPlayer::startMIDIThread() {
  if (threadRunning.load()) {
    std::cerr << "MIDI thread already running!" << std::endl;
    return;
  }

  threadRunning.store(true);
  midiThread = std::thread(midiThreadFunction);
  std::cout << "MIDI thread started" << std::endl;
}

void MIDIPlayer::stopMIDIThread() {
  if (!threadRunning.load()) {
    return;
  }

  threadRunning.store(false);
  if (midiThread.joinable()) {
    midiThread.join();
  }
  std::cout << "MIDI thread stopped" << std::endl;
}

void MIDIPlayer::midiThreadFunction() {
  const double MIDI_UPDATE_INTERVAL = 0.001; // 1ms = 1000 updates per second
  const auto interval = std::chrono::microseconds(
      static_cast<long>(MIDI_UPDATE_INTERVAL * 1000000));

  auto nextUpdate = std::chrono::high_resolution_clock::now();

  while (threadRunning.load()) {
    // Update MIDI player with fixed timestep
    update(static_cast<float>(MIDI_UPDATE_INTERVAL));

    // Sleep until next update time
    nextUpdate += interval;
    std::this_thread::sleep_until(nextUpdate);
  }
}

void MIDIPlayer::pushNoteEvent(int channel, int note, int velocity, bool noteOn,
                               bool hasNextNote, int nextNote,
                               double nextNoteTime) {
  // Check if we should filter this channel
  {
    std::lock_guard<std::mutex> lock(registeredChannelsMutex);
    if (!registeredChannels.empty()) {
      bool shouldPush = false;
      for (int registeredChannel : registeredChannels) {
        if (registeredChannel == channel) {
          shouldPush = true;
          break;
        }
      }
      if (!shouldPush) {
        return; // Skip this event
      }
    }
  }

  // Push to event queue
  std::lock_guard<std::mutex> lock(eventQueueMutex);
  eventQueue.push({channel, note, velocity, noteOn, time, hasNextNote, nextNote,
                   nextNoteTime});
}

std::vector<NoteCallbackEvent> MIDIPlayer::pollNoteEvents() {
  std::vector<NoteCallbackEvent> events;
  std::lock_guard<std::mutex> lock(eventQueueMutex);

  // Move all events from queue to vector
  while (!eventQueue.empty()) {
    events.push_back(eventQueue.front());
    eventQueue.pop();
  }

  return events;
}

void MIDIPlayer::registerChannelForEvents(int channel) {
  std::lock_guard<std::mutex> lock(registeredChannelsMutex);

  // Check if already registered
  for (int ch : registeredChannels) {
    if (ch == channel) {
      return;
    }
  }

  registeredChannels.push_back(channel);
}

void MIDIPlayer::unregisterChannelForEvents(int channel) {
  std::lock_guard<std::mutex> lock(registeredChannelsMutex);

  auto it =
      std::find(registeredChannels.begin(), registeredChannels.end(), channel);
  if (it != registeredChannels.end()) {
    registeredChannels.erase(it);
  }
}

void MIDIPlayer::clearRegisteredChannels() {
  std::lock_guard<std::mutex> lock(registeredChannelsMutex);
  registeredChannels.clear();
}