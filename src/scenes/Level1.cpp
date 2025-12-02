#include "scenes/Level1.hpp"

#include "BattleSystem.hpp"
#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "Renderer.hpp"
#include "actors/PuzzleActors.hpp"
#include <iostream>

void Level1::Initialize() {
  std::cout << "Initializing TestSceneB..." << std::endl;

  mGame->GetRenderer()->SetIsDark(false);
  mGame->GetRenderer()->setNight();

  // Load song before creating battle system
  MIDIPlayer::loadSong0();

  LoadLevel("assets/levels/level1");

  // TODO: Colocar no editor de mapas
  auto musicButton = new MusicButtonActor(mGame, 60); // C4
  musicButton->SetPosition(mGame->GetPlayer()->GetPosition() +
                           Vector3(-3.0f, 0.5f, 0.0f));

  // Creating the battle system
  mGame->SetBattleSystem(new BattleSystem(mGame));
  MIDIPlayer::play();
}