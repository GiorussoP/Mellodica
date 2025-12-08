#include "scenes/GameOver.hpp"
#include "../../include/UI/Screen/GameOverScreen.hpp"
#include "AssetLoader.hpp"
#include "MIDI/MIDIPlayer.hpp"

void GameOver::Initialize() {

  mGame->GetRenderer()->SetIsDark(true);
  auto mGOScreen =
      new GameOverScreen(mGame,getAssetPath( "fonts/MedodicaRegular.otf"));

  MIDIPlayer::loadGameOverTheme();
  MIDIPlayer::jumpTo(-1.5f);
  MIDIPlayer::play();
}
