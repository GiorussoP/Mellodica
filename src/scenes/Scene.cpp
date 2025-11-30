#include "Scene.hpp"
#include "CSV.h"
#include "Game.hpp"
#include <fstream>
#include <iostream>

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

  MapReader mapReader(levelPath);

  for (const auto actor : mapReader.GetMapActors()) {

    unsigned int type = static_cast<int>(std::get<3>(actor));

    float x = std::get<0>(actor).x;
    float z = std::get<0>(actor).y;

    float size_x = std::get<1>(actor);
    float size_y = std::get<2>(actor);

    std::cout << "Scene::LoadLevel: Spawning actor of type " << type << " at ("
              << x << ", " << z << ") with size (" << size_x << ", " << size_y
              << ")" << std::endl;

    switch (type) {
    case 0: {
      auto ground = new GroundActor(mGame, Color::White, 3);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 1: {
      auto house = new HouseActor(mGame);
      float vertical_size = (size_x + size_y) / 4.0f;
      house->SetPosition(Vector3(x, 0.5f + vertical_size / 2.0f, z));
      house->SetScale(Vector3(size_x / 1.5f, vertical_size, size_y / 1.5f));
      break;
    }
    case 2: {
      auto wall = new RockWall(mGame);
      wall->SetPosition(Vector3(x, 2.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 3: {
      auto ground = new GroundActor(mGame, Color::White, 0);
      ground->SetPosition(Vector3(x, 0.0f, z));
      ground->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 4: {
      auto wall = new RockCubeActor(mGame);
      wall->SetPosition(Vector3(x, 1.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    default:
      std::cerr << "Scene::LoadLevel: Unknown ActorMap type: " << type
                << std::endl;
      break;
    }
  }
}