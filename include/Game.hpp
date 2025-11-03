#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_set>
#include "Math.hpp"

class Game
{
public:
    Game();
    
    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }
    
    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    
    // Always-active actor functions
    void AddAlwaysActive(class Actor* actor);
    void RemoveAlwaysActive(class Actor* actor);
    
    // Drawable functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);
    std::vector<class DrawComponent*>& GetDrawables() { return mDrawables; }
    
    // Renderer getter
    class Renderer* GetRenderer() { return mRenderer; }
    
    // Chunk grid getter (for automatic actor tracking)
    class ChunkGrid* GetChunkGrid() { return mChunkGrid; }
    
    // Camera functions
    Vector3& GetCameraPos() { return mCameraPos; }
    void SetCameraPos(Vector3& position);

    Vector3& GetCameraForward() { return mCameraForward; }
    void SetCameraForward(const Vector3& forward) { mCameraForward = forward; }
    
    Vector3& GetCameraUp() { return mCameraUp; }
    void SetCameraUp(const Vector3& up) { mCameraUp = up; }
    
    float GetCameraYaw() const { return mCameraYaw; }
    void SetCameraYaw(float yaw) { mCameraYaw = yaw; }
    
    float GetCameraPitch() const { return mCameraPitch; }
    void SetCameraPitch(float pitch) { mCameraPitch = pitch; }

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void GenerateOutput();
    
    // Track if we're updating actors right now
    bool mUpdatingActors;
    
    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;
    
    // Always-active actors (updated/processed even when not visible)
    std::unordered_set<class Actor*> mAlwaysActiveActors;
    std::vector<class Actor*> mVisibleActors;
    
    // All draw components
    std::vector<class DrawComponent*> mDrawables;
    
    // SDL window
    SDL_Window* mWindow;
    
    // OpenGL context
    SDL_GLContext mGLContext;
    
    // Renderer
    class Renderer* mRenderer;
    
    // Chunk grid for efficient queries
    class ChunkGrid* mChunkGrid;
    int mCurrentChunk;
    
    // Camera state
    Vector3 mCameraPos;
    Vector3 mCameraForward;
    Vector3 mCameraUp;
    float mCameraYaw;
    float mCameraPitch;
    
    // Game state
    Uint32 mTicksCount;
    bool mIsRunning;
    bool mIsDebugging;
};
