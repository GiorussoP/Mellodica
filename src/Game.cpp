#include "Game.hpp"
#include "actors/Actor.hpp"
#include "components/DrawComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/ColliderComponent.hpp"
#include "render/Renderer.hpp"
#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"
#include "actors/TestActors.hpp"
#include "ChunkGrid.hpp"
#include "MIDI/SynthEngine.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "Player.hpp"
#include "HUDElement.hpp"

#include <GL/glew.h>
#include <iostream>
#include <algorithm>
#include <unordered_set>

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
, mCameraPos(Vector3::Zero) 
, mCameraForward(0.0f, 0.0f, -1.0f)  // Looking toward negative Z
, mCameraUp(0.0f, 1.0f, 0.0f)
, mTicksCount(0)
, mIsRunning(true)
, mIsDebugging(false)
{
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
        "TP Final - Mellodica",
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
    // Initializing the SynthEngine
    SynthEngine::init();
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

    // Initializing MIDI Player
    MIDIPlayer::loadSong("assets/songs/7.mid",true);
    MIDIPlayer::startMIDIThread();
    MIDIPlayer::play();
    

    
    // Some actors for testing
    const int gridSize = 300;
    const float spacing = 1.0f;
    const float gridOffset = (gridSize - 1) * spacing * 0.5f;
    int actorCount = 0;
    for (int x = 0; x < gridSize; x++) {   
        for (int z = 0; z < gridSize; z++) {
            auto mesh = new GrassCubeActor(this);
            mesh->SetPosition(Vector3( x * spacing - gridOffset,0.0f,z * spacing - gridOffset));
            actorCount += 2;
        }
    }



    auto pyramid = new PyramidActor(this, Color::Red);
    pyramid->SetPosition(Vector3(6.0f, 1.0f, 0.0f));
    pyramid->SetScale(Vector3(2.0f,1.0f,2.0f));

    auto cube1 = new CubeActor(this, Color::Green);
    cube1->SetPosition(Vector3(12.0f, 1.0f, 5.0f));
    cube1->SetScale(Vector3(5.0f,0.5f,1.0f));
    cube1->GetComponent<MeshComponent>()->SetBloomed(true);

    auto cube2 = new CubeActor(this, Color::Yellow);
    cube2->SetPosition(Vector3(7.5f, 1.0f, -2.5f));
    cube2->SetScale(Vector3(0.5f,2.5f,0.5f));
    cube2->GetComponent<MeshComponent>()->SetBloomed(true);

    auto cube3 = new CubeActor(this, Color::Blue);
    cube3->SetPosition(Vector3(5.5f, 0.5f, -7.5f));
    cube3->SetScale(Vector3(3.5f,2.5f,0.5f));
    cube3->SetRotation(Quaternion(0.7071f,0.0f,0.0f,0.7071f));
    cube3->GetComponent<MeshComponent>()->SetBloomed(true);
    
    // Create MIDI control actor for testing operations
    new MIDIControlActor(this);

    // Create test OBB actor (tilted 45 degrees)
    auto obbTest1 = new OBBTestActor(this);
    obbTest1->SetPosition(Vector3(5.0f, 1.0f, 0.0f)); 


    auto obbTest2 = new OBBTestActor(this);
    obbTest2->SetPosition(Vector3(6.0f, 1.0f,-4.0f));  // Place it to the right of spawn

    // Creating the Player actor
    mPlayer = new Player(this);
    mPlayer->SetPosition(Vector3(10.0f,1.0f,0.0f));

    // Creating a test HUD element
    auto hudElement = new HUDElement(this, "assets/textures/hud/hud.png", "assets/textures/hud/hud.json");
    hudElement->SetPosition(Vector3(-0.4f, 0.4f, 0.0f));
    hudElement->GetSpriteComponent().AddAnimation("default",{"hud1.png","hud2.png"});
    hudElement->GetSpriteComponent().SetAnimFPS(2.0f);
    hudElement->GetSpriteComponent().SetAnimation("default");

    // Setting camera
    SetCameraPos(mPlayer->GetPosition());
    SetCameraForward(Vector3::Normalize(Vector3(0.0f,-1.0f,-1.0f)));
}
    

void Game::SetCameraPos(Vector3 position)
{
    mCameraPos = position;
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
        
        // Get active actors
        UpdateActiveActors();

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
    std::cout << "Shutdown: Deleting " << mActors.size() << " actors..." << std::endl;
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
    // Normal removal: unregister from Chunk grid (if it still exists)
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
                else if (event.key.keysym.sym == SDLK_F11)
                {
                    // Toggle fullscreen (use desktop fullscreen for best compatibility)
                    if (mWindow)
                    {
                        Uint32 flags = SDL_GetWindowFlags(mWindow);
                        if (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP))
                        {
                            SDL_SetWindowFullscreen(mWindow, 0); // windowed
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                    }
                }
                else if (event.key.keysym.sym == SDLK_F1)
                {
                    mIsDebugging = !mIsDebugging;
                    
                    std::cout << "=== DEBUG MODE " << (mIsDebugging ? "ENABLED" : "DISABLED") << " ===" << std::endl;
                }
                break;
        }
    }
    
    // Get keyboard state for continuous input
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Get visible actors from chunk grid
    //std::vector<Actor*> mActiveActors = mChunkGrid->GetVisibleActors(mCameraPos);
    

    
    // Process input for all active actors
    for (auto actor : mActiveActors)
    {
        actor->ProcessInput(keyState);
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    
    // Get visible actors from chunk grid
    //std::vector<Actor*> mActiveActors = mChunkGrid->GetVisibleActors(mCameraPos);
    

    
    // Update all active actors
    for (auto actor : mActiveActors)
    {
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

void Game::UpdateActiveActors()
{
    mActiveActors = mChunkGrid->GetVisibleActors(mCameraPos);
    
    // Add always-active actors (but avoid duplicates if they're already visible)
    std::unordered_set<Actor*> actorSet(mActiveActors.begin(), mActiveActors.end());
    for (auto actor : mAlwaysActiveActors)
    {
        if (actorSet.insert(actor).second)
        {
            mActiveActors.push_back(actor);
        }
    }
}

void Game::UpdateGame(float deltaTime)
{
    // MIDI is updated in its own thread now, just update actors
    UpdateActors(deltaTime);
    
    // Check collisions after all actors have been updated
    CheckCollisions();
}

void Game::CheckCollisions()
{
    // Get visible actors from chunk grid
    
    // Collect all colliders from active actors
    std::vector<ColliderComponent*> colliders;
    for (auto actor : mActiveActors)
    {
        auto collider = actor->GetComponent<ColliderComponent>();
        if (collider)
        {
            colliders.push_back(collider);
        }
    }
    
    // Check collisions between all pairs
    for (size_t i = 0; i < colliders.size(); i++)
    {
        for (size_t j = i + 1; j < colliders.size(); j++)
        {
            ColliderComponent* colliderA = colliders[i];
            ColliderComponent* colliderB = colliders[j];
            
            // Check if they intersect
            if (colliderA->Intersect(*colliderB))
            {
                // Get penetration vector for A (how much to push A out of B)
                Vector3 penetrationA = colliderA->DetectCollision(*colliderB);
                
                // Determine who gets pushed based on static flags
                bool aIsStatic = colliderA->IsStatic();
                bool bIsStatic = colliderB->IsStatic();
                
                if (!aIsStatic && !bIsStatic)
                {
                    // Both dynamic - split the penetration
                    colliderA->GetOwner()->OnCollision(penetrationA * 0.5f, colliderB);
                    colliderB->GetOwner()->OnCollision(penetrationA * -0.5f, colliderA);
                }
                else if (!aIsStatic && bIsStatic)
                {
                    // A is dynamic, B is static - only push A
                    colliderA->GetOwner()->OnCollision(penetrationA, colliderB);
                }
                else if (aIsStatic && !bIsStatic)
                {
                    // A is static, B is dynamic - only push B
                    colliderB->GetOwner()->OnCollision(penetrationA * -1.0f, colliderA);
                }
                // else: both static - no response needed
            }
        }
    }
}

void Game::GenerateOutput()
{
    static Uint32 lastTime = SDL_GetTicks();
    static int frameCount = 0;
    Uint32 startFrame = SDL_GetTicks();
    
    // Update camera
    Vector3 targetPos = mCameraPos + mCameraForward;
    mRenderer->SetViewMatrix(Matrix4::CreateLookAt(mCameraPos, targetPos, mCameraUp));

    RendererMode mode = mIsDebugging ? RendererMode::LINES : RendererMode::TRIANGLES;
    
    // Get visible actors from chunk grid
    //std::vector<Actor*> mActiveActors = mChunkGrid->GetVisibleActors(mCameraPos);
    

    
    // Collect meshes and sprites from active actors
    std::vector<MeshComponent*> activeMeshes;
    std::vector<SpriteComponent*> activeSprites;
    bool hasBloom = false;
    
    for (auto actor : mActiveActors)
    {
        auto& components = actor->GetComponents();
        for (auto component : components)
        {
            if (auto mesh = dynamic_cast<MeshComponent*>(component))
            {
                if (mesh->IsVisible())
                {
                    activeMeshes.push_back(mesh);
                    if (mesh->IsBloomed())
                    {
                        hasBloom = true;
                    }
                }
            }
            else if (auto sprite = dynamic_cast<SpriteComponent*>(component))
            {
                if (sprite->IsVisible())
                {
                    activeSprites.push_back(sprite);
                    if (sprite->IsBloomed() && !sprite->IsHUD())
                    {
                        hasBloom = true;
                    }
                }
            }
        }
    }
    
    // BLOOM PASS: Render ALL objects to bloom framebuffer
    // Bloomed objects render normally, non-bloomed objects render as black for occlusion
    if (hasBloom)
    {
        mRenderer->BeginBloomPass();
        
        // Temporarily mark non-bloomed objects with negative color to render them black
        std::vector<Vector3> originalMeshColors;
        std::vector<Vector3> originalSpriteColors;
        
        // Save original colors and mark non-bloomed meshes
        for (auto* mesh : activeMeshes)
        {
            originalMeshColors.push_back(mesh->GetColor());
            if (!mesh->IsBloomed())
            {
                mesh->SetColor(Vector3(-1.0f, -1.0f, -1.0f));  // Negative = black in bloom pass
            }
        }
        
        // Separate world sprites and save colors
        std::vector<SpriteComponent*> worldSprites;
        for (auto* sprite : activeSprites)
        {
            if (!sprite->IsHUD())
            {
                worldSprites.push_back(sprite);
                originalSpriteColors.push_back(sprite->GetColor());
                if (!sprite->IsBloomed())
                {
                    sprite->SetColor(Vector3(-1.0f, -1.0f, -1.0f));  // Negative = black in bloom pass
                }
            }
        }
        
        // Render ALL meshes (bloomed and non-bloomed for occlusion)
        if (!activeMeshes.empty())
        {
            mRenderer->ActivateMeshShaderForBloom();
            mRenderer->DrawMeshesInstanced(activeMeshes, mode);
        }
        
        // Render ALL world sprites (bloomed and non-bloomed for occlusion)
        if (!worldSprites.empty())
        {
            mRenderer->ActivateSpriteShaderForBloom();
            mRenderer->DrawSpritesInstanced(worldSprites, mode);
        }
        
        // Restore original colors
        for (size_t i = 0; i < activeMeshes.size(); ++i)
        {
            activeMeshes[i]->SetColor(originalMeshColors[i]);
        }
        
        for (size_t i = 0; i < worldSprites.size(); ++i)
        {
            worldSprites[i]->SetColor(originalSpriteColors[i]);
        }
        
        mRenderer->EndBloomPass();
        
        // Apply Gaussian blur to bloom texture
        mRenderer->ApplyBloomBlur();
    }
    
    // Begin rendering to main framebuffer
    mRenderer->BeginFramebuffer();
   
    // Render meshes with instancing (batch draw)
    mRenderer->ActivateMeshShader();
    mRenderer->DrawMeshesInstanced(activeMeshes, mode);

    if (mIsDebugging) {
        for (auto actor : mActiveActors) {
            auto& components = actor->GetComponents();
            for (auto component : components) {
                component->DebugDraw(mRenderer);
            }
        }
    }
    
    // Render sprites with instancing (batch draw)
    mRenderer->ActivateSpriteShader();
    
    // Separate world sprites from HUD sprites
    std::vector<SpriteComponent*> worldSprites;
    std::vector<SpriteComponent*> hudSprites;
    for (auto* sprite : activeSprites) {
        if (sprite->IsHUD()) {
            hudSprites.push_back(sprite);
        }
        else {
            worldSprites.push_back(sprite);
        }
    }
    
    // Draw world sprites in 3D space
    mRenderer->DrawSpritesInstanced(worldSprites, mode);
    
    // End framebuffer rendering and display to screen
    mRenderer->EndFramebuffer();

    // Draw HUD sprites in screen space (after framebuffer)
    mRenderer->DrawHUDSprites(hudSprites);
    
   
    
    SDL_GL_SwapWindow(mWindow);
}
