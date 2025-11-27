#include "TestScene.hpp"
#include "HUDElement.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "Renderer.hpp"
#include "actors/Combatant.hpp"
#include "actors/EnemyGroup.hpp"
#include "actors/Player.hpp"
#include "actors/TestActors.hpp"

#include <iostream>

void TestSceneA::Initialize() {

  std::cout << "Initializing TestSceneA..." << std::endl;
  mGame->GetRenderer()->SetIsDark(true);

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
  MIDIPlayer::loadSong("assets/songs/a0.mid", true);
  MIDIPlayer::setChannelTranspose(11, -60);
  MIDIPlayer::play();

  // Creating the battke system
  mGame->SetBattleSystem(new BattleSystem(mGame));

  // Some actors for testing

  auto mesh = new Actor(mGame);
  mesh->SetScale(Vector3(300.f, 1.0f, 300.f));
  mesh->SetPosition(Vector3(0.0f, -1.0f, 0.0f));

  auto pyramid = new PyramidActor(mGame, Color::Red);
  pyramid->SetPosition(Vector3(-3.0f, 1.0f, 0.0f));
  pyramid->SetScale(Vector3(2.0f, 1.0f, 2.0f));

  auto cube1 = new CubeActor(mGame, Color::Green);
  cube1->SetPosition(Vector3(12.0f, 1.0f, 5.0f));
  cube1->SetScale(Vector3(5.0f, 0.5f, 1.0f));
  cube1->GetComponent<MeshComponent>()->SetBloomed(true);

  auto cube2 = new CubeActor(mGame, Color::Cyan);
  cube2->SetPosition(Vector3(12.5f, 1.0f, -2.5f));
  cube2->SetScale(Vector3(0.5f, 2.5f, 0.5f));
  cube2->GetComponent<MeshComponent>()->SetBloomed(true);

  auto cube3 = new CubeActor(mGame, Color::Blue);
  cube3->SetPosition(Vector3(5.5f, 0.5f, -7.5f));
  cube3->SetScale(Vector3(3.5f, 2.5f, 0.5f));
  cube3->SetRotation(Quaternion(0.7071f, 0.0f, 0.0f, 0.7071f));
  cube3->GetComponent<MeshComponent>()->SetBloomed(true);

  auto obbTest1 = new OBBTestActor(mGame);
  obbTest1->SetPosition(Vector3(5.0f, 1.0f, 5.0f));

  auto obbTest2 = new OBBTestActor(mGame);
  obbTest2->SetPosition(
      Vector3(6.0f, 1.0f, 7.0f)); // Place it to the right of spawn

  // Creating the Player actor
  mGame->SetPlayer(new Player(mGame));
  mGame->GetPlayer()->SetPosition(Vector3(10.0f, 1.0f, 0.0f));
  mGame->GetPlayer()->GetComponent<SpriteComponent>()->SetBloomed(true);

  // Creating a test HUD element
  auto hudElement = new HUDElement(mGame, "assets/textures/hud/hud.png",
                                   "assets/textures/hud/hud.json");
  hudElement->SetPosition(Vector3(-0.8, -0.8f, 0.0f));
  hudElement->SetScale(Vector3(0.2f, 0.2f, 1.0f));
  hudElement->GetSpriteComponent().AddAnimation("default",
                                                {"hud1.png", "hud2.png"});
  hudElement->GetSpriteComponent().SetAnimFPS(2.0f);
  hudElement->GetSpriteComponent().SetAnimation("default");

  // Setting camera
  mGame->GetCamera()->SetMode(CameraMode::Isometric);
  mGame->GetCamera()->SetPosition(mGame->GetPlayer()->GetPosition());
  mGame->GetCamera()->SetCameraForward(
      Vector3::Normalize(Vector3(0.0f, -1.0f, -1.0f)));

  auto cube4 = new CubeActor(mGame, Color::White, 5);
  cube4->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  cube4->SetScale(Vector3(100.0f, 1.0f, 100.0f));
  cube4->GetComponent<MeshComponent>()->SetBloomed(false);
  mGame->AddAlwaysActive(cube4);

  auto testEnemyGroup = new EnemyGroup(mGame, {{0, 100}, {1, 100}, {2, 100}});
  testEnemyGroup->SetPosition(Vector3(-10.0f, 1.0f, -10.0f));
}

void TestSceneB::Initialize() {
  std::cout << "Initializing TestSceneB..." << std::endl;

  SynthEngine::setChannels({{0, 24},  // Acoustic Guitar (nylon)
                            {0, 40},  // Violin
                            {0, 21},  // Accordion
                            {0, 43},  // Contrabass
                            {0, 73},  // Flute
                            {0, 71},  // Clarinet
                            {0, 56},  // Trumpet
                            {0, 46},  // Harp
                            {0, 0},   // Always active: Piano
                            {128, 0}, // Battle drums
                            {0, 49},  // Always active: Slow strings
                            {128, 1}, // Always Active: song drums
                            {0, 0},   // Player Channel: Piano
                            {0, 42},  // SFX1
                            {0, 11},  // SFX2
                            {0, 0}}); // SFX3

  // Creating the Player actor
  mGame->SetPlayer(new Player(mGame));
  mGame->GetPlayer()->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
  mGame->GetPlayer()->GetComponent<SpriteComponent>()->SetBloomed(false);
  mGame->GetRenderer()->SetIsDark(false);

  MIDIPlayer::loadSong("assets/songs/a1.mid", true);

  // Creating the battke system
  mGame->SetBattleSystem(new BattleSystem(mGame));

  MIDIPlayer::setChannelTranspose(11, -60);

  std::cout << MIDIPlayer::getChannels()[10].notes.size() << std::endl;

  MIDIPlayer::play();

  auto testMario = new MarioActor(mGame);
  testMario->SetPosition(Vector3(-5.0f, 1.0f, 0.0f));

  auto testGoomba = new GoombaActor(mGame);
  testGoomba->SetPosition(Vector3(4.0f, 1.0f, -12.0f));

  auto obbTest1 = new OBBTestActor(mGame);
  obbTest1->SetPosition(Vector3(5.0f, 1.0f, 12.0f));
  obbTest1->SetRotation(Quaternion(Vector3::UnitY, Math::ToRadians(45.0f)));

  auto cube2 = new CubeActor(mGame, Color::LightBlue, 0);
  cube2->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  cube2->SetScale(Vector3(32.0f, 1.0f, 32.0f));
  mGame->AddAlwaysActive(cube2);

  MultiDrawablesActor *multiDrawablesActor = new MultiDrawablesActor(mGame);
  multiDrawablesActor->SetPosition(Vector3(-10.0f, 1.0f, 10.0f));

  EnemyGroup *enemies = new EnemyGroup(mGame, {{0, 100}});
  enemies->SetPosition(Vector3(-10.0f, 1.0f, -10.0f));

  EnemyGroup *enemies2 =
      new EnemyGroup(mGame, {{4, 100}, {5, 100}, {6, 100}, {7, 100}});
  enemies2->SetPosition(Vector3(10.0f, 1.0f, -10.0f));
}