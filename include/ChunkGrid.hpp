#pragma once
#include "Math.hpp"
#include <vector>
#include <unordered_map>

class Actor;

class ChunkGrid
{
public:
    ChunkGrid(const Vector3& worldMin, const Vector3& worldMax, float cellSize);
    ~ChunkGrid();
    
    // Automatically called by Game - developers don't need to worry about these
    void RegisterActor(Actor* actor);
    void UnregisterActor(Actor* actor);
    void UpdateActor(Actor* actor);  // Call when actor moves
    
    // Get actors in camera cell + adjacent cells (3x3 grid)
    std::vector<Actor*> GetVisibleActors(const Vector3& cameraPos);
    
    // Debug info
    int GetActiveCellCount() const;
    int GetTotalActorCount() const;
    Vector3 GetCellBounds(int cellIndex) const;
    
private:
    struct Cell
    {
        std::vector<Actor*> actors;
        int x, z;  // Cell coordinates
    };
    
    // Calculate which cell a position belongs to
    int GetCellIndex(const Vector3& position) const;
    void GetCellCoords(const Vector3& position, int& outX, int& outZ) const;
    int CoordsToIndex(int x, int z) const;
    
    // World bounds
    Vector3 mWorldMin;
    Vector3 mWorldMax;
    float mCellSize;
    int mGridWidth;   // Number of cells in X
    int mGridDepth;   // Number of cells in Z
    
    // Cells storage
    std::vector<Cell> mCells;
    
    // Track which cell each actor is in for fast updates
    std::unordered_map<Actor*, int> mActorCellMap;
};
