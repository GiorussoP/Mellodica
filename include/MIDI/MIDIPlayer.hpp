//
// Created by grpaschoal on 26/09/2025.
//

#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "MIDIParser/Midi.h"
#include "SynthEngine.hpp"

struct NoteEvent {
  double start;
  bool on;
  int note;
  int velocity;
};

// Event that gets pushed to the callback queue
struct NoteCallbackEvent {
  int channel;
  int note;
  int velocity;
  bool noteOn;      // true = note started, false = note ended
  double timestamp; // time in song when event occurred

  // Next note information (useful for look-ahead mechanics)
  bool hasNextNote;    // true if there's another noteOn event in this channel
  int nextNote;        // MIDI note number of next noteOn (-1 if no next note)
  double nextNoteTime; // timestamp of next noteOn (-1.0 if no next note)
};

struct PitchBendEvent {
  double time;
  int value;
};

struct Channel {
  bool active = false;
  unsigned int pos = 0;
  int transpose = 0; // Semitones to transpose notes (+/-)
  std::vector<NoteEvent> notes;
  std::vector<PitchBendEvent> pitchBends;
  int pitchBendPos = 0;
};

class MIDIPlayer {
public:
  static std::vector<Channel> &getChannels() { return channels; }

  static void loadSong(const char *filename, bool loop_song = true);
  static void update(float dt); // Can still be called manually if needed
  static void setSpeed(double speed);
  static void play();
  static void pause();
  static void jumpTo(float seconds);
  static void muteChannel(unsigned int channel);
  static void unmuteChannel(unsigned int channel);
  static void setChannelTranspose(unsigned int channel, int semitones);
  static void setChannelVolume(unsigned int channel, int volume);

  // Thread management
  static void startMIDIThread();
  static void stopMIDIThread();

  // Event system - call this from game loop
  static std::vector<NoteCallbackEvent> pollNoteEvents();

  // Clear the event queue
  static void clearEventQueue();

  // Optional: Register channels you want to receive events from
  // If no channels registered, all channels will trigger events
  static void registerChannelForEvents(int channel);
  static void unregisterChannelForEvents(int channel);
  static void clearRegisteredChannels();

private:
  static void midiThreadFunction();
  static void pushNoteEvent(int channel, int note, int velocity, bool noteOn,
                            bool hasNextNote, int nextNote,
                            double nextNoteTime);

  static std::vector<Channel> channels;
  static bool paused;
  static double time;
  static double song_length;
  static double song_speed;
  static bool loop_song;

  // Thread synchronization
  static std::thread midiThread;
  static std::atomic<bool> threadRunning;
  static std::mutex midiMutex; // Protects all MIDI state

  // Event queue
  static std::queue<NoteCallbackEvent> eventQueue;
  static std::mutex eventQueueMutex;

  // Channel filtering - empty means all channels
  static std::vector<int> registeredChannels;
  static std::mutex registeredChannelsMutex;
};

#endif