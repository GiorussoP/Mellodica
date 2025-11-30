#include "TestScene.hpp"
#include "../../include/UI/HUDElement.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "Renderer.hpp"
#include "actors/Combatant.hpp"
#include "actors/EnemyGroup.hpp"
#include "actors/Ghost.hpp"
#include "actors/Player.hpp"
#include "actors/RobotA.hpp"
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

  auto floor = new GroundActor(mGame, Color::White, 0);
  floor->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  floor->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));

  floor->GetComponent<MeshComponent>()->SetBloomed(false);
  mGame->AddAlwaysActive(floor);

  std::vector<Combatant *> ghosts1 = {new Ghost(mGame, 0, 1000),
                                      new Ghost(mGame, 1, 1000),
                                      new Ghost(mGame, 2, 1000)};
  auto testEnemyGroup = new EnemyGroup(mGame, ghosts1);
  testEnemyGroup->SetPosition(Vector3(-10.0f, 1.0f, -10.0f));

  std::vector<Combatant *> ghosts2 = {new Ghost(mGame, 3, 1000)};
  auto enemyGroup2 = new EnemyGroup(mGame, ghosts2);
  enemyGroup2->SetPosition(Vector3(10.0f, 1.0f, -10.0f));

  std::vector<Combatant *> ghosts3 = {new Ghost(mGame, 4, 1000),
                                      new Ghost(mGame, 5, 1000)};
  auto enemyGroup3 = new EnemyGroup(mGame, ghosts3);
  enemyGroup3->SetPosition(Vector3(15.0f, 1.0f, 10.0f));

  std::vector<Combatant *> ghosts4 = {new Ghost(mGame, 6, 1000)};
  auto enemyGroup4 = new EnemyGroup(mGame, ghosts4);
  enemyGroup4->SetPosition(Vector3(-15.0f, 1.0f, -15.0f));

  std::vector<Combatant *> ghosts5 = {new Ghost(mGame, 7, 1000)};
  auto enemyGroup5 = new EnemyGroup(mGame, ghosts5);
  enemyGroup5->SetPosition(Vector3(0.0f, 1.0f, -20.0f));

  std::vector<Combatant *> ghosts6 = {
      new Ghost(mGame, 0, 1000), new Ghost(mGame, 1, 1000),
      new Ghost(mGame, 2, 1000), new Ghost(mGame, 3, 1000)};
  auto enemyGroup6 = new EnemyGroup(mGame, ghosts6);
  enemyGroup6->SetPosition(Vector3(20.0f, 1.0f, -5.0f));

  std::vector<Combatant *> ghosts7 = {new Ghost(mGame, 4, 1000),
                                      new Ghost(mGame, 5, 1000),
                                      new Ghost(mGame, 6, 1000)};
  auto enemyGroup7 = new EnemyGroup(mGame, ghosts7);
  enemyGroup7->SetPosition(Vector3(-20.0f, 1.0f, 5.0f));

  std::vector<Combatant *> ghosts8 = {new Ghost(mGame, 7, 1000),
                                      new Ghost(mGame, 0, 1000),
                                      new Ghost(mGame, 1, 1000)};
  auto enemyGroup8 = new EnemyGroup(mGame, ghosts8);
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

  LoadLevel("assets/levels/levelTeste.csv");

  /**/
  auto grassCube = new GrassCubeActor(mGame);
  grassCube->SetPosition(Vector3(2.0f, 1.0f, 25.0f));

  auto rockCube = new RockCubeActor(mGame);
  rockCube->SetPosition(Vector3(-2.0f, 1.0f, 25.0f));
  rockCube->SetScale(Vector3(3.0f, 1.0f, 22.0f));

  auto dirtCube = new DirtCubeActor(mGame);
  dirtCube->SetPosition(Vector3(20.0f, 1.0f, 25.0f));

  auto grassWall = new GrassWall(mGame);
  grassWall->SetPosition(Vector3(25.0f, 2.0f, 25.0f));

  auto rockWall = new RockWall(mGame);
  rockWall->SetPosition(Vector3(27.0f, 2.0f, 25.0f));

  auto doorWall = new DoorWall(mGame);
  doorWall->SetPosition(Vector3(29.0f, 2.0f, 25.0f));

  auto windowWall = new WindowWall(mGame);
  windowWall->SetPosition(Vector3(31.0f, 2.0f, 25.0f));

  auto entranceWall = new EntranceWall(mGame);
  entranceWall->SetPosition(Vector3(33.0f, 2.0f, 25.0f));

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
  obbTest1->SetPosition(Vector3(5.0f, 2.0f, 12.0f));
  obbTest1->SetRotation(Quaternion(Vector3::UnitY, Math::ToRadians(45.0f)));

  std::vector<Combatant *> ghostsB1 = {
      new Ghost(mGame, 4, 1000), new Ghost(mGame, 5, 1000),
      new Ghost(mGame, 6, 1000), new Ghost(mGame, 7, 1000)};
  EnemyGroup *enemies = new EnemyGroup(mGame, ghostsB1);
  enemies->SetPosition(Vector3(10.0f, 1.0f, 30.0f));

  std::vector<Combatant *> ghostsB2 = {new Ghost(mGame, 8, 1000),
                                       new Ghost(mGame, 9, 1000)};
  EnemyGroup *enemies2 = new EnemyGroup(mGame, ghostsB2);
  enemies2->SetPosition(Vector3(20.0f, 1.0f, 8.0f));

  std::vector<Combatant *> ghostsB3 = {new Ghost(mGame, 10, 1000),
                                       new Ghost(mGame, 11, 1000),
                                       new Ghost(mGame, 12, 1000)};
  EnemyGroup *enemies3 = new EnemyGroup(mGame, ghostsB3);
  enemies3->SetPosition(Vector3(35.0f, 1.0f, 15.0f));

  std::vector<Combatant *> robotsB1 = {new RobotA(mGame, 0, 1000),
                                       new RobotA(mGame, 1, 1000)};
  EnemyGroup *robotEnemies1 = new EnemyGroup(mGame, robotsB1);
  robotEnemies1->SetPosition(Vector3(5.0f, 1.0f, 30.0f));

  std::vector<Combatant *> robotsB2 = {new RobotA(mGame, 2, 1000),
                                       new RobotA(mGame, 3, 1000),
                                       new RobotA(mGame, 4, 1000)};
  EnemyGroup *robotEnemies2 = new EnemyGroup(mGame, robotsB2);
  robotEnemies2->SetPosition(Vector3(20.0f, 1.0f, 30.0f));

  std::vector<Combatant *> mixedGroup = {
      new Ghost(mGame, 5, 1000), new RobotA(mGame, 6, 1000),
      new Ghost(mGame, 7, 1000), new RobotA(mGame, 8, 1000)};
  EnemyGroup *mixedEnemies = new EnemyGroup(mGame, mixedGroup);
  mixedEnemies->SetPosition(Vector3(10.0f, 1.0f, 5.0f));
}