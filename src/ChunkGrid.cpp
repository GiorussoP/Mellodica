#include "ChunkGrid.hpp"
#include "actors/Actor.hpp"
#include <algorithm>
#include <iostream>

ChunkGrid::ChunkGrid(const Vector3 &worldMin, const Vector3 &worldMax,
                     float cellSize)
    : mWorldMin(worldMin), mWorldMax(worldMax), mCellSize(cellSize) {
  // Calculate grid dimensions
  float worldWidth = worldMax.x - worldMin.x;
  float worldDepth = worldMax.z - worldMin.z;

  mGridWidth = static_cast<int>(std::ceil(worldWidth / cellSize));
  mGridDepth = static_cast<int>(std::ceil(worldDepth / cellSize));

  // Create cells
  int totalCells = mGridWidth * mGridDepth;
  mCells.resize(totalCells);

  // Initialize cell coordinates
  for (int z = 0; z < mGridDepth; z++) {
    for (int x = 0; x < mGridWidth; x++) {
      int index = z * mGridWidth + x;
      mCells[index].x = x;
      mCells[index].z = z;
    }
  }

  std::cout << "ChunkGrid created: " << mGridWidth << "x" << mGridDepth
            << " cells (" << totalCells << " total), cell size: " << cellSize
            << std::endl;
  std::cout << "World bounds: (" << mWorldMin.x << ", " << mWorldMin.z
            << ") to (" << mWorldMax.x << ", " << mWorldMax.z << ")"
            << std::endl;
}

ChunkGrid::~ChunkGrid() {
  // Clear all cells
  for (auto &cell : mCells) {
    cell.actors.clear();
  }
  mActorCellMap.clear();
}

void ChunkGrid::RegisterActor(Actor *actor) {
  if (!actor)
    return;

  int cellIndex = GetCellIndex(actor->GetPosition());

  // Check if valid cell
  if (cellIndex < 0 || cellIndex >= static_cast<int>(mCells.size())) {
    // Actor is outside grid bounds - skip it
    return;
  }

  // Add to cell
  mCells[cellIndex].actors.push_back(actor);
  mActorCellMap[actor] = cellIndex;
}

void ChunkGrid::UnregisterActor(Actor *actor) {
  if (!actor)
    return;

  auto it = mActorCellMap.find(actor);
  if (it == mActorCellMap.end()) {
    return; // Actor not in grid
  }

  int cellIndex = it->second;

  // CRITICAL FIX: Verify cellIndex is valid before accessing
  if (cellIndex < 0 || cellIndex >= static_cast<int>(mCells.size())) {
    // Cell index is invalid, but actor is in map - remove from map only
    mActorCellMap.erase(it);
    return;
  }

  // Remove from cell
  auto &cellActors = mCells[cellIndex].actors;
  auto actorIt = std::find(cellActors.begin(), cellActors.end(), actor);
  if (actorIt != cellActors.end()) {
    // Swap with last and pop for O(1) removal
    std::iter_swap(actorIt, cellActors.end() - 1);
    cellActors.pop_back();
  }

  // Remove from map
  mActorCellMap.erase(it);
}

void ChunkGrid::UpdateActor(Actor *actor) {
  if (!actor)
    return;

  auto it = mActorCellMap.find(actor);
  if (it == mActorCellMap.end()) {
    // Actor not registered yet - register it
    RegisterActor(actor);
    return;
  }

  int oldCellIndex = it->second;
  int newCellIndex = GetCellIndex(actor->GetPosition());

  // Check if actor moved to invalid cell
  if (newCellIndex < 0 || newCellIndex >= static_cast<int>(mCells.size())) {
    // Actor moved outside grid - unregister it
    UnregisterActor(actor);
    return;
  }

  // Check if cell changed
  if (oldCellIndex != newCellIndex) {
    // Remove from old cell
    auto &oldCellActors = mCells[oldCellIndex].actors;
    auto actorIt = std::find(oldCellActors.begin(), oldCellActors.end(), actor);
    if (actorIt != oldCellActors.end()) {
      std::iter_swap(actorIt, oldCellActors.end() - 1);
      oldCellActors.pop_back();
    }

    // Add to new cell
    mCells[newCellIndex].actors.push_back(actor);
    mActorCellMap[actor] = newCellIndex;
  }
}

std::vector<Actor *> ChunkGrid::GetVisibleActors(const Vector3 &cameraPos) {
  std::vector<Actor *> visibleActors;

  // Get camera's cell coordinates
  int camX, camZ;
  GetCellCoords(cameraPos, camX, camZ);

  /*
  static bool debugPrinted = false;
  if (!debugPrinted) {
      std::cout << "Camera cell coords: (" << camX << ", " << camZ << ")" <<
  std::endl; std::cout << "Grid dimensions: " << mGridWidth << "x" << mGridDepth
  << std::endl;

      // Count actors per cell to see distribution
      int nonEmptyCells = 0;
      int maxActorsInCell = 0;
      for (const auto& cell : mCells) {
          if (!cell.actors.empty()) {
              nonEmptyCells++;
              if (static_cast<int>(cell.actors.size()) > maxActorsInCell) {
                  maxActorsInCell = static_cast<int>(cell.actors.size());
              }
          }
      }
      std::cout << "Non-empty cells: " << nonEmptyCells << "/" << mCells.size()
  << std::endl; std::cout << "Max actors in single cell: " << maxActorsInCell <<
  std::endl;

      debugPrinted = true;
  }
  */

  // Collect actors from camera cell + 8 adjacent cells (3x3 grid)
  for (int dz = -1; dz <= 1; dz++) {
    for (int dx = -1; dx <= 1; dx++) {
      int x = camX + dx;
      int z = camZ + dz;

      // Check bounds
      if (x < 0 || x >= mGridWidth || z < 0 || z >= mGridDepth) {
        continue;
      }

      int cellIndex = CoordsToIndex(x, z);

      // Add all actors from this cell
      const auto &cellActors = mCells[cellIndex].actors;

      // CRITICAL FIX: Only add actors that are not marked for destruction
      // This prevents use-after-free if an actor was marked for deletion
      // but not yet removed from the grid
      for (Actor *actor : cellActors) {
        if (actor && actor->GetState() != ActorState::Destroy) {
          visibleActors.push_back(actor);
        }
      }
    }
  }

  return visibleActors;
}

int ChunkGrid::GetActiveCellCount() const {
  int count = 0;
  for (const auto &cell : mCells) {
    if (!cell.actors.empty()) {
      count++;
    }
  }
  return count;
}

int ChunkGrid::GetTotalActorCount() const {
  return static_cast<int>(mActorCellMap.size());
}

Vector3 ChunkGrid::GetCellBounds(int cellIndex) const {
  if (cellIndex < 0 || cellIndex >= static_cast<int>(mCells.size())) {
    return Vector3::Zero;
  }

  const Cell &cell = mCells[cellIndex];
  float minX = mWorldMin.x + cell.x * mCellSize;
  float minZ = mWorldMin.z + cell.z * mCellSize;

  return Vector3(minX + mCellSize * 0.5f, 0.0f, minZ + mCellSize * 0.5f);
}

int ChunkGrid::GetCellIndex(const Vector3 &position) const {
  int x, z;
  GetCellCoords(position, x, z);

  // Check bounds
  if (x < 0 || x >= mGridWidth || z < 0 || z >= mGridDepth) {
    return -1; // Outside grid
  }

  return CoordsToIndex(x, z);
}

void ChunkGrid::GetCellCoords(const Vector3 &position, int &outX,
                              int &outZ) const {
  float relX = position.x - mWorldMin.x;
  float relZ = position.z - mWorldMin.z;

  outX = static_cast<int>(std::floor(relX / mCellSize));
  outZ = static_cast<int>(std::floor(relZ / mCellSize));
}

int ChunkGrid::CoordsToIndex(int x, int z) const { return z * mGridWidth + x; }
