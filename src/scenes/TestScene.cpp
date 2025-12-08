#include "scenes/TestScene.hpp"
#include "AssetLoader.hpp"
#include "UI/HUDElement.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "render/Renderer.hpp"
#include "actors/Combatant.hpp"
#include "actors/EnemyGroup.hpp"
#include "actors/Ghost.hpp"
#include "actors/Player.hpp"
#include "actors/PuzzleActors.hpp"
#include "actors/RobotA.hpp"
#include "actors/SceneActors.hpp"

#include <iostream>

void TestSceneA::Initialize() {

  std::cout << "Initializing TestSceneA..." << std::endl;
  mGame->GetRenderer()->SetIsDark(true);

  MIDIPlayer::loadSong2a();

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

  // Setting camera
  mGame->GetCamera()->SetMode(CameraMode::Isometric);
  mGame->GetCamera()->SetPosition(mGame->GetPlayer()->GetPosition());
  mGame->GetCamera()->SetCameraForward(
      Vector3::Normalize(Vector3(0.0f, -1.0f, -1.0f)));

  auto floor = new GroundActor(mGame, Color::White, 0);
  floor->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  floor->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));

  floor->GetComponent<MeshComponent>()->SetBloomed(false);
  mGame->AddAlwaysActive(floor);

  std::vector<Combatant *> ghosts1 = {new Ghost(mGame, 0, 500),
                                      new Ghost(mGame, 1, 500),
                                      new Ghost(mGame, 2, 500)};
  auto testEnemyGroup = new EnemyGroup(mGame, ghosts1);
  testEnemyGroup->SetPosition(Vector3(-10.0f, 1.0f, -10.0f));

  std::vector<Combatant *> ghosts2 = {new Ghost(mGame, 3, 500)};
  auto enemyGroup2 = new EnemyGroup(mGame, ghosts2);
  enemyGroup2->SetPosition(Vector3(10.0f, 1.0f, -10.0f));

  std::vector<Combatant *> ghosts3 = {new Ghost(mGame, 4, 500),
                                      new Ghost(mGame, 5, 500)};
  auto enemyGroup3 = new EnemyGroup(mGame, ghosts3);
  enemyGroup3->SetPosition(Vector3(15.0f, 1.0f, 10.0f));

  std::vector<Combatant *> ghosts4 = {new Ghost(mGame, 6, 500)};
  auto enemyGroup4 = new EnemyGroup(mGame, ghosts4);
  enemyGroup4->SetPosition(Vector3(-15.0f, 1.0f, -15.0f));

  std::vector<Combatant *> ghosts5 = {new Ghost(mGame, 7, 500)};
  auto enemyGroup5 = new EnemyGroup(mGame, ghosts5);
  enemyGroup5->SetPosition(Vector3(0.0f, 1.0f, -20.0f));

  std::vector<Combatant *> ghosts6 = {
      new Ghost(mGame, 0, 500), new Ghost(mGame, 1, 500),
      new Ghost(mGame, 2, 500), new Ghost(mGame, 3, 500)};
  auto enemyGroup6 = new EnemyGroup(mGame, ghosts6);
  enemyGroup6->SetPosition(Vector3(20.0f, 1.0f, -5.0f));

  std::vector<Combatant *> ghosts7 = {new Ghost(mGame, 4, 500),
                                      new Ghost(mGame, 5, 500),
                                      new Ghost(mGame, 6, 500)};
  auto enemyGroup7 = new EnemyGroup(mGame, ghosts7);
  enemyGroup7->SetPosition(Vector3(-20.0f, 1.0f, 5.0f));

  std::vector<Combatant *> ghosts8 = {new Ghost(mGame, 7, 500),
                                      new Ghost(mGame, 0, 500),
                                      new Ghost(mGame, 1, 500)};
  auto enemyGroup8 = new EnemyGroup(mGame, ghosts8);
  enemyGroup8->SetPosition(Vector3(5.0f, 1.0f, 20.0f));

  SDL_Log("Saving state for test");
  mGame->SaveState();

  SDL_Log("Loading state for test");
  auto myMap = mGame->LoadState();

  for (const auto& [key, value] : myMap) {
      std::cout << key << " => " << value << '\n';
  }
}

void TestSceneB::Initialize() {
  std::cout << "Initializing TestSceneB..." << std::endl;

  mGame->GetRenderer()->SetIsDark(false);

  // Load song before creating battle system
  MIDIPlayer::loadSong0();

  LoadLevel(getAssetPath("levels/level1"));

  // Creating the battle system
  mGame->SetBattleSystem(new BattleSystem(mGame));
  MIDIPlayer::play();

  /*
  auto tree = new TreeActor(mGame);
  tree->SetPosition(Vector3(16.0f, 1.0f, 11.0f));

  auto tree2 = new TreeActor(mGame);
  tree2->SetPosition(Vector3(11.0f, 1.0f, 18.0f));

  auto tree3 = new TreeActor(mGame);
  tree3->SetPosition(Vector3(18.0f, 1.0f, 25.0f));

  auto rock = new MediumRockActor(mGame);
  rock->SetPosition(Vector3(22.0f, 1.0f, 25.0f));

  auto rock1 = new MediumRockActor(mGame);
  rock1->SetPosition(Vector3(13.0f, 1.0f, 27.0f));

  auto rock2 = new SmallRockActor(mGame);
  rock2->SetPosition(Vector3(24.0f, 1.0f, 22.0f));

  auto rock3 = new SmallRockActor(mGame);
  rock3->SetPosition(Vector3(23.0f, 1.0f, 20.0f));

  auto rock4 = new SmallRockActor(mGame);
  rock4->SetPosition(Vector3(26.0f, 1.0f, 12.0f));

  auto DoorWall1 = new DoorWall(mGame);
  DoorWall1->SetPosition(Vector3(8.5f, 2.0f, 26.9f));
  DoorWall1->SetScale(Vector3(1.0f, 1.0f, 1.0f));

  // Some grass around the player

  auto grass1 = new GrassActorA(mGame);
  grass1->SetPosition(Vector3(10.0f, 1.0f, 14.0f));
  auto grass2 = new GrassActorB(mGame);
  grass2->SetPosition(Vector3(15.0f, 1.0f, 17.0f));
  auto grass3 = new GrassActorA(mGame);
  grass3->SetPosition(Vector3(16.0f, 1.0f, 14.0f));

  auto grass4 = new GrassActorA(mGame);
  grass4->SetPosition(Vector3(20.0f, 1.0f, 18.0f));

  // Simplest group enemy

  auto enemy1 = new EnemyGroup(mGame, {new RobotA(mGame, 0, 500)});
  enemy1->SetPosition(Vector3(26.0f, 1.0f, 17.0f));

  auto enemy2 = new EnemyGroup(
      mGame, {new Ghost(mGame, 1, 500), new RobotA(mGame, 2, 500)});
  enemy2->SetPosition(Vector3(38.0f, 1.0f, 15.0f));

  auto enemy3 =
      new EnemyGroup(mGame, {new Ghost(mGame, 3, 500), new Ghost(mGame, 4, 500),
                             new RobotA(mGame, 5, 500)});
  enemy3->SetPosition(Vector3(30.0f, 1.0f, 35.0f));

  // Random elements
  auto grassCube = new RockCubeActor(mGame);
  grassCube->SetPosition(Vector3(38.0f, 1.0f, 23.0f));

  auto dirtCube = new RockCubeActor(mGame);
  dirtCube->SetPosition(Vector3(35.0f, 1.0f, 25.0f));

  auto windowWall = new WindowWall(mGame);
  windowWall->SetPosition(Vector3(26.1f, 2.0f, 31.0f));
  windowWall->SetScale(Vector3(1.0f, 1.0f, 9.0f));

  // Some bushes
  auto bush1 = new BushActor(mGame);
  bush1->SetPosition(Vector3(35.0f, 1.0f, 18.0f));

  auto bush2 = new BushActor(mGame);
  bush2->SetPosition(Vector3(40.0f, 1.0f, 37.0f));

  auto bush3 = new BushActor(mGame);
  bush3->SetPosition(Vector3(42.0f, 1.0f, 29.0f));

  auto enemy4 = new EnemyGroup(mGame, {new RobotA(mGame, 6, 500),
                                       new RobotA(mGame, 7, 500),
                                       new RobotA(mGame, 8, 500)});

  enemy4->SetPosition(Vector3(47.0f, 1.0f, 47.0f));

  auto enemy5 = new EnemyGroup(
      mGame, {new Ghost(mGame, 0, 500), new Ghost(mGame, 2, 500),
              new Ghost(mGame, 4, 500), new Ghost(mGame, 6, 500)});
  enemy5->SetPosition(Vector3(12.0f, 1.0f, 42.0f));

  // some bushes near enemy5
  auto bush4 = new BushActor(mGame);
  bush4->SetPosition(Vector3(9.0f, 1.0f, 33.0f));
  auto bush5 = new BushActor(mGame);
  bush5->SetPosition(Vector3(10.0f, 1.0f, 47.0f));
  auto bush6 = new BushActor(mGame);

  auto exit = new TriggerActor(mGame);
  exit->SetPosition(Vector3(3.0f, 1.0f, 43.0f));

  /*
  auto grassWall = new GrassWall(mGame);
  grassWall->SetPosition(Vector3(25.0f, 2.0f, 25.0f));

  auto rockWall = new RockWall(mGame);
  rockWall->SetPosition(Vector3(27.0f, 2.0f, 25.0f));

  auto doorWall = new DoorWall(mGame);
  doorWall->SetPosition(Vector3(29.0f, 2.0f, 25.0f));

  auto windowWall = new WindowWall(mGame);
  windowWall->SetPosition(Vector3(31.0f, 2.0f, 25.0f));

  auto entranceWall = new EntranceWall(mGame);
  entranceWall->SetPosition(Vector3(40.0f, 2.0f, 25.0f));

  auto tree = new TreeActor(mGame);
  tree->SetPosition(Vector3(30.0f, 1.0f, 23.0f));

  auto smallRock = new SmallRockActor(mGame);
  smallRock->SetPosition(Vector3(32.0f, 1.0f, 23.0f));

  auto mediumRock = new MediumRockActor(mGame);
  mediumRock->SetPosition(Vector3(28.0f, 1.0f, 23.0f));

  auto bush = new BushActor(mGame);
  bush->SetPosition(Vector3(26.0f, 1.0f, 23.0f));

  auto grassA = new GrassActorA(mGame);
  grassA->SetPosition(Vector3(24.0f, 1.0f, 23.0f));

  auto grassB = new GrassActorB(mGame);
  grassB->SetPosition(Vector3(22.0f, 1.0f, 23.0f));

  auto grassC = new GrassActorC(mGame);
  grassC->SetPosition(Vector3(20.0f, 1.0f, 23.0f));

  auto obbTest1 = new OBBTestActor(mGame);
  obbTest1->SetPosition(Vector3(5.0f, 1.0f, 12.0f));
  obbTest1->SetRotation(Quaternion(Vector3::UnitY, Math::ToRadians(45.0f)));

  std::vector<Combatant *> ghostsB1 = {
      new Ghost(mGame, 4, 500), new Ghost(mGame, 5, 500),
      new Ghost(mGame, 6, 500), new Ghost(mGame, 7, 500)};
  EnemyGroup *enemies = new EnemyGroup(mGame, ghostsB1);
  enemies->SetPosition(Vector3(10.0f, 1.0f, 30.0f));

  std::vector<Combatant *> ghostsB2 = {new Ghost(mGame, 8, 500),
                                       new Ghost(mGame, 9, 500)};
  EnemyGroup *enemies2 = new EnemyGroup(mGame, ghostsB2);
  enemies2->SetPosition(Vector3(20.0f, 1.0f, 8.0f));

  std::vector<Combatant *> ghostsB3 = {new Ghost(mGame, 10, 500),
                                       new Ghost(mGame, 11, 500),
                                       new Ghost(mGame, 12, 500)};
  EnemyGroup *enemies3 = new EnemyGroup(mGame, ghostsB3);
  enemies3->SetPosition(Vector3(35.0f, 1.0f, 15.0f));

  std::vector<Combatant *> robotsB1 = {new RobotA(mGame, 0, 500),
                                       new RobotA(mGame, 1, 500)};
  EnemyGroup *robotEnemies1 = new EnemyGroup(mGame, robotsB1);
  robotEnemies1->SetPosition(Vector3(5.0f, 1.0f, 30.0f));

  std::vector<Combatant *> robotsB2 = {new RobotA(mGame, 2, 500),
                                       new RobotA(mGame, 3, 500),
                                       new RobotA(mGame, 4, 500)};
  EnemyGroup *robotEnemies2 = new EnemyGroup(mGame, robotsB2);
  robotEnemies2->SetPosition(Vector3(20.0f, 1.0f, 30.0f));

  std::vector<Combatant *> mixedGroup = {
      new Ghost(mGame, 5, 500), new RobotA(mGame, 6, 500),
      new Ghost(mGame, 7, 500), new RobotA(mGame, 8, 500)};
  EnemyGroup *mixedEnemies = new EnemyGroup(mGame, mixedGroup);
  mixedEnemies->SetPosition(Vector3(50.0f, 1.0f, 5.0f));
  */
}
