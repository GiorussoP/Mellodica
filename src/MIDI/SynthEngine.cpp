//
// Created by grpaschoal on 26/09/2025.
//

#include "MIDI/SynthEngine.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>

fluid_settings_t *SynthEngine::settings = nullptr;
fluid_synth_t *SynthEngine::synth = nullptr;
fluid_audio_driver_t *SynthEngine::driver = nullptr;
int SynthEngine::sfid = 0;

void SynthEngine::init(const char *soundfont_path, const char *audio_driver) {
  settings = new_fluid_settings();
  fluid_settings_setstr(settings, "audio.driver", audio_driver);
  fluid_settings_setnum(settings, "synth.gain", 1.0);
  fluid_settings_setnum(settings, "synth.sample-rate",
                        32000); // try 32000 or 22050

  synth = new_fluid_synth(settings);

  fluid_synth_set_polyphony(synth, 2048);

  sfid = fluid_synth_sfload(synth, soundfont_path, /*reset_presets=*/0);
  if (sfid < 0) {
    std::cerr << "Failed to load SF2: " << soundfont_path << std::endl;
  }
  driver = nullptr;

  std::cout << "Loaded SoundFont: " << soundfont_path << std::endl;
  for (auto preset : getSoundPresets())
    std::cout << preset.first << ": " << preset.second.bank_num << "/"
              << preset.second.num << '\n';
}

void SynthEngine::clean() {
  if (driver) {
    delete_fluid_audio_driver(driver);
    driver = nullptr;
  }
  if (synth) {
    delete_fluid_synth(synth);
    synth = nullptr;
  }
  if (settings) {
    delete_fluid_settings(settings);
    settings = nullptr;
  }
}

std::vector<std::pair<std::string, SoundPreset>>
SynthEngine::getSoundPresets() {

  fluid_sfont_t *sfont = fluid_synth_get_sfont_by_id(synth, sfid);

  std::vector<std::pair<std::string, SoundPreset>> presets;

  fluid_sfont_iteration_start(sfont);
  fluid_preset_t *preset;
  while ((preset = fluid_sfont_iteration_next(sfont))) {
    std::pair<std::string, SoundPreset> p = {
        fluid_preset_get_name(preset),
        {fluid_preset_get_banknum(preset), fluid_preset_get_num(preset)}};
    presets.emplace_back(p);
  }

  return presets;
}

void SynthEngine::setChannels(const std::vector<SoundPreset> &presets) {
  if (driver != nullptr)
    delete_fluid_audio_driver(driver);
  if (presets.size() != 16)
    throw std::logic_error("16 channels need to be set!");

  // Default panning
  const int default_pan_positions[16] = {
      64, // Channel 0: center
      54, // Channel 1: slight left
      74, // Channel 2: slight right
      44, // Channel 3: left
      84, // Channel 4: right
      34, // Channel 5: more left
      94, // Channel 6: more right
      49, // Channel 7: left-center
      79, // Channel 8: right-center
      64, // Channel 9: center (drums typically centered)
      59, // Channel 10: slight left
      69, // Channel 11: slight right
      39, // Channel 12: left
      89, // Channel 13: right
      29, // Channel 14: far left
      99  // Channel 15: far right
  };

  for (int ch = 0; ch < presets.size(); ++ch) {
    SoundPreset preset = presets[ch];

    // Set channel type based on bank number (128 = drum bank in General MIDI)
    if (preset.bank_num == 128) {
      fluid_synth_set_channel_type(synth, ch, CHANNEL_TYPE_DRUM);
    } else {
      fluid_synth_set_channel_type(synth, ch, CHANNEL_TYPE_MELODIC);
    }

    if (fluid_synth_program_select(synth, ch, sfid, preset.bank_num,
                                   preset.num) != 0) {
      std::cerr << "Failed to bind channel " << ch << " to bank "
                << preset.bank_num << ", num " << preset.num << "\n";
    }
    fluid_synth_cc(synth, ch, 10, default_pan_positions[ch]); // set default pan

    fluid_synth_cc(synth, ch, 7, 127);  // volume max
    fluid_synth_cc(synth, ch, 11, 127); // expression max

    fluid_synth_cc(synth, ch, 91, 127); // Reverb max
    fluid_synth_cc(synth, ch, 93, 10);  // Chorus 10
    fluid_synth_cc(synth, ch, 64, 1);   // sustain pedal on

    fluid_synth_cc(synth, ch, 120, 0);    // all sound off
    fluid_synth_cc(synth, ch, 123, 0);    // all notes off
    fluid_synth_all_notes_off(synth, ch); // stop notes
  }

  driver = new_fluid_audio_driver(settings, synth);
}

void SynthEngine::startNote(unsigned int ch, unsigned int note,
                            unsigned int velocity) {
  if (!synth)
    return;

  int panning_shift = (note % 12) - 6; // Shift based on note within an octave
  setPan(ch, 64 + panning_shift * 6);
  fluid_synth_noteon(synth, ch, note, velocity);
}

void SynthEngine::stopNote(unsigned int ch, unsigned int note) {
  if (synth)
    fluid_synth_noteoff(synth, ch, note);
}

void SynthEngine::setPan(unsigned int ch, unsigned int pan) {
  if (pan > 127)
    pan = 127;
  fluid_synth_cc(synth, ch, 10, pan);
}

void SynthEngine::testSoundFont() {

  auto presets = getSoundPresets();

  for (auto preset : presets) {
    std::cout << "Testing " << preset.first << ": (" << preset.second.bank_num
              << '/' << preset.second.num << ")..." << std::endl;

    setChannels({{preset.second.bank_num, preset.second.num}});

    for (int i = 0; i < 5; ++i) {
      startNote(0, 60 + 4 * i);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    for (int i = 0; i < 5; ++i) {
      stopNote(0, 60 + 4 * i);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    for (int i = 4; i >= 0; --i) {
      startNote(0, 60 + 4 * i);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    for (int i = 4; i >= 0; --i) {
      stopNote(0, 60 + 4 * i);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }
}
