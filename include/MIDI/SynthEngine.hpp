//
// SynthEngine.h
//
#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H


#include <fluidsynth.h>
#include <string>
#include <vector>

struct SoundPreset {
    int bank_num;
    int num;
};

class SynthEngine {
public:

    static void init(const char* soundfont_path = "songs/sf.sf2", const char* audio_driver = "sdl2");
    static void clean();
    static void setChannels(const std::vector<SoundPreset>& presets);
    static std::vector<std::pair<std::string,SoundPreset>> getSoundPresets();
    static void startNote(unsigned int ch,unsigned int note, unsigned int velocity = 127);
    static void stopNote(unsigned int ch,unsigned int note);
    static void setPan(unsigned int ch, unsigned int pan);
    static void testSoundFont();

    static fluid_synth_t* synth;
private:

    static fluid_settings_t* settings;

    static fluid_audio_driver_t* driver;
    static int sfid;
};

#endif