#pragma once
#include "Math.hpp"
#include "Player.hpp"
#include <SDL2/SDL.h>
#include <unordered_set>
#include <vector>

class Scene;

class Game {
public:
  Game();

  bool Initialize();
  void RunLoop();
  void Shutdown();
  void Quit() { mIsRunning = false; }

  // Actor functions
  void LoadScene(Scene *scene);
  void UpdateActors(float deltaTime);
  void FindActiveActors();
  void AddActor(class Actor *actor);
  void RemoveActor(class Actor *actor);

  // Always-active actor functions
  void AddAlwaysActive(class Actor *actor);
  void RemoveAlwaysActive(class Actor *actor);

  // Renderer getter
  class Renderer *GetRenderer() { return mRenderer; }

  // Chunk grid getter (for automatic actor tracking)
  class ChunkGrid *GetChunkGrid() { return mChunkGrid; }

  // Camera functions
  Vector3 &GetCameraPos() { return mCameraPos; }
  void SetCameraPos(Vector3 position);

  // Player getter
  Player *GetPlayer() { return mPlayer; }
  void SetPlayer(Player *player) { mPlayer = player; }

  Vector3 &GetCameraForward() { return mCameraForward; }
  void SetCameraForward(const Vector3 forward) { mCameraForward = forward; }

  Vector3 &GetCameraUp() { return mCameraUp; }
  void SetCameraUp(const Vector3 up) { mCameraUp = up; }

  Quaternion GetCameraRotation() const {
    return Math::LookRotation(mCameraForward, mCameraUp);
  }

  void SetCameraRotation(const Quaternion rotation) {
    mCameraForward = Vector3::Transform(Vector3::UnitZ, rotation);
    mCameraUp = Vector3::Transform(Vector3::UnitY, rotation);
  }

private:
  void ProcessInput();
  void UpdateGame(float deltaTime);
  void GenerateOutput();
  void CheckCollisions();

  // Track if we're updating actors right now
  bool mUpdatingActors;

  // All the actors in the game
  std::vector<class Actor *> mActors;
  std::vector<class Actor *> mPendingActors;

  // Always-active actors (updated/processed even when not visible)
  std::unordered_set<class Actor *> mAlwaysActiveActors;
  std::vector<Actor *> mActiveActors;

  // SDL window
  SDL_Window *mWindow;

  // OpenGL context
  SDL_GLContext mGLContext;

  // Renderer
  class Renderer *mRenderer;

  // Chunk grid for efficient queries
  class ChunkGrid *mChunkGrid;

  Scene *mCurrentScene;
  Scene *mPendingScene;

  // Camera state
  Vector3 mCameraPos;
  Vector3 mCameraForward;
  Vector3 mCameraUp;

  // Player
  Player *mPlayer;

  // Game state
  Uint32 mTicksCount;
  bool mIsRunning;
  bool mIsDebugging;
};
