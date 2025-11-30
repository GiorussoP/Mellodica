#include "Scene.hpp"
#include "CSV.h"
#include "Game.hpp"
#include <fstream>
#include <iostream>

#include "actors/TestActors.hpp"

void Scene::Cleanup() {
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
      auto wall = new CubeActor(mGame, Color::White);
      wall->SetPosition(Vector3(x, 0.0f, z));
      wall->SetScale(Vector3(size_x, 1.0f, size_y));
      break;
    }
    case 1: {
      auto wall = new CubeActor(mGame, Color::Blue);
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