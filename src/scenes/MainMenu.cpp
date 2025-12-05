

#include "scenes/MainMenu.hpp"
#include "../../include/UI/HUDElement.hpp"
#include "../../include/UI/Screen/MainMenuScreen.hpp"
#include "MIDI/MIDIPlayer.hpp"

void MainMenu::Initialize() {
  auto mMMScreen =
      new MainMenuScreen(mGame, "./assets/fonts/MedodicaRegular.otf");

  MIDIPlayer::loadMainTheme();

  MIDIPlayer::play();
}