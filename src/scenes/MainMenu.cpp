

#include "scenes/MainMenu.hpp"
#include "../../include/UI/HUDElement.hpp"
#include "../../include/UI/Screen/MainMenuScreen.hpp"
#include "MIDI/MIDIPlayer.hpp"

void MainMenu::Initialize() {
  auto image = new HUDElement(mGame, "assets/sprites/scenes/title-screen.png");
  auto mMMScreen = new MainMenuScreen(mGame, "randomstring");

  image->SetScale(Vector3(2.0f, 2.0f, 1.0f));

  MIDIPlayer::loadMainTheme();

  MIDIPlayer::play();
}