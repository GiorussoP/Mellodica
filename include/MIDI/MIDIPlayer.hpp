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

// Event for the manual note queue (independent from song playback)
struct NoteQueueEvent {
  double delay; // Time delay in seconds from previous event (cumulative)
  int channel;  // MIDI channel (0-15)
  int note;     // MIDI note number (0-127)
  int velocity; // Note velocity (0-127), defaults to 100
  bool noteOn;  // true = note on, false = note off

  // Constructor with default velocity
  NoteQueueEvent(double d, int ch, int n, bool on, int vel = 100)
      : delay(d), channel(ch), note(n), velocity(vel), noteOn(on) {}
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

  // Manual note queue system (independent from song playback)
  static void playSequence(const std::vector<NoteQueueEvent> &events);
  static void clearNoteQueue();
  static bool isNoteQueueEmpty();

  // Song loading methods
  static void loadMainTheme();
  static void loadGameOverTheme();
  static void loadOpeningTheme();
  static void loadCreditsTheme();
  static void loadSong0();
  static void loadSong1();
  static void loadSong2a();
  static void loadSong2b();
  static void loadSong3();

private:
  static void midiThreadFunction();
  static void pushNoteEvent(int channel, int note, int velocity, bool noteOn,
                            bool hasNextNote, int nextNote,
                            double nextNoteTime);
  static void processNoteQueue(double dt);

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

  // Note queue system
  struct QueuedNoteWithTime {
    double triggerTime; // Absolute time when this event should trigger
    int channel;
    int note;
    int velocity;
    bool noteOn;
  };
  static std::vector<QueuedNoteWithTime> noteQueue;
  static std::mutex noteQueueMutex;
  static double noteQueueTimer;
};

#endif