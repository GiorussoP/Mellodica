#include "scenes/CreditsScene.hpp"
#include "../../include/UI/Screen/CreditsScreen.hpp"
#include "MIDI/MIDIPlayer.hpp"

void CreditsScene::Initialize() {
  auto mCreditsScreen =
      new CreditsScreen(mGame, "./assets/fonts/MedodicaRegular.otf");

  MIDIPlayer::loadCreditsTheme();
  MIDIPlayer::jumpTo(-5.0f);
  MIDIPlayer::play();
}