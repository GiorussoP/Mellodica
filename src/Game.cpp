#include "Game.hpp"
#include "actors/Actor.hpp"
#include "components/DrawComponent.hpp"
#include "render/Renderer.hpp"
#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"
#include "actors/GameActors.hpp"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FPS = 60;
const int FRAME_TIME = 1000 / FPS;

Game::Game()
: mUpdatingActors(false)
, mWindow(nullptr)
, mGLContext(nullptr)
, mRenderer(nullptr)
, mCameraPos(0.0f, 3.0f, 10.0f)
, mCameraForward(0.0f, 0.0f, -1.0f)  // Looking toward negative Z
, mCameraUp(0.0f, 1.0f, 0.0f)
, mCameraYaw(0.0f)
, mCameraPitch(-15.0f)  // Looking down 15 degrees
, mTicksCount(0)
, mIsRunning(true)
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
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
    
    // Initialize game actors
    InitializeActors();
    
    return true;
}

void Game::InitializeActors()
{
    std::cout << "Shared resources created!" << std::endl;
    
    // Create camera controller
    new CameraController(this);
    
    // Create a grid of cube actors (alternating grass, rock, and red cubes)
    const int gridSize = 3;
    const float spacing = 2.5f;
    const float gridOffset = (gridSize - 1) * spacing * 0.5f;
    
    for (int x = 0; x < gridSize; x++)
    {
        for (int z = 0; z < gridSize; z++)
        {
            // Alternate between grass, rock, and red cubes
            Actor* cube = nullptr;
            Actor* sprite = nullptr;
            int pattern = (x + z) % 3;
            if (pattern == 0) {
                cube = new GrassCubeActor(this);
                sprite = new GoombaActor(this);
            } else if (pattern == 1) {
                cube = new RockCubeActor(this);
                sprite = new GoombaActor(this);
            } else {
                // Red color-only cube (no texture)
                cube = new PyramidActor(this, Color::Red,{3});
                sprite = new SpriteActor(this);
            }
            
            Vector3 cubePos = Vector3(
                x * spacing - gridOffset,
                0.0f,
                z * spacing - gridOffset
            );
            cube->SetPosition(cubePos);
            // All cubes at default size 1.0
            
            // Create a Mario sprite on top of each cube
            
            sprite->SetPosition(Vector3(cubePos.x, cubePos.y + 1.0f, cubePos.z));
            // Sprite defaults to scale 1.0, making it 1x1 unit (same as cube)
        }
    }
    
    std::cout << "Initialized " << gridSize * gridSize << " cube actors and " << gridSize * gridSize << " sprite actors" << std::endl;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame(1.0f / static_cast<float>(FPS));  // Fixed timestep
        GenerateOutput();
        
        // Frame rate limiting
        Uint32 frameTime = SDL_GetTicks() - mTicksCount;
        if (frameTime < FRAME_TIME)
        {
            SDL_Delay(FRAME_TIME - frameTime);
        }
        mTicksCount = SDL_GetTicks();
    }
}

void Game::Shutdown()
{
    // Delete actors (which will delete their components)
    while (!mActors.empty())
    {
        delete mActors.back();
    }
    
    if (mRenderer)
    {
        mRenderer->Shutdown();
        delete mRenderer;
        mRenderer = nullptr;
    }
    
    if (mGLContext)
    {
        SDL_GL_DeleteContext(mGLContext);
        mGLContext = nullptr;
    }
    
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    
    SDL_Quit();
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
    }
}

void Game::RemoveActor(Actor* actor)
{
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

void Game::AddDrawable(DrawComponent* drawable)
{
    // Simply add to the end - depth testing will handle draw order
    mDrawables.push_back(drawable);
}

void Game::RemoveDrawable(DrawComponent* drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if (iter != mDrawables.end())
    {
        mDrawables.erase(iter);
    }
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
    
    // Process input for all actors
    for (auto actor : mActors)
    {
        actor->ProcessInput(keyState);
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;
    
    // Move pending actors to active
    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
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
    // Update all actors
    UpdateActors(deltaTime);
}

void Game::GenerateOutput()
{
    mRenderer->Clear();
    
    // Update camera
    Vector3 targetPos = mCameraPos + mCameraForward;
    mRenderer->SetViewMatrix(Matrix4::CreateLookAt(mCameraPos, targetPos, mCameraUp));
    mRenderer->SetCameraPosition(mCameraPos);
    
    // Draw all drawable components
    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);
    }
    
    SDL_GL_SwapWindow(mWindow);
}
