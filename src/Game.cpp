#include "Game.hpp"
#include "../include/UI/HUDElement.hpp"
#include "AssetLoader.hpp"
#include "ChunkGrid.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "MIDI/SynthEngine.hpp"
#include "actors/Player.hpp"
#include "actors/Actor.hpp"
#include "actors/Ghost.hpp"
#include "actors/RobotA.hpp"
#include "actors/SceneActors.hpp"
#include "components/ColliderComponent.hpp"
#include "components/DrawComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"
#include "render/TextureAtlas.hpp"

#include "Input.hpp"

#include "scenes/MainMenu.hpp"
#include "scenes/OpeningScene.hpp"
#include "scenes/TestScene.hpp"

#include <GL/glew.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include "scenes/Level0.hpp"
#include "scenes/Level1.hpp"
#include "scenes/Level2.hpp"
#include "scenes/Level3.hpp"
#include "UI/Screen/UIScreen.hpp"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FPS = 60;
const int FRAME_TIME = 1000 / FPS;
const float MIDI_UPDATE_INTERVAL = 0.001f; // Update MIDI every 1ms

Game::Game()
    : mUpdatingActors(false), mWindow(nullptr), mGLContext(nullptr),
      mRenderer(nullptr), mChunkGrid(nullptr), mCurrentScene(nullptr),
      mPendingScene(nullptr), mTicksCount(0), mIsRunning(true),
      mIsDebugging(false), mPlayer(nullptr), mCamera(nullptr),
      mBattleSystem(nullptr) {
  mCamera = new Camera(this, Vector3::Zero);
}

bool Game::Initialize() {

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    return false;
  }

  // Disable compositor bypass for better performance when maximized
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "1");

  // Set OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Create window
  mWindow = SDL_CreateWindow(
      "TP Final - Mellodica", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (!mWindow) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return false;
  }

  // Create OpenGL context
  mGLContext = SDL_GL_CreateContext(mWindow);
  if (!mGLContext) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError()
              << std::endl;
    return false;
  }

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewError)
              << std::endl;
    return false;
  }

  // Enable adaptive VSync (allows tearing to prevent lag)
  if (SDL_GL_SetSwapInterval(-1) < 0) {
    // Fallback to regular VSync if adaptive is not supported
    SDL_GL_SetSwapInterval(1);
  }

  // Create renderer
  mRenderer = new Renderer(this);
  if (!mRenderer->Initialize(static_cast<float>(WINDOW_WIDTH),
                             static_cast<float>(WINDOW_HEIGHT))) {
    std::cerr << "Failed to initialize renderer" << std::endl;
    return false;
  }

  mTicksCount = SDL_GetTicks();

  // Create Chunk grid
  mChunkGrid = new ChunkGrid(Vector3(-1000.0f, -1000.0f, -1000.0f),
                             Vector3(1000.0f, 1000.0f, 1000.0f), 48.0f);

  // Setting up SynthEngine
  SynthEngine::init();

  // Creating MIDI thread
  MIDIPlayer::startMIDIThread();

  // Initialize game actors
  // Load saved state and check if we should skip opening
  auto savedState = LoadState();
  if (savedState.find("SCENE_STATE") != savedState.end()) {
    int savedScene = savedState["SCENE_STATE"];
    // If saved level is level1, level2, or level3, skip opening
    if (savedScene >= 1 && savedScene <= 3) {
      LoadScene(new MainMenu(this));
    } else {
      LoadScene(new OpeningScene(this));
    }
  } else {
    LoadScene(new OpeningScene(this));
  }

  return true;
}

std::string Game::GetLevelAssetPath() const {
  if (!mCurrentScene) {
    return getAssetPath("sprites/level0/");
    // return "./assets/sprites/level0/";
  }

  auto sceneID = mCurrentScene->GetSceneID();
  switch (sceneID) {
  case Scene::SceneEnum::scene0:
    return getAssetPath("sprites/level0/");
    // return "./assets/sprites/level0/";
  case Scene::SceneEnum::scene1:
    return getAssetPath("sprites/level1/");
    // return "./assets/sprites/level1/";
  case Scene::SceneEnum::scene2:
    return getAssetPath("sprites/level2/");
    // return "./assets/sprites/level2/";
  case Scene::SceneEnum::scene3:
    return getAssetPath("sprites/level3/");
    // return "./assets/sprites/level3/";
  default:
    return getAssetPath("sprites/level0/");
    // return "./assets/sprites/level0/";
  }
}

void Game::LoadScene(Scene *scene) {
  if (mUpdatingActors) {
    // Defer the scene change if we're currently updating actors
    if (mPendingScene) {
      delete mPendingScene; // Clean up any previous pending scene
    }
    mPendingScene = scene;
  } else {
    // Load immediately if it's safe
    // Cleanup current scene if present
    mPlayer = nullptr;
    if (mCurrentScene) {

      // Clean up UI screens
      for (auto uiScreen : mUIStack) {
        uiScreen->Close();
      }
      mUIStack.clear();

      mCurrentScene->Cleanup();

      UpdateActors(0.0f);
      delete mCurrentScene;

      std::cout << "mActors size after scene cleanup: " << mActors.size()
                << std::endl;
      std::cout << "mPendingActors size after scene cleanup: "
                << mPendingActors.size() << std::endl;
      std::cout << "mAlwaysActiveActors size after scene cleanup: "
                << mAlwaysActiveActors.size() << std::endl;
      std::cout << "Chunkgrid actor count after scene cleanup: "
                << mChunkGrid->GetTotalActorCount() << std::endl;
      std::cout << "mVisibleActors size after scene cleanup: "
                << mActiveActors.size() << std::endl;
    }

    MIDIPlayer::pause();
    MIDIPlayer::clearEventQueue();

    mCurrentScene = scene;
    mCurrentScene->Initialize();

    // Rebuild active actors from new scene
    FindActiveActors();
  }
}

void Game::RunLoop() {
  while (mIsRunning) {
    // Calculate actual elapsed time since last frame
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - mTicksCount) / 1000.0f;
    mTicksCount = currentTicks;

    // Cap deltaTime to prevent huge jumps (max 0.25 seconds)
    if (deltaTime > 0.05f) {
      deltaTime = 0.05f;
    }

    // Update game with deltaTime
    ProcessInput();
    UpdateGame(deltaTime);

    GenerateOutput();

    // Frame rate limiting - delay to maintain 60 FPS
    Uint32 frameTime = SDL_GetTicks() - currentTicks;
    if (frameTime < FRAME_TIME) {
      SDL_Delay(FRAME_TIME - frameTime);
    }
  }
}

void Game::Shutdown() {
  if (!mBattleSystem->IsInBattle() &&
      (mCurrentScene->GetSceneID() == Scene::scene0 ||
       mCurrentScene->GetSceneID() == Scene::scene1 ||
       mCurrentScene->GetSceneID() == Scene::scene2 ||
       mCurrentScene->GetSceneID() == Scene::scene3)) {
    SaveState();
  }

  std::cout << "Shutdown: Starting cleanup..." << std::endl;

  // Stop MIDI thread first
  std::cout << "Shutdown: Stopping MIDI thread..." << std::endl;
  MIDIPlayer::pause();
  MIDIPlayer::stopMIDIThread();
  SynthEngine::clean();

  // Close the window
  if (mWindow) {
    std::cout << "Shutdown: Closing window..." << std::endl;
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    std::cout << "Shutdown: Window closed" << std::endl;
  }

  mAlwaysActiveActors.clear();

  // Delete Chunk grid first
  std::cout << "Shutdown: Deleting Chunk grid..." << std::endl;
  delete mChunkGrid;
  mChunkGrid = nullptr;
  std::cout << "Shutdown: Chunk grid deleted" << std::endl;

  // Delete all actors
  std::cout << "Shutdown: Deleting " << mActors.size() << " actors..."
            << std::endl;
  while (!mActors.empty()) {
    delete mActors.back();
  }
  mActors.clear();
  std::cout << "Shutdown: All actors deleted" << std::endl;

  // Shutdown renderer
  std::cout << "Shutdown: Shutting down renderer..." << std::endl;
  mRenderer->Shutdown();
  delete mRenderer;
  mRenderer = nullptr;
  std::cout << "Shutdown: Renderer deleted" << std::endl;

  if (mGLContext) {
    std::cout << "Shutdown: Deleting GL context..." << std::endl;
    SDL_GL_DeleteContext(mGLContext);
    mGLContext = nullptr;
  }

  // Delete camera
  delete mCamera;

  std::cout << "Shutdown: Quitting SDL..." << std::endl;
  SDL_Quit();
  std::cout << "Shutdown: Complete!" << std::endl;
}

void Game::AddActor(Actor *actor) {
  if (mUpdatingActors) {
    mPendingActors.emplace_back(actor);
  } else {
    mActors.emplace_back(actor);

    // Register with chunk grid at actor's current position
    mChunkGrid->RegisterActor(actor);

    mCurrentScene->RegisterActor(actor);
  }
}

void Game::RemoveActor(Actor *actor) {
  // Unregister from chunk grid
  if (mChunkGrid) {
    mChunkGrid->UnregisterActor(actor);
  }

  // Unregister from scene
  if (mCurrentScene) {
    mCurrentScene->UnregisterActor(actor);
  }

  // Remove from always-active if present
  mAlwaysActiveActors.erase(actor);

  // Check pending actors
  auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
  if (iter != mPendingActors.end()) {
    // Swap to end and pop
    std::iter_swap(iter, mPendingActors.end() - 1);
    mPendingActors.pop_back();
  }

  // Check active actors
  iter = std::find(mActors.begin(), mActors.end(), actor);
  if (iter != mActors.end()) {
    std::iter_swap(iter, mActors.end() - 1);
    mActors.pop_back();
  }
}

void Game::AddAlwaysActive(Actor *actor) { mAlwaysActiveActors.insert(actor); }

void Game::RemoveAlwaysActive(Actor *actor) {
  mAlwaysActiveActors.erase(actor);
}

void Game::ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      mIsRunning = false;
      break;
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        int w, h;
        SDL_GetWindowSize(mWindow, &w, &h);
        float currentAspect = (float)w / h;
        float targetAspect = 16.0f / 9.0f;
        if (currentAspect > targetAspect) {
          w = (int)(h * targetAspect + 0.5f);
        } else {
          h = (int)(w / targetAspect + 0.5f);
        }
        SDL_SetWindowSize(mWindow, w, h);
      } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        // Switch to borderless fullscreen when maximized to avoid compositor
        // lag
        SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
      } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
        // Restore normal windowed mode when un-maximized
        SDL_SetWindowFullscreen(mWindow, 0);
      }
      break;
    case SDL_KEYDOWN:
      if (!mUIStack.empty() &&
          mUIStack.back()->GetUIState() != UIScreen::UIState::Closing) {
        // SDL_Log("Key Pressed and Passed to some UI.");
        mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
      }
    }
  }

  Input::Update();

  if (Input::WasKeyPressed(SDL_SCANCODE_ESCAPE)) {
    mIsRunning = false;
  }

  if (Input::WasKeyPressed(SDL_SCANCODE_F11)) {
    // Toggle fullscreen (use desktop fullscreen for best compatibility)
    if (mWindow) {
      Uint32 flags = SDL_GetWindowFlags(mWindow);
      if (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
        SDL_SetWindowFullscreen(mWindow, 0); // windowed (not maximized)
        SDL_RestoreWindow(mWindow);          // ensure it's not maximized
      } else {
        SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
      }
    }
  }
  if (Input::WasKeyPressed(SDL_SCANCODE_M)) {
    MIDIPlayer::loadSong2a();
    MIDIPlayer::play();
  }
  if (Input::WasKeyPressed(SDL_SCANCODE_F3)) {
    // LoadScene(new TestSceneB(this));
  }

  if (Input::WasKeyPressed(SDL_SCANCODE_F1)) {
    mIsDebugging = !mIsDebugging;

    std::cout << "=== DEBUG MODE " << (mIsDebugging ? "ENABLED" : "DISABLED")
              << " ===" << std::endl;
  }

  for (auto &actor : mActiveActors) {
    actor->ProcessInput();
  }
}

void Game::UpdateActors(float deltaTime) {
  // Note: mUpdatingActors is set in UpdateGame(), not here, to cover collision
  // checks too

  // Update player first if it exists
  if (mPlayer && mPlayer->GetState() != ActorState::Destroy) {
    auto it = std::find(mActiveActors.begin(), mActiveActors.end(), mPlayer);
    if (it != mActiveActors.end()) {
      mPlayer->Update(deltaTime);
    }
  }

  // Update all other active actors
  for (auto actor : mActiveActors) {
    if (actor != mPlayer && actor->GetState() != ActorState::Destroy) {
      actor->Update(deltaTime);
    }
  }

  // Updating UI screens
  for (auto &ui : mUIStack) {
    if (ui->GetUIState() != UIScreen::UIState::Closing) {
      ui->Update(deltaTime);
    }
  }

  // Remove ALL closing UI screens, not just the ones at the back
  auto it = mUIStack.begin();
  while (it != mUIStack.end()) {
    if ((*it)->GetUIState() == UIScreen::UIState::Closing) {
      std::cout << "Deleting UI Screen" << std::endl;
      delete *it;
      it = mUIStack.erase(it);
    } else {
      ++it;
    }
  }

  // Note: mUpdatingActors is set to false in UpdateGame() after collision
  // checks

  // Move pending actors to active and register with chunk grid
  for (auto pending : mPendingActors) {
    mActors.emplace_back(pending);

    // Register with chunk grid now that we're not iterating
    mChunkGrid->RegisterActor(pending);
    mCurrentScene->RegisterActor(pending);
  }
  mPendingActors.clear();

  // Delete dead actors
  std::vector<Actor *> deadActors;
  for (auto actor : mActors) {
    if (actor->GetState() == ActorState::Destroy) {
      deadActors.emplace_back(actor);
    }
  }

  for (auto actor : deadActors) {
    delete actor;
  }

  FindActiveActors();
}

void Game::FindActiveActors() {
  // During battle transitions, use player position instead of camera position
  // to ensure the game world around the player remains visible
  Vector3 queryPosition = mCamera->GetPosition();

  if (mBattleSystem && mBattleSystem->IsTransitioning() && mPlayer) {
    queryPosition = mPlayer->GetPosition();
  }

  std::vector<Actor *> visibleActors =
      mChunkGrid->GetVisibleActors(queryPosition);

  // Filter out destroyed actors from chunk grid results
  mActiveActors.clear();
  for (auto actor : visibleActors) {
    if (actor->GetState() != ActorState::Destroy) {
      mActiveActors.push_back(actor);
    }
  }

  // Add always-active actors (but avoid duplicates if they're already active)
  // Also skip actors marked for destruction
  std::unordered_set<Actor *> actorSet(mActiveActors.begin(),
                                       mActiveActors.end());
  for (auto actor : mAlwaysActiveActors) {
    // Skip destroyed actors
    if (actor->GetState() == ActorState::Destroy) {
      continue;
    }
    if (actorSet.insert(actor).second) {
      mActiveActors.push_back(actor);
    }
  }
}

void Game::UpdateGame(float deltaTime) {
  // Handle pending scene change at the start of update (safe point)
  if (mPendingScene) {

    // Clean up UI screens
    for (auto uiScreen : mUIStack) {
      uiScreen->Close();
    }
    mUIStack.clear();

    // Cleanup current scene if present
    if (mCurrentScene) {

      mPlayer = nullptr;
      mCurrentScene->Cleanup();
      delete mCurrentScene;
    }

    MIDIPlayer::pause();
    MIDIPlayer::clearEventQueue();

    mCurrentScene = mPendingScene;
    mPendingScene = nullptr;

    mCurrentScene->Initialize();

    // Rebuild active actors from new scene
    FindActiveActors();
  }

  mUpdatingActors = true;

  UpdateActors(deltaTime);

  // Update Camera after updating actors, as they can request camera movements
  mCamera->Update(deltaTime);

  // Check collisions after all actors have been updated
  CheckCollisions();

  mUpdatingActors = false;
}

void Game::CheckCollisions() {
  // Get visible actors from chunk grid

  // Collect all colliders from active actors
  std::vector<ColliderComponent *> colliders;
  for (auto actor : mActiveActors) {
    auto collider = actor->GetComponent<ColliderComponent>();
    if (collider) {
      colliders.push_back(collider);
    }
  }

  // Check collisions between all pairs
  for (size_t i = 0; i < colliders.size(); i++) {
    for (size_t j = i + 1; j < colliders.size(); j++) {
      ColliderComponent *colliderA = colliders[i];
      ColliderComponent *colliderB = colliders[j];

      // Check if they intersect
      if (colliderA->Intersect(*colliderB)) {
        // Get penetration vector for A (how much to push A out of B)
        Vector3 penetrationA = colliderA->DetectCollision(*colliderB);

        // Determine who gets pushed based on static flags
        bool aIsStatic = colliderA->IsStatic();
        bool bIsStatic = colliderB->IsStatic();

        if (!aIsStatic && !bIsStatic) {
          // Both dynamic - split the penetration
          colliderA->GetOwner()->OnCollision(penetrationA * 0.5f, colliderB);
          colliderB->GetOwner()->OnCollision(penetrationA * -0.5f, colliderA);
        } else if (!aIsStatic && bIsStatic) {
          // A is dynamic, B is static - only push A
          colliderA->GetOwner()->OnCollision(penetrationA, colliderB);
        } else if (aIsStatic && !bIsStatic) {
          // A is static, B is dynamic - only push B
          colliderB->GetOwner()->OnCollision(penetrationA * -1.0f, colliderA);
        }
        // else: both static - no response needed
      }
    }
  }
}

void Game::GenerateOutput() {
  static Uint32 lastTime = SDL_GetTicks();
  static int frameCount = 0;
  Uint32 startFrame = SDL_GetTicks();

  RendererMode mode =
      mIsDebugging ? RendererMode::LINES : RendererMode::TRIANGLES;

  // Get visible actors from chunk grid
  // std::vector<Actor*> mActiveActors =
  // mChunkGrid->GetVisibleActors(mCameraPos);

  // Collect meshes and sprites from active actors
  std::vector<MeshComponent *> activeMeshes;
  std::vector<MeshComponent *> bloomedMeshes;
  std::vector<MeshComponent *> nonBloomedMeshes;
  std::vector<SpriteComponent *> activeSprites;
  std::vector<SpriteComponent *> worldSprites;
  std::vector<SpriteComponent *> bloomedSprites;
  std::vector<SpriteComponent *> nonBloomedSprites;
  std::vector<SpriteComponent *> hudSprites;
  bool hasBloom = false;

  for (auto actor : mActiveActors) {
    auto &components = actor->GetComponents();
    for (auto component : components) {
      if (auto mesh = dynamic_cast<MeshComponent *>(component)) {
        if (mesh->IsVisible()) {
          activeMeshes.push_back(mesh);
          if (mesh->IsBloomed()) {
            bloomedMeshes.push_back(mesh);
            hasBloom = true;
          } else {
            nonBloomedMeshes.push_back(mesh);
          }
        }
      } else if (auto sprite = dynamic_cast<SpriteComponent *>(component)) {
        if (sprite->IsVisible()) {
          activeSprites.push_back(sprite);

          // Separate world from HUD sprites
          if (sprite->IsHUD()) {
            hudSprites.push_back(sprite);
          } else {
            worldSprites.push_back(sprite);
            if (sprite->IsBloomed()) {
              bloomedSprites.push_back(sprite);
              hasBloom = true;
            } else {
              nonBloomedSprites.push_back(sprite);
            }
          }
        }
      }
    }
  }

  // BLOOM PASS: Render ALL objects to bloom framebuffer
  // Bloomed objects render normally, non-bloomed objects render as black for
  // occlusion
  mRenderer->BeginBloomPass();

  // Temporarily mark non-bloomed objects with negative color to render them
  // black
  std::vector<Vector3> originalMeshColors;
  std::vector<Vector3> originalSpriteColors;

  // Save original colors and mark non-bloomed meshes
  for (auto *mesh : activeMeshes) {
    originalMeshColors.push_back(mesh->GetColor());
    if (!mesh->IsBloomed()) {
      mesh->SetColor(
          Vector3(-1.0f, -1.0f, -1.0f)); // Negative = black in bloom pass
    }
  }

  // Save sprite colors and mark non-bloomed sprites
  for (auto *sprite : worldSprites) {
    originalSpriteColors.push_back(sprite->GetColor());
    if (!sprite->IsBloomed()) {
      sprite->SetColor(
          Vector3(-1.0f, -1.0f, -1.0f)); // Negative = black in bloom pass
    }
  }

  // Render ALL meshes (bloomed and non-bloomed for occlusion)
  if (!activeMeshes.empty()) {
    mRenderer->ActivateMeshShaderForBloom();
    mRenderer->DrawMeshesInstanced(activeMeshes, mode);
  }

  // Render ALL world sprites (bloomed and non-bloomed for occlusion)
  if (!worldSprites.empty()) {
    mRenderer->ActivateSpriteShaderForBloom();
    mRenderer->DrawSpritesInstanced(worldSprites, mode);
  }

  // Restore original colors
  for (size_t i = 0; i < activeMeshes.size(); ++i) {
    activeMeshes[i]->SetColor(originalMeshColors[i]);
  }

  for (size_t i = 0; i < worldSprites.size(); ++i) {
    worldSprites[i]->SetColor(originalSpriteColors[i]);
  }

  mRenderer->EndBloomPass();

  // Apply Gaussian blur to bloom texture
  mRenderer->ApplyBloomBlur();

  // Begin rendering to main framebuffer
  mRenderer->BeginFramebuffer();

  // Render non-bloomed meshes with lighting
  if (!nonBloomedMeshes.empty()) {
    mRenderer->ActivateMeshShader();
    mRenderer->DrawMeshesInstanced(nonBloomedMeshes, mode);
  }

  // Render bloomed meshes without lighting
  if (!bloomedMeshes.empty()) {
    mRenderer->ActivateMeshShaderNoLighting();
    mRenderer->DrawMeshesInstanced(bloomedMeshes, mode);
  }

  if (mIsDebugging) {
    for (auto actor : mActiveActors) {
      auto &components = actor->GetComponents();
      for (auto component : components) {
        component->DebugDraw(mRenderer);
      }
    }
  }

  // Render non-bloomed sprites with lighting
  if (!nonBloomedSprites.empty()) {
    mRenderer->ActivateSpriteShader();
    mRenderer->DrawSpritesInstanced(nonBloomedSprites, mode);
  }

  // Render bloomed sprites without lighting
  if (!bloomedSprites.empty()) {
    mRenderer->ActivateSpriteShaderNoLighting();
    mRenderer->DrawSpritesInstanced(bloomedSprites, mode);
  }

  // End framebuffer rendering and display to screen
  mRenderer->EndFramebuffer();

  // Draw HUD sprites in screen space (after framebuffer)
  mRenderer->DrawHUDSprites(hudSprites);

  // Only swap if the window has a valid drawable size (not minimized)
  if (mWindow) {
    int drawableW, drawableH;
    SDL_GL_GetDrawableSize(mWindow, &drawableW, &drawableH);
    if (drawableW > 0 && drawableH > 0) {
      SDL_GL_SwapWindow(mWindow);
    }
  }
}

void Game::SaveState() {
  // Don't save if there's no player or current scene
  if (!mPlayer || !mCurrentScene) {
    SDL_Log("Cannot save: no player or scene exists");
    return;
  }

  std::ofstream file("game_save.ckpt");

  if (!file.is_open()) {
    SDL_Log("Failed to save game_save.ckpt");
    return;
  }

  SDL_Log("Saving Player State");
  file << "PLAYER_STATE\n";
  auto playerPos = mPlayer->GetPosition();
  file << (int)playerPos.x << "\n"
       << (int)playerPos.y << "\n"
       << (int)playerPos.z << "\n";
  // Clamp health to at least 1 when saving
  int healthToSave = mPlayer->getHealth();
  if (healthToSave <= 0) {
    healthToSave = 1;
  }
  file << healthToSave << "\n";
  file << (int)mPlayer->getEnergy() << "\n";
  file << "PLAYER_ALLIES\n";
  SDL_Log("Saving Allies State");
  auto alliesVector = mPlayer->GetActiveAllies();
  SDL_Log("Number of allies to save: %zu", alliesVector.size());
  file << alliesVector.size() << "\n";
  for (size_t i = 0; i < alliesVector.size(); i++) {
    // Ally ID
    file << i << "\n";
    // Ally State
    file << alliesVector[i]->GetMaxHealth() << "\n";
    // Clamp ally health to at least 1 when saving
    int allyHealth = alliesVector[i]->GetHealth();
    if (allyHealth <= 0) {
      allyHealth = 1;
    }
    file << allyHealth << "\n";
    file << alliesVector[i]->GetChannel() << "\n";
    auto combType = alliesVector[i]->GetCombatantType();
    switch (combType) {
    case CombatantType::Phantasm:
      file << "Phantasm\n";
      break;
    case CombatantType::Robot:
      file << "Robot\n";
      break;
    }
  }
  SDL_Log("Saving scene state");
  file << "SCENE_STATE\n";

  auto scenestate = mCurrentScene->GetSceneID();

  SDL_Log("Saving scene state %d", scenestate);

  switch (scenestate) {
  case Scene::SceneEnum::scene0:
    file << "0\n";
    break;
  case Scene::SceneEnum::scene1:
    file << "1\n";
    break;
  case Scene::SceneEnum::scene2:
    file << "2\n";
    break;
  case Scene::SceneEnum::scene3:
    file << "3\n";
    break;
  case Scene::SceneEnum::scene4:
    file << "4\n";
    break;
  case Scene::SceneEnum::scene5:
    file << "5\n";
    break;
  case Scene::SceneEnum::scene6:
    file << "6\n";
    break;
  case Scene::SceneEnum::scene7:
    file << "7\n";
    break;
  }

  file.close();
  SDL_Log("End saving process");
}

std::map<std::string, int> Game::LoadState() {
  std::ifstream file("game_save.ckpt");

  if (!file.is_open()) {
    SDL_Log("Failed to load game_save.ckpt");
    return std::map<std::string, int>();
  }

  auto my_map = std::map<std::string, int>();

  std::string line;

  while (std::getline(file, line)) {
    if (line == "PLAYER_STATE") {
      int posX, posY, posZ;
      SDL_Log("Loading Player State");

      std::getline(file, line);
      posX = std::atoi(line.c_str());
      my_map["PLAYER_X"] = posX;

      std::getline(file, line);
      posY = std::atoi(line.c_str());
      my_map["PLAYER_Y"] = posY;

      std::getline(file, line);
      posZ = std::atoi(line.c_str());
      my_map["PLAYER_Z"] = posZ;

      // Load health and energy
      std::getline(file, line);
      int health = std::atoi(line.c_str());
      my_map["PLAYER_HEALTH"] = health;

      std::getline(file, line);
      int energy = std::atoi(line.c_str());
      my_map["PLAYER_ENERGY"] = energy;
    }
    if (line == "PLAYER_ALLIES") {
      SDL_Log("Loading Allies State");
      int numOfAllies;
      std::getline(file, line);
      numOfAllies = std::atoi(line.c_str());
      if (numOfAllies > 0) {
        my_map["NUM_OF_ALLIES"] = numOfAllies;
      }
      for (int i = 0; i < numOfAllies; i++) {
        // ID
        std::getline(file, line);
        auto key = "ALLY_" + line;

        // maxhealth
        std::getline(file, line);
        int maxHealth = std::atoi(line.c_str());
        my_map[key + "_MAXHEALTH"] = maxHealth;

        // health
        std::getline(file, line);
        int health = std::atoi(line.c_str());
        my_map[key + "_HEALTH"] = health;

        // channel
        std::getline(file, line);
        int channel = std::atoi(line.c_str());
        my_map[key + "_CHANNEL"] = channel;

        // combatant type
        std::getline(file, line);
        if (line == "Phantasm") {
          my_map[key + "_COMBTYPE"] = 0;
        } else if (line == "Robot") {
          my_map[key + "_COMBTYPE"] = 1;
        } else {
          my_map[key + "_COMBTYPE"] = 2;
        }
      }
    }
    if (line == "SCENE_STATE") {
      SDL_Log("Loading Scene State");
      std::getline(file, line);
      int scenestate = std::atoi(line.c_str());
      my_map["SCENE_STATE"] = scenestate;
    }
  }

  return my_map;
}

void Game::ResetSaveToLevel0() {
  std::ofstream file("game_save.ckpt");

  if (!file.is_open()) {
    SDL_Log("Failed to reset game_save.ckpt");
    return;
  }

  SDL_Log("Resetting save to Level0");
  // Write minimal save with level 0
  file << "SCENE_STATE\n";
  file << "0\n";

  file.close();
  SDL_Log("Save reset to Level0 completed");
}

void Game::RestorePlayerAllies() {
  if (!mPlayer) {
    SDL_Log("Cannot restore allies: no player exists");
    return;
  }

  auto savedState = LoadState();

  // Restore player health and energy
  if (savedState.find("PLAYER_HEALTH") != savedState.end()) {
    int health = savedState["PLAYER_HEALTH"];
    mPlayer->setHealth(health);
    SDL_Log("Restored player health: %d", health);
  }

  if (savedState.find("PLAYER_ENERGY") != savedState.end()) {
    int energy = savedState["PLAYER_ENERGY"];
    mPlayer->setEnergy(static_cast<float>(energy));
    SDL_Log("Restored player energy: %d", energy);
  }

  if (savedState.find("NUM_OF_ALLIES") == savedState.end()) {
    SDL_Log("No allies to restore");
    return;
  }

  int numAllies = savedState["NUM_OF_ALLIES"];
  SDL_Log("Restoring %d allies", numAllies);

  for (int i = 0; i < numAllies; i++) {
    std::string allyKey = "ALLY_" + std::to_string(i);

    if (savedState.find(allyKey + "_MAXHEALTH") == savedState.end()) {
      SDL_Log("Missing ally %d data, skipping", i);
      continue;
    }

    int maxHealth = savedState[allyKey + "_MAXHEALTH"];
    int health = savedState[allyKey + "_HEALTH"];
    int channel = savedState[allyKey + "_CHANNEL"];
    int combType = savedState[allyKey + "_COMBTYPE"];

    SDL_Log("Restoring ally %d: health=%d/%d, channel=%d, type=%d", i, health,
            maxHealth, channel, combType);

    // Create the appropriate combatant type
    Combatant *ally = nullptr;
    if (combType == 0) { // Phantasm
      ally = new Ghost(this, channel, maxHealth);
    } else if (combType == 1) { // Robot
      ally = new RobotA(this, channel, maxHealth);
    }

    if (ally) {
      // Set the restored health
      ally->SetHealth(health);

      // Position ally behind player (simple offset)
      Vector3 playerPos = mPlayer->GetPosition();
      ally->SetPosition(playerPos);

      // Add to player's allies
      mPlayer->GetActiveAllies().push_back(ally);

      SDL_Log("Ally %d restored successfully", i);
    }
  }

  SDL_Log("Finished restoring allies");
}
