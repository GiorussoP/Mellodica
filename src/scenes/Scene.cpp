#include "Scene.hpp"
#include "CSV.h"
#include "Game.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "EnemyGroup.hpp"
#include "Ghost.hpp"
#include "Renderer.hpp"
#include "actors/PuzzleActors.hpp"
#include "actors/SceneActors.hpp"

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

int Scene::GetEnemyBitmask(int encounterNumber) {
  // Ensure we're in valid range [0, 255]
  encounterNumber = encounterNumber % 256;

  // First 8 encounters are single enemy types (power of 2)
  if (encounterNumber < 8) {
    return 1
           << encounterNumber; // 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
  }

  // For remaining encounters, generate all 256 combinations sorted by:
  // 1. Number of bits set (Hamming weight)
  // 2. Numerical value (for stable ordering within same bit count)
  static std::vector<int> sortedCombinations;

  // Initialize the sorted list only once
  if (sortedCombinations.empty()) {
    // Generate all 256 combinations (0-255)
    for (int i = 0; i < 256; i++) {
      sortedCombinations.push_back(i);
    }

    // Sort by Hamming weight (number of 1 bits), then by value
    std::sort(sortedCombinations.begin(), sortedCombinations.end(),
              [](int a, int b) {
                // Count bits in each number
                int bitsA = __builtin_popcount(a);
                int bitsB = __builtin_popcount(b);

                // Sort by bit count first
                if (bitsA != bitsB) {
                  return bitsA < bitsB;
                }

                // If same bit count, sort by value
                return a < b;
              });
  }

  // Return the combination at this encounter index
  return sortedCombinations[encounterNumber];
}

void Scene::LoadLevel(const std::string &levelPath) {
  // Empty default implementation - should be overridden by subclasses
}
