#include "Game.hpp"
#include "../include/UI/HUDElement.hpp"
#include "ChunkGrid.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "MIDI/SynthEngine.hpp"
#include "Player.hpp"
#include "actors/Actor.hpp"
#include "actors/TestActors.hpp"
#include "components/ColliderComponent.hpp"
#include "components/DrawComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"
#include "render/TextureAtlas.hpp"

#include "Input.hpp"

#include "scenes/MainMenu.hpp"
#include "scenes/TestScene.hpp"

#include <GL/glew.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>

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

  // Enable VSync
  SDL_GL_SetSwapInterval(1);

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
                             Vector3(1000.0f, 1000.0f, 1000.0f), 100.0f);

  // Setting up SynthEngine
  SynthEngine::init();

  // Creating MIDI thread
  MIDIPlayer::startMIDIThread();

  // Initialize game actors
  LoadScene(new MainMenu(this));

  return true;
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
        SDL_SetWindowFullscreen(mWindow, 0); // windowed
      } else {
        SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
      }
    }
  }
  if (Input::WasKeyPressed(SDL_SCANCODE_M)) {
    LoadScene(new TestSceneA(this));
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
  mUpdatingActors = true;

  // Update player first if it exists
  if (mPlayer) {
    auto it = std::find(mActiveActors.begin(), mActiveActors.end(), mPlayer);
    if (it != mActiveActors.end()) {
      mPlayer->Update(deltaTime);
    }
  }

  // Update all other active actors
  for (auto actor : mActiveActors) {
    if (actor != mPlayer) {
      actor->Update(deltaTime);
    }
  }
  mUpdatingActors = false;

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
  mActiveActors = mChunkGrid->GetVisibleActors(mCamera->GetPosition());

  // Add always-active actors (but avoid duplicates if they're already active
  std::unordered_set<Actor *> actorSet(mActiveActors.begin(),
                                       mActiveActors.end());
  for (auto actor : mAlwaysActiveActors) {
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

  UpdateActors(deltaTime);

  // Update Camera after updating actors, as they can request camera movements
  mCamera->Update(deltaTime);

  // Check collisions after all actors have been updated
  CheckCollisions();

  if (mUIStack.size() > 0 &&
      mUIStack.back()->GetUIState() != UIScreen::UIState::Closing) {
    mUIStack.back()->Update(deltaTime);
  }
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
  std::vector<SpriteComponent *> activeSprites;
  std::vector<SpriteComponent *> worldSprites;
  std::vector<SpriteComponent *> hudSprites;
  bool hasBloom = false;

  for (auto actor : mActiveActors) {
    auto &components = actor->GetComponents();
    for (auto component : components) {
      if (auto mesh = dynamic_cast<MeshComponent *>(component)) {
        if (mesh->IsVisible()) {
          activeMeshes.push_back(mesh);
          if (mesh->IsBloomed()) {
            hasBloom = true;
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
              hasBloom = true;
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

  // Render meshes with instancing (batch draw)
  mRenderer->ActivateMeshShader();
  mRenderer->DrawMeshesInstanced(activeMeshes, mode);

  if (mIsDebugging) {
    for (auto actor : mActiveActors) {
      auto &components = actor->GetComponents();
      for (auto component : components) {
        component->DebugDraw(mRenderer);
      }
    }
  }

  // Render sprites with instancing (batch draw)
  mRenderer->ActivateSpriteShader();

  // Draw world sprites in 3D space (already separated earlier)
  mRenderer->DrawSpritesInstanced(worldSprites, mode);

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
