#include "scenes/OpeningScene.hpp"
#include "../../include/UI/Screen/OpeningScreen.hpp"
#include "MIDI/MIDIPlayer.hpp"

void OpeningScene::Initialize() {
  auto mOpeningScreen =
      new OpeningScreen(mGame, "./assets/fonts/MedodicaRegular.otf");

  MIDIPlayer::loadOpeningTheme();
  MIDIPlayer::play();
}