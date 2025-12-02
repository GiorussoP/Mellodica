

#include "scenes/MainMenu.hpp"
#include "../../include/UI/HUDElement.hpp"
#include "../../include/UI/Screen/MainMenuScreen.hpp"
#include "MIDI/MIDIPlayer.hpp"

void MainMenu::Initialize() {
  auto mMMScreen = new MainMenuScreen(mGame, "randomstring");

  MIDIPlayer::loadMainTheme();

  MIDIPlayer::play();
}