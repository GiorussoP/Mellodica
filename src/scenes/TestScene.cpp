#include "TestScene.hpp"
#include "HUDElement.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "Renderer.hpp"
#include "actors/Player.hpp"
#include "actors/TestActors.hpp"

#include <iostream>

void TestSceneA::Initialize() {

  std::cout << "Initializing TestSceneA..." << std::endl;

  // Setting scene instruments
  SynthEngine::setChannels({{0, 0},
                            {0, 2},
                            {0, 32},
                            {0, 24},
                            {0, 25},
                            {0, 40},
                            {0, 48},
                            {0, 56},
                            {0, 66},
                            {128, 0},
                            {0, 89},
                            {0, 73},
                            {0, 57},
                            {0, 42},
                            {0, 11},
                            {0, 52}});

  // Initializing MIDI Player
  MIDIPlayer::loadSong("assets/songs/8.mid", true);
  MIDIPlayer::play();

  // Some actors for testing
  const int gridSize = 300;
  const float spacing = 1.0f;
  const float gridOffset = (gridSize - 1) * spacing * 0.5f;
  int actorCount = 0;
  for (int x = 0; x < gridSize; x++) {
    for (int z = 0; z < gridSize; z++) {
      auto mesh = new Actor(mGame);
      mesh->SetPosition(
          Vector3(x * spacing - gridOffset, 0.0f, z * spacing - gridOffset));
      actorCount += 2;
    }
  }

  auto pyramid = new PyramidActor(mGame, Color::Red);
  pyramid->SetPosition(Vector3(6.0f, 1.0f, 0.0f));
  pyramid->SetScale(Vector3(2.0f, 1.0f, 2.0f));

  auto cube1 = new CubeActor(mGame, Color::Green);
  cube1->SetPosition(Vector3(12.0f, 1.0f, 5.0f));
  cube1->SetScale(Vector3(5.0f, 0.5f, 1.0f));
  cube1->GetComponent<MeshComponent>()->SetBloomed(true);

  auto cube2 = new CubeActor(mGame, Color::Yellow);
  cube2->SetPosition(Vector3(7.5f, 1.0f, -2.5f));
  cube2->SetScale(Vector3(0.5f, 2.5f, 0.5f));
  cube2->GetComponent<MeshComponent>()->SetBloomed(true);

  auto cube3 = new CubeActor(mGame, Color::Blue);
  cube3->SetPosition(Vector3(5.5f, 0.5f, -7.5f));
  cube3->SetScale(Vector3(3.5f, 2.5f, 0.5f));
  cube3->SetRotation(Quaternion(0.7071f, 0.0f, 0.0f, 0.7071f));
  cube3->GetComponent<MeshComponent>()->SetBloomed(true);

  // Create test OBB actor (tilted 45 degrees)
  auto obbTest1 = new OBBTestActor(mGame);
  obbTest1->SetPosition(Vector3(5.0f, 1.0f, 0.0f));

  auto obbTest2 = new OBBTestActor(mGame);
  obbTest2->SetPosition(
      Vector3(6.0f, 1.0f, -4.0f)); // Place it to the right of spawn

  // Creating the Player actor
  mGame->SetPlayer(new Player(mGame));
  mGame->GetPlayer()->SetPosition(Vector3(10.0f, 1.0f, 0.0f));
  mGame->GetPlayer()->GetComponent<SpriteComponent>()->SetBloomed(true);

  // Creating a test HUD element
  auto hudElement = new HUDElement(mGame, "assets/textures/hud/hud.png",
                                   "assets/textures/hud/hud.json");
  hudElement->SetPosition(Vector3(-0.4f, 0.4f, 0.0f));
  hudElement->GetSpriteComponent().AddAnimation("default",
                                                {"hud1.png", "hud2.png"});
  hudElement->GetSpriteComponent().SetAnimFPS(2.0f);
  hudElement->GetSpriteComponent().SetAnimation("default");

  // Setting camera
  mGame->SetCameraPos(mGame->GetPlayer()->GetPosition());
  mGame->SetCameraForward(Vector3::Normalize(Vector3(0.0f, -1.0f, -1.0f)));

  for (int i = -25; i < 25; i++) {
    for (int j = -25; j < 25; j++) {
      auto cube4 = new CubeActor(mGame, Color::White, 5);
      cube4->SetPosition(Vector3((float)i, 0.0f, (float)j));
      cube4->SetScale(Vector3(1.0f, 1.0f, 1.0f));
      cube4->GetComponent<MeshComponent>()->SetBloomed(false);
      mGame->AddAlwaysActive(cube4);
    }
  }
}

void TestSceneB::Initialize() {
  std::cout << "Initializing TestSceneB..." << std::endl;

  SynthEngine::setChannels({{8, 39},
                            {0, 2},
                            {0, 32},
                            {0, 24},
                            {0, 25},
                            {0, 40},
                            {0, 48},
                            {0, 56},
                            {0, 66},
                            {128, 0},
                            {0, 89},
                            {0, 73},
                            {0, 57},
                            {0, 42},
                            {0, 11},
                            {0, 52}});

  // Creating the Player actor
  mGame->SetPlayer(new Player(mGame));
  mGame->GetPlayer()->SetPosition(Vector3(12.0f, 1.0f, 12.0f));
  mGame->GetPlayer()->GetComponent<SpriteComponent>()->SetBloomed(false);
  mGame->GetRenderer()->SetIsDark(false);

  MIDIPlayer::loadSong("assets/songs/3.mid", true);
  MIDIPlayer::play();

  auto testMario = new MarioActor(mGame);
  testMario->SetPosition(Vector3(-5.0f, 1.0f, 0.0f));

  auto testGoomba = new GoombaActor(mGame);
  testGoomba->SetPosition(Vector3(-5.0f, 1.0f, 5.0f));

  auto obbTest1 = new OBBTestActor(mGame);
  obbTest1->SetPosition(Vector3(5.0f, 1.0f, 0.0f));
  obbTest1->SetRotation(Quaternion(Vector3::UnitY, Math::ToRadians(45.0f)));

  auto cube2 = new CubeActor(mGame, Color::Cyan, 0);
  cube2->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  cube2->SetScale(Vector3(32.0f, 1.0f, 32.0f));
  mGame->AddAlwaysActive(cube2);
}