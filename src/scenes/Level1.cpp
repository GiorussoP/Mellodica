#include "scenes/Level1.hpp"

#include "BattleSystem.hpp"
#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "Renderer.hpp"
#include <iostream>

void Level1::Initialize() {
  std::cout << "Initializing TestSceneB..." << std::endl;

  mGame->GetRenderer()->SetIsDark(false);

  // Load song before creating battle system
  MIDIPlayer::loadSong0();

  LoadLevel("assets/levels/level1");

  // Creating the battle system
  mGame->SetBattleSystem(new BattleSystem(mGame));
  MIDIPlayer::play();
}