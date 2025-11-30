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

void Scene::LoadLevel(const std::string &levelPath, unsigned int width,
                      unsigned int height) {

  // For now, we'll just print the level path
  std::cout << "Loading level from: " << levelPath << std::endl;

  std::ifstream f(levelPath);

  int **levelData = new int *[height];
  for (int i = 0; i < height; i++)
    levelData[i] = new int[width];

  if (!f.is_open()) {
    SDL_Log("Erro lendo nível.");
  }

  std::string line;
  int i = 0;
  while (std::getline(f, line)) {

    std::vector<int> row = CSVHelper::Split(line);

    int j = 0;
    for (int n : row) {
      levelData[i][j] = n;
      j++;
    }
    std::cout << '\n';
    ++i;
  }
  f.close();

  int y = 0;
  for (int i = 0; i < height; ++i) {
    int x = 0;
    for (int j = 0; j < width; ++j) {
      switch (levelData[i][j]) {
      case -1:
        break;
      case 0: {
        auto block = new GrassCubeActor(mGame);
        block->SetPosition(
            Vector3(static_cast<float>(x), static_cast<float>(y), 0.0f));
        break;
      }
      case 1: {

        break;
      }
      case 2: {

        break;
      }
      case 4: {
        break;
      }
      case 6: {

        break;
      }
      case 8: {

        break;
      }
      case 9: {

        break;
      }
      case 10: {

        break;
      }
      case 12: {

        break;
      }
      case 13: {

        break;
      }
      case 15: {

        break;
      }
      case 16: {

        break;
      }

      default:
        break;
      }
    }
  }
}