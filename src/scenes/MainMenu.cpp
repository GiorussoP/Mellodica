

#include "scenes/MainMenu.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "../../include/UI/HUDElement.hpp"

void MainMenu::Initialize() {
  auto image = new HUDElement(mGame, "assets/sprites/scenes/title-screen.png");
  image->SetScale(Vector3(2.0f, 2.0f, 1.0f));

  SynthEngine::setChannels({{0, 0},  // Grand Piano
                            {0, 49}, // Slow Strings
                            {0, 73}, // Flute
                            {0, 10}, // Music box
                            {0, 56}, // Trumpet
                            {0, 32}, // Acoustic bass
                            {0, 0},
                            {0, 0},
                            {0, 0},
                            {128, 0}, // Drums 1
                            {0, 0},
                            {128, 1}, // Drums 2
                            {0, 0},   // Player Channel: Piano
                            {0, 0},
                            {0, 0},
                            {0, 0}});

  // Initializing MIDI Player
  MIDIPlayer::loadSong("assets/songs/main_theme.mid", true);

  MIDIPlayer::setChannelVolume(0, 127);
  MIDIPlayer::setChannelVolume(1, 127);
  MIDIPlayer::setChannelVolume(2, 127);
  MIDIPlayer::setChannelVolume(3, 127);
  MIDIPlayer::setChannelVolume(4, 127);
  MIDIPlayer::setChannelVolume(5, 127);
  MIDIPlayer::setChannelVolume(6, 127);

  MIDIPlayer::setChannelVolume(9, 127);

  // MIDIPlayer::muteChannel(0);
  // MIDIPlayer::muteChannel(1);
  // MIDIPlayer::muteChannel(2);
  // MIDIPlayer::muteChannel(3);
  // MIDIPlayer::muteChannel(4);
  // MIDIPlayer::muteChannel(5);
  // MIDIPlayer::muteChannel(6);
  //
  // MIDIPlayer::muteChannel(9);
  //
  MIDIPlayer::play();
}