#include "scenes/CreditsScene.hpp"
#include "../../include/UI/Screen/CreditsScreen.hpp"
#include "AssetLoader.hpp"
#include "MIDI/MIDIPlayer.hpp"

void CreditsScene::Initialize() {
  auto mCreditsScreen =
      new CreditsScreen(mGame,getAssetPath("fonts/MedodicaRegular.otf"));

  MIDIPlayer::loadCreditsTheme();
  MIDIPlayer::jumpTo(-5.0f);
  MIDIPlayer::play();
}
