#pragma once
#include "BattleSystem.hpp"
#include "Math.hpp"
#include "Player.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <unordered_set>
#include <vector>

#include "Camera.hpp"

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

  // Camera
  Camera *GetCamera() const { return mCamera; }

  // Debugging getter
  bool IsDebugging() const { return mIsDebugging; }

  // Ticks count getter
  Uint32 GetTicksCount() const { return mTicksCount; }

  // Player getter
  Player *GetPlayer() { return mPlayer; }
  void SetPlayer(Player *player) { mPlayer = player; }

  // Scene getter
  Scene *GetCurrentScene() { return mCurrentScene; }

  // Get level-specific asset path (e.g., "./assets/sprites/level0/")
  std::string GetLevelAssetPath() const;

  // Battle system getter
  BattleSystem *GetBattleSystem() { return mBattleSystem; }
  void SetBattleSystem(BattleSystem *battleSystem) {
    mBattleSystem = battleSystem;
  }

  // UI Functions
  void PushUI(class UIScreen *screen) { mUIStack.emplace_back(screen); }
  const std::vector<class UIScreen *> &GetUIStack() { return mUIStack; }

  // Game Save function
  void SaveState();
  std::map<std::string, int> LoadState();
  void ResetSaveToLevel0();
  void RestorePlayerAllies();

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

  // Game Camera
  class Camera *mCamera;

  Scene *mCurrentScene;
  Scene *mPendingScene;

  // Player
  Player *mPlayer;

  // Battle system
  BattleSystem *mBattleSystem;

  // All UI screens in the game
  std::vector<class UIScreen *> mUIStack;

  // Game state
  Uint32 mTicksCount;
  bool mIsRunning;
  bool mIsDebugging;
};
