//
// Created by grpaschoal on 26/09/2025.
//

#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <vector>

#include  "MIDIParser/Midi.h"
#include  "SynthEngine.hpp"

struct NoteEvent{
    double start;
    bool on;
    int note;
    int velocity;
};

struct Channel {
    bool active = false;
    unsigned int pos = 0;
    std::vector<NoteEvent> notes;
};

class MIDIPlayer {

    public:
        static std::vector<Channel>& getChannels() { return channels;}

        static void loadSong(const char* filename,bool loop_song = true);
        static void update(float dt);
        static void setSpeed(double speed);
        static void play();
        static void pause();
        static void jumpTo(float seconds);
        static void muteChannel(unsigned int channel);
        static void unmuteChannel(unsigned int channel);

    private:
        static std::vector<Channel> channels;
        static bool paused;
        static double time;
        static double song_length;
        static double song_speed;
        static bool loop_song;
};


#endif