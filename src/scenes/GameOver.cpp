#include "scenes/GameOver.hpp"
#include "../../include/UI/Screen/GameOverScreen.hpp"
#include "MIDI/MIDIPlayer.hpp"

void GameOver::Initialize() {

  mGame->GetRenderer()->SetIsDark(true);
  auto mGOScreen =
      new GameOverScreen(mGame, "./assets/fonts/MedodicaRegular.otf");

  MIDIPlayer::loadGameOverTheme();
  MIDIPlayer::play();
}