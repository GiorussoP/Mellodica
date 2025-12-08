

#include "scenes/MainMenu.hpp"
#include "../../include/UI/HUDElement.hpp"
#include "../../include/UI/Screen/MainMenuScreen.hpp"
#include "AssetLoader.hpp"
#include "MIDI/MIDIPlayer.hpp"

void MainMenu::Initialize() {
  auto mMMScreen =
      new MainMenuScreen(mGame,getAssetPath("fonts/MedodicaRegular.otf"));

  MIDIPlayer::loadMainTheme();

  MIDIPlayer::play();
}
