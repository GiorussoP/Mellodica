#include "Scene.hpp"
#include "CSV.h"
#include "Game.hpp"
#include <fstream>
#include <iostream>

#include "EnemyGroup.hpp"
#include "Ghost.hpp"
#include "Renderer.hpp"
#include "actors/TestActors.hpp"

void Scene::Cleanup() {
  // Remove all HUD elements from renderer
  auto renderer = mGame->GetRenderer();
  if (renderer) {
    // Remove all HUDElements
    for (auto actor : mActors) {
      HUDElement *hud = dynamic_cast<HUDElement *>(actor);
      if (hud) {
        renderer->RemoveUIElement(hud);
      }
    }
  }

  // Iterate over a copy since deletion modifies the set
  std::vector<Actor *> actorsToDelete(mActors.begin(), mActors.end());

  for (auto actor : actorsToDelete) {
    actor->SetState(ActorState::Destroy);
    // delete actor; // Actor destructor will call mGame->RemoveActor(this)
  }

  mActors.clear(); // Clear the set (should already be empty)
}

void Scene::LoadLevel(const std::string &levelPath) {

  std::cout << "Loading level from: " << levelPath << std::endl;

  std::cout << "Loading terrain..." << std::endl;
  MapReader mapReader(levelPath + "_terrain.csv");

  for (const auto actor : mapReader.GetMapActors()) {

    int type = static_cast<int>(std::get<3>(actor));

    float x = std::get<0>(actor).y;
    float z = -std::get<0>(actor).x;

    float size_x = std::get<2>(actor);
    float size_y = std::get<1>(actor);

    switch (type) {
    case 87: {
      auto ground = new RockCubeActor(mGame);
      ground->SetPosition(Vector3(x, 1.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 82: {
      auto house = new HouseActor(mGame);
      float vertical_size = (size_x + size_y) / 4.0f;
      house->SetPosition(Vector3(x, 0.5f + vertical_size / 2.0f, z));
      house->SetScale(Vector3(size_x / 1.5f, vertical_size, size_y / 1.5f));
      break;
    }
    case 84: {
      auto wall = new RockWall(mGame);
      wall->SetPosition(Vector3(x, 2.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 85: {
      auto wall = new GrassWall(mGame);
      wall->SetPosition(Vector3(x, 2.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 129: {
      auto ground = new GroundActor(mGame, Color::White, 3);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 104: {
      auto ground = new GroundActor(mGame, Color::White, 2);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 107: {
      auto ground = new GroundActor(mGame, Color::White, 10);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 110: {
      auto ground = new GroundActor(mGame, Color::White, 25);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 88: {
      auto ground = new GroundActor(mGame, Color::White, 57);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 95: {
      auto ground = new GroundActor(mGame, Color::White, 27);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 130: {
      auto ground = new GroundActor(mGame, Color::White, 15);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 113: {
      auto ground = new GroundActor(mGame, Color::White, 40);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 119: {
      auto ground = new GroundActor(mGame, Color::White, 48);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 101: {
      auto ground = new GroundActor(mGame, Color::White, 64);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 103: {
      auto ground = new GroundActor(mGame, Color::White, 9);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 128: {
      auto ground = new GroundActor(mGame, Color::White, 24);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 111: {
      auto ground = new GroundActor(mGame, Color::White, 18);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 114: {
      auto ground = new GroundActor(mGame, Color::White, 33);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 117: {
      auto ground = new GroundActor(mGame, Color::White, 12);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 122: {
      auto ground = new GroundActor(mGame, Color::White, 20);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 125: {
      auto ground = new GroundActor(mGame, Color::White, 56);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 127: {
      auto ground = new GroundActor(mGame, Color::White, 42);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 98: {
      auto ground = new GroundActor(mGame, Color::White, 21);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 99: {
      auto ground = new GroundActor(mGame, Color::White, 14);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 100: {
      auto ground = new GroundActor(mGame, Color::White, 7);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 89: {
      auto ground = new GroundActor(mGame, Color::White, 0);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 120: {
      auto ground = new GroundActor(mGame, Color::White, 41);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 109: {
      auto ground = new GroundActor(mGame, Color::White, 32);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 123: {
      auto ground = new GroundActor(mGame, Color::White, 13);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 126: {
      auto ground = new GroundActor(mGame, Color::White, 49);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 112: {
      auto ground = new GroundActor(mGame, Color::White, 4);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 115: {
      auto ground = new GroundActor(mGame, Color::White, 26);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 131: {
      auto ground = new GroundActor(mGame, Color::White, 11);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 92: {
      auto ground = new GroundActor(mGame, Color::White, 8);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 97: {
      auto ground = new GroundActor(mGame, Color::White, 28);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 106: {
      auto ground = new GroundActor(mGame, Color::White, 17);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 118: {
      auto ground = new GroundActor(mGame, Color::White, 5);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 121: {
      auto ground = new GroundActor(mGame, Color::White, 34);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 124: {
      auto ground = new GroundActor(mGame, Color::White, 6);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 90: {
      auto wall = new DirtCubeActor(mGame);
      wall->SetPosition(Vector3(x, 1.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 86: {
      auto cube = new GrassCubeActor(mGame);
      cube->SetPosition(Vector3(x, 1.0f, z));
      cube->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 80: {
      auto cube = new SandCubeActor(mGame);
      cube->SetPosition(Vector3(x, 1.0f, z));
      cube->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 81: {
      auto wall = new EntranceWall(mGame);
      wall->SetPosition(Vector3(x, 2.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 83: {
      auto wall = new WindowWall(mGame);
      wall->SetPosition(Vector3(x, 2.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case 91: {
      auto wall = new DoorWall(mGame);
      wall->SetPosition(Vector3(x, 2.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }

    case -1: {
      // Empty space, do nothing
      break;
    }
    default:
      std::cerr << "Scene::LoadLevel: Unknown Terrain type: " << type
                << std::endl;
      break;
    }
  }

  std::cout << "Loading objects..." << std::endl;
  MapReader objMapReader(levelPath + "_objects.csv", false);

  short int enemy = 1;
  for (const auto actor : objMapReader.GetMapActors()) {

    int type = static_cast<int>(std::get<3>(actor));

    float x = std::get<0>(actor).y;
    float z = -std::get<0>(actor).x;

    switch (type) {
    case 6: {
      auto tree = new TreeActor(mGame);
      tree->SetPosition(Vector3(x, 1.0f, z));
      break;
    }
    case 5: {
      auto rock = new MediumRockActor(mGame);
      rock->SetPosition(Vector3(x, 1.0f, z));
      break;
    }

    case 4: {
      auto rock = new SmallRockActor(mGame);
      rock->SetPosition(Vector3(x, 1.0f, z));
      break;
    }

    case 3: {
      auto grass = new GrassActorC(mGame);
      grass->SetPosition(Vector3(x, 1.0f, z));
      break;
    }
    case 2: {
      auto grass = new GrassActorB(mGame);
      grass->SetPosition(Vector3(x, 1.0f, z));
      break;
    }

    case 1: {
      auto grass = new GrassActorA(mGame);
      grass->SetPosition(Vector3(x, 1.0f, z));
      break;
    }

    case 0: {
      auto bush = new BushActor(mGame);
      bush->SetPosition(Vector3(x, 1.0f, z));
      break;
    }

    case 7: {
      if (!mGame->GetPlayer()) {
        mGame->SetPlayer(new Player(mGame));
      }
      mGame->GetPlayer()->SetPosition(Vector3(x, 1.0f, z));
      if (mGame->GetCamera()) {
        mGame->GetCamera()->SetMode(CameraMode::Isometric);
        mGame->GetCamera()->SetPosition(mGame->GetPlayer()->GetPosition());
        mGame->GetCamera()->SetIsometricDirection(
            IsometricDirections::NorthEast);
      }
      break;
    }

    case 9: {
      static int enemyCounter = 1;
      int enemyValue = enemyCounter++;
      std::vector<Combatant *> enemies;
      for (int i = 0; i < 8; i++) {
        if (enemyValue & (1 << i)) {
          enemies.push_back(new Ghost(mGame, i, 500));
        }
      }
      auto enemy = new EnemyGroup(mGame, enemies);
      enemy->SetPosition(Vector3(x, 1.0f, z));
      break;
    }
    case -1: {
      // Empty space, do nothing
      break;
    }
    default:
      std::cerr << "Scene::LoadLevel: Unknown ObjectMap type: " << type
                << std::endl;
      break;
    }
  }
}