//
// Created by grpaschoal on 26/09/2025.
//

#include "MIDIPlayer.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

std::vector<Channel> MIDIPlayer::channels = std::vector<Channel>(16);

bool MIDIPlayer::paused = true;
double MIDIPlayer::time = 0.0f;
double MIDIPlayer::song_length = 0.0f;
double MIDIPlayer::song_speed = 1.0f;
bool MIDIPlayer::loop_song = true;

std::thread MIDIPlayer::midiThread;
std::atomic<bool> MIDIPlayer::threadRunning(false);
std::mutex MIDIPlayer::midiMutex;

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
  }

  std::cout << "Active channels: ";
  for (int i = 0; i < 16; ++i) {
    channels[i].pos = 0;
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
        // std::cout<<"Time:"<<time<<", event:
        // "<<channel.notes.at(channel.pos).start<<'
        // '<<channel.notes.at(channel.pos).note<<"
        // "<<channel.notes.at(channel.pos).on<<'\n';
        if (channel.notes.at(channel.pos).on)
          SynthEngine::startNote(i, channel.notes.at(channel.pos).note,
                                 channel.notes.at(channel.pos).velocity);
        else
          SynthEngine::stopNote(i, channel.notes.at(channel.pos).note);
      }

      channel.pos++;
      if (channel.pos >= channel.notes.size()) {
        channel.pos = channel.notes.size();
        break;
      }
    }
  }

  if (time >= song_length) {

    if (loop_song) {
      time = fmod(time, song_length);
      for (int i = 0; i < 16; ++i) {
        if (!channels[i].notes.empty()) {
          fluid_synth_all_notes_off(SynthEngine::synth, i);
          channels[i].pos = 0;
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
      // Find the correct position for this time
      while (channels[i].pos < channels[i].notes.size() &&
             channels[i].notes[channels[i].pos].start < time) {
        channels[i].pos++;
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