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

  MIDIPlayer::loadSong2();

  // Creating the battke system
  mGame->SetBattleSystem(new BattleSystem(mGame));

  MIDIPlayer::play();

  // Some actors for testing

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
  cube4->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
  cube4->GetComponent<MeshComponent>()->SetBloomed(false);
  mGame->AddAlwaysActive(cube4);

  auto testEnemyGroup =
      new EnemyGroup(mGame, {{0, 1000}, {1, 1000}, {2, 1000}});
  testEnemyGroup->SetPosition(Vector3(-10.0f, 1.0f, -10.0f));

  // Additional random enemy groups for testing
  auto enemyGroup2 = new EnemyGroup(mGame, {{3, 1000}});
  enemyGroup2->SetPosition(Vector3(10.0f, 1.0f, -10.0f));

  auto enemyGroup3 = new EnemyGroup(mGame, {{4, 1000}, {5, 1000}});
  enemyGroup3->SetPosition(Vector3(15.0f, 1.0f, 10.0f));

  auto enemyGroup4 = new EnemyGroup(mGame, {{6, 1000}});
  enemyGroup4->SetPosition(Vector3(-15.0f, 1.0f, -15.0f));

  auto enemyGroup5 = new EnemyGroup(mGame, {{7, 1000}});
  enemyGroup5->SetPosition(Vector3(0.0f, 1.0f, -20.0f));

  auto enemyGroup6 =
      new EnemyGroup(mGame, {{0, 1000}, {1, 1000}, {2, 1000}, {3, 1000}});
  enemyGroup6->SetPosition(Vector3(20.0f, 1.0f, -5.0f));

  auto enemyGroup7 = new EnemyGroup(mGame, {{4, 1000}, {5, 1000}, {6, 1000}});
  enemyGroup7->SetPosition(Vector3(-20.0f, 1.0f, 5.0f));

  auto enemyGroup8 = new EnemyGroup(mGame, {{7, 1000}, {0, 1000}, {1, 1000}});
  enemyGroup8->SetPosition(Vector3(5.0f, 1.0f, 20.0f));
}

void TestSceneB::Initialize() {
  std::cout << "Initializing TestSceneB..." << std::endl;

  // Creating the Player actor
  mGame->SetPlayer(new Player(mGame));
  mGame->GetPlayer()->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
  mGame->GetPlayer()->GetComponent<SpriteComponent>()->SetBloomed(false);
  mGame->GetRenderer()->SetIsDark(false);

  // Load song before creating battle system
  MIDIPlayer::loadSong1();

  // Creating the battle system
  mGame->SetBattleSystem(new BattleSystem(mGame));
  MIDIPlayer::play();

  std::cout << MIDIPlayer::getChannels()[10].notes.size() << std::endl;

  auto testMario = new MarioActor(mGame);
  testMario->SetPosition(Vector3(-5.0f, 1.0f, 0.0f));

  auto testGoomba = new GoombaActor(mGame);
  testGoomba->SetPosition(Vector3(4.0f, 1.0f, -12.0f));

  auto obbTest1 = new OBBTestActor(mGame);
  obbTest1->SetPosition(Vector3(5.0f, 1.0f, 12.0f));
  obbTest1->SetRotation(Quaternion(Vector3::UnitY, Math::ToRadians(45.0f)));

  auto cube2 = new CubeActor(mGame, Color::Cyan, 0);
  cube2->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  cube2->SetScale(Vector3(32.0f, 1.0f, 320.0f));
  mGame->AddAlwaysActive(cube2);

  MultiDrawablesActor *multiDrawablesActor = new MultiDrawablesActor(mGame);
  multiDrawablesActor->SetPosition(Vector3(-15.0f, 1.0f, 15.0f));

  EnemyGroup *enemies =
      new EnemyGroup(mGame, {{4, 1000}, {5, 1000}, {6, 1000}, {7, 1000}});
  enemies->SetPosition(Vector3(10.0f, 1.0f, -10.0f));

  EnemyGroup *enemies2 = new EnemyGroup(mGame, {{8, 1000}, {9, 1000}});
  enemies2->SetPosition(Vector3(15.0f, 1.0f, 10.0f));

  EnemyGroup *enemies3 =
      new EnemyGroup(mGame, {{10, 1000}, {11, 1000}, {12, 1000}});
  enemies3->SetPosition(Vector3(-15.0f, 1.0f, -15.0f));

  EnemyGroup *enemy0 = new EnemyGroup(mGame, {{0, 1000}});
  enemy0->SetPosition(Vector3(-10.0f, 1.0f, -10.0f));

  EnemyGroup *enemy1 = new EnemyGroup(mGame, {{1, 1000}});
  enemy1->SetPosition(Vector3(-10.0f, 1.0f, -20.0f));

  EnemyGroup *enemy2 = new EnemyGroup(mGame, {{2, 1000}});
  enemy2->SetPosition(Vector3(-10.0f, 1.0f, -30.0f));

  EnemyGroup *enemy3 = new EnemyGroup(mGame, {{3, 1000}});
  enemy3->SetPosition(Vector3(-10.0f, 1.0f, -40.0f));

  EnemyGroup *enemy4 = new EnemyGroup(mGame, {{4, 1000}});
  enemy4->SetPosition(Vector3(-10.0f, 1.0f, -50.0f));

  EnemyGroup *enemy5 = new EnemyGroup(mGame, {{5, 1000}});
  enemy5->SetPosition(Vector3(-10.0f, 1.0f, -60.0f));

  EnemyGroup *enemy6 = new EnemyGroup(mGame, {{6, 1000}});
  enemy6->SetPosition(Vector3(-10.0f, 1.0f, -70.0f));

  EnemyGroup *enemy7 = new EnemyGroup(mGame, {{7, 1000}});
  enemy7->SetPosition(Vector3(-10.0f, 1.0f, -80.0f));
}