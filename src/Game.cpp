#include "Game.hpp"
#include "actors/Actor.hpp"
#include "components/DrawComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "render/Renderer.hpp"
#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"
#include "actors/TestActors.hpp"
#include "ChunkGrid.hpp"
#include "SynthEngine.hpp"
#include "MIDIPlayer.hpp"

#include <GL/glew.h>
#include <iostream>
#include <algorithm>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FPS = 60;
const int FRAME_TIME = 1000 / FPS;
const float MIDI_UPDATE_INTERVAL = 0.001f;  // Update MIDI every 1ms

Game::Game()
: mUpdatingActors(false)
, mWindow(nullptr)
, mGLContext(nullptr)
, mRenderer(nullptr)
, mChunkGrid(nullptr)
, mCurrentChunk(-1)
, mCameraPos(Vector3::Zero) 
, mCameraForward(0.0f, 0.0f, -1.0f)  // Looking toward negative Z
, mCameraUp(0.0f, 1.0f, 0.0f)
, mCameraYaw(0.0f)
, mCameraPitch(-45.0f)  // Looking down more to see the grid
, mTicksCount(0)
, mIsRunning(true)
, mIsDebugging(false)
{
    // Update forward vector based on pitch
    float pitchRad = Math::ToRadians(mCameraPitch);
    mCameraForward.y = Math::Sin(pitchRad);
    float horizontalLength = Math::Cos(pitchRad);
    mCameraForward.x = 0.0f;
    mCameraForward.z = -horizontalLength;  // Negative Z is forward
    mCameraForward.Normalize();
}

bool Game::Initialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
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
        "TP1 - Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!mWindow)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create OpenGL context
    mGLContext = SDL_GL_CreateContext(mWindow);
    if (!mGLContext)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewError) << std::endl;
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // Create renderer
    mRenderer = new Renderer();
    if (!mRenderer->Initialize(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)))
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    
    mTicksCount = SDL_GetTicks();
    
    // Create Chunk grid
    mChunkGrid = new ChunkGrid(Vector3(-100.0f, -10.0f, -1000.0f), Vector3(100.0f, 10.0f, 1000.0f), 10.0f);
    
    // Initialize game actors
    InitializeActors();
    
    return true;
}

void Game::InitializeActors()
{
    SynthEngine::init("assets/songs/sf.sf2");
    for (auto preset : SynthEngine::getSoundPresets()) {
        std::cout << preset.first << ": "<<preset.second.bank_num<<"/"<<preset.second.num<<'\n';
    }
    SynthEngine::setChannels({
        {0,0},
        {0,2},
        {0,32},
        {0,24},
        {0,25},
        {0,40},
        {0,48},
        {0,56},
        {0,66},
        {128,0},
        {0,89},
        {0,73},
        {0,57},
        {0,42},
        {0,11},
        {0,52}
    });
    MIDIPlayer::loadSong("assets/songs/5.mid",true);
    MIDIPlayer::play();

    
    std::cout << "Starting actor creation..." << std::endl;
    
    // Create a grid of cube actors (alternating grass, rock, and red cubes)
    const int gridSize = 300;
    const float spacing = 1.0f;
    const float gridOffset = (gridSize - 1) * spacing * 0.5f;
    
    int actorCount = 0;
    for (int x = 0; x < gridSize; x++)
    {
        if (x % 50 == 0) {
            std::cout << "Creating actors row " << x << "/" << gridSize << "..." << std::endl;
        }
        for (int z = 0; z < gridSize; z++)
        {
            // Alternate between grass, rock, and red cubes
            Actor* mesh = nullptr;
            Actor* sprite = nullptr;
            int pattern = (x + z) % 3;
            if (pattern == 0) {
                mesh = new GrassCubeActor(this);
                sprite = new MarioActor(this);
                sprite->SetPosition(Vector3(x * spacing - gridOffset,1.0f,z * spacing - gridOffset));
            } else if (pattern == 1) {
                mesh = new RockCubeActor(this);
                sprite = new GoombaActor(this);
                sprite->SetPosition(Vector3(x * spacing - gridOffset,1.0f,z * spacing - gridOffset));
            } else {
                
                mesh = new PyramidActor(this, Color::Cyan,{3});
            }
            
            mesh->SetPosition(Vector3( x * spacing - gridOffset,0.0f,z * spacing - gridOffset));
            actorCount += 2;
        }
    }
    
    std::cout << "Initialized " << actorCount / 2 << " cube actors and " << actorCount / 2 << " sprite actors" << std::endl;
    std::cout << "Total actors in grid: " << (mChunkGrid ? mChunkGrid->GetTotalActorCount() : 0) << std::endl;


    // Create camera controller
    new CameraController(this);
    //SetCameraPos(Vector3(0.0f,5.0,0.0));
}

void Game::SetCameraPos(Vector3 position){
    mCameraPos = position;
    
    int newChunk = mChunkGrid->GetCellIndex(mCameraPos);
    if(newChunk != mCurrentChunk){
        mCurrentChunk = newChunk;
        mVisibleActors = mChunkGrid->GetVisibleActors(mCameraPos);


        mVisibleMeshes.clear();
        mVisibleSprites.clear();
        
    
        // First, collect from always-active actors
        for (auto actor : mAlwaysActiveActors)
        {
            auto& components = actor->GetComponents();
            for (auto component : components)
            {
                if (auto mesh = dynamic_cast<MeshComponent*>(component))
                {
                    if (mesh->IsVisible())
                    {
                        mVisibleMeshes.push_back(mesh);
                    }
                }
                else if (auto sprite = dynamic_cast<SpriteComponent*>(component))
                {
                    if (sprite->IsVisible())
                    {
                        mVisibleSprites.push_back(sprite);
                    }
                }
            }
        }
    
    // Then, collect from Chunkly visible actors
    for (auto actor : mVisibleActors)
    {
        // Get all drawable components from this actor
        auto& components = actor->GetComponents();
        for (auto component : components)
        {
            // Quick type check without virtual call
            if (auto mesh = dynamic_cast<MeshComponent*>(component))
            {
                if (mesh->IsVisible())
                {
                    mVisibleMeshes.push_back(mesh);
                }
            }
            else if (auto sprite = dynamic_cast<SpriteComponent*>(component))
            {
                if (sprite->IsVisible())
                {
                    mVisibleSprites.push_back(sprite);
                }
            }
        }
    }



    }


    



}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate actual elapsed time since last frame
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - mTicksCount) / 1000.0f;
        mTicksCount = currentTicks;
        
        // Cap deltaTime to prevent huge jumps (max 0.25 seconds)
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }
        
        // Update game with actual deltaTime
        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();
        
        // Frame rate limiting - delay to maintain 60 FPS
        Uint32 frameTime = SDL_GetTicks() - currentTicks;
        if (frameTime < FRAME_TIME)
        {
            SDL_Delay(FRAME_TIME - frameTime);
        }
    }
}

void Game::Shutdown()
{
    std::cout << "Shutdown: Starting cleanup..." << std::endl;
    
    // Close the window immediately so user sees it close
    if (mWindow)
    {
        std::cout << "Shutdown: Closing window..." << std::endl;
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
        std::cout << "Shutdown: Window closed" << std::endl;
    }
    
    // Now do the cleanup in the background
    
    // Clear always-active set first (actors will try to unregister during deletion)
    mAlwaysActiveActors.clear();
    
    // Delete Chunk grid first
    if (mChunkGrid)
    {
        std::cout << "Shutdown: Deleting Chunk grid..." << std::endl;
        delete mChunkGrid;
        mChunkGrid = nullptr;
        std::cout << "Shutdown: Chunk grid deleted" << std::endl;
    }
    
    std::cout << "Shutdown: Deleting " << mActors.size() << " actors..." << std::endl;
    Uint32 startTime = SDL_GetTicks();
    
    // Fast mass deletion - delete in batches
    size_t total = mActors.size();
    size_t deleted = 0;
    
    for (size_t i = 0; i < total; ++i)
    {
        delete mActors[i];
        deleted++;
        
        if (deleted % 20000 == 0) {
            Uint32 elapsed = SDL_GetTicks() - startTime;
            std::cout << "Deleted " << deleted << "/" << total << " actors in " << elapsed << "ms..." << std::endl;
        }
    }
    
    Uint32 totalTime = SDL_GetTicks() - startTime;
    mActors.clear();
    std::cout << "Shutdown: All actors deleted in " << totalTime << "ms" << std::endl;
    
    if (mRenderer)
    {
        std::cout << "Shutdown: Shutting down renderer..." << std::endl;
        mRenderer->Shutdown();
        delete mRenderer;
        mRenderer = nullptr;
        std::cout << "Shutdown: Renderer deleted" << std::endl;
    }
    
    if (mGLContext)
    {
        std::cout << "Shutdown: Deleting GL context..." << std::endl;
        SDL_GL_DeleteContext(mGLContext);
        mGLContext = nullptr;
    }
    
    std::cout << "Shutdown: Quitting SDL..." << std::endl;
    SDL_Quit();
    std::cout << "Shutdown: Complete!" << std::endl;
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
        // Chunk grid registration happens automatically in Actor constructor
    }
}

void Game::RemoveActor(Actor* actor)
{
    // During mass deletion (shutdown with mChunkGrid == nullptr), 
    // skip all operations for performance
    if (!mChunkGrid && mActors.size() > 10000)
    {
        // Fast path during shutdown
        return;
    }
    
    // Normal removal: unregister from Chunk grid
    if (mChunkGrid)
    {
        mChunkGrid->UnregisterActor(actor);
    }
    
    // Remove from always-active if present
    mAlwaysActiveActors.erase(actor);
    
    // Check pending actors
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end and pop
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }
    
    // Check active actors
    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddAlwaysActive(Actor* actor)
{
    mAlwaysActiveActors.insert(actor);
}

void Game::RemoveAlwaysActive(Actor* actor)
{
    mAlwaysActiveActors.erase(actor);
}


void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                mIsRunning = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    // Handle window resize
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    
                    float orthoSize = 5.0f;
                    float aspectRatio = static_cast<float>(event.window.data1) / static_cast<float>(event.window.data2);
                    Matrix4 orthoProjection = Matrix4::CreateOrtho(
                        -orthoSize * aspectRatio, orthoSize * aspectRatio,
                        -orthoSize, orthoSize, 0.1f, 1000.0f
                    );
                    mRenderer->SetProjectionMatrix(orthoProjection);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    mIsRunning = false;
                }
                break;
        }
    }
    
    // Get keyboard state for continuous input
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Always process always-active actors first (like camera controller)
    for (auto actor : mAlwaysActiveActors)
    {
        actor->ProcessInput(keyState);
    }
    
    // Only process input for visible actors (skip if already processed as always-active)
    for (auto actor : mVisibleActors)
    {
        // Skip if this actor is always-active (already processed above)
        if (mAlwaysActiveActors.find(actor) != mAlwaysActiveActors.end())
        {
            continue;
        }
        actor->ProcessInput(keyState);
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    
    // Always update always-active actors first (like camera controller)
    for (auto actor : mAlwaysActiveActors)
    {
        actor->Update(deltaTime);
    }
    
    // Only update visible actors (skip if already updated as always-active)
    for (auto actor : mVisibleActors)
    {
        // Skip if this actor is always-active (already updated above)
        if (mAlwaysActiveActors.find(actor) != mAlwaysActiveActors.end())
        {
            continue;
        }
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;
    
    // Move pending actors to active
    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
        // Chunk grid registration happens automatically when actor is created
    }
    mPendingActors.clear();
    
    // Delete dead actors
    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }
    
    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::UpdateGame(float deltaTime)
{
    // Update MIDI player
    MIDIPlayer::update(deltaTime);

    // Update all actors
    UpdateActors(deltaTime);
}

void Game::GenerateOutput()
{
    static Uint32 lastTime = SDL_GetTicks();
    static int frameCount = 0;
    Uint32 startFrame = SDL_GetTicks();
    
    mRenderer->Clear();
    
    // Update camera
    Vector3 targetPos = mCameraPos + mCameraForward;
    mRenderer->SetViewMatrix(Matrix4::CreateLookAt(mCameraPos, targetPos, mCameraUp));

    /*
    Uint32 afterCameraSetup = SDL_GetTicks();
    
    
    
    if (mChunkGrid)
    {
        
        frameCount++;
        if (frameCount % 60 == 0) {  // Debug every 60 frames
            Uint32 currentTime = SDL_GetTicks();
            float fps = 60000.0f / (currentTime - lastTime);
            lastTime = currentTime;
            
            std::cout << "=== Performance Stats ===" << std::endl;
            std::cout << "FPS: " << fps << std::endl;
            std::cout << "Camera pos: (" << mCameraPos.x << ", " << mCameraPos.y << ", " << mCameraPos.z << ")" << std::endl;
            std::cout << "Total actors in grid: " << mChunkGrid->GetTotalActorCount() << std::endl;
            std::cout << "Visible actors: " << mVisibleActors.size() << std::endl;
        }

    }
    
    Uint32 afterChunkQuery = SDL_GetTicks();
    
    
    Uint32 afterCollect = SDL_GetTicks();
    

    if (frameCount % 60 == 0 && mChunkGrid) {
        std::cout << "Visible meshes: " << mVisibleMeshes.size() << std::endl;
        std::cout << "Visible sprites: " << mVisibleSprites.size() << std::endl;
        std::cout << "Time - Camera: " << (afterCameraSetup - startFrame) 
                  << "ms, Chunk: " << (afterChunkQuery - afterCameraSetup)
                  << "ms, Collect: " << (afterCollect - afterChunkQuery) << "ms" << std::endl;
    }

    */


    RendererMode mode = mIsDebugging ? RendererMode::LINES : RendererMode::TRIANGLES;
   

    // Render meshes with instancing (batch draw)
    mRenderer->ActivateMeshShader();
    mRenderer->DrawMeshesInstanced(mVisibleMeshes, mode);
    
    // Render sprites with instancing (batch draw)
    mRenderer->ActivateSpriteShader();
    mRenderer->DrawSpritesInstanced(mVisibleSprites, mode);
    
    SDL_GL_SwapWindow(mWindow);
}
