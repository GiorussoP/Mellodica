#include "actors/GameActors.hpp"
#include "render/TextureAtlas.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"
#include "render/Texture.hpp"
#include <iostream>


// CameraController implementation
CameraController::CameraController(Game* game)
: Actor(game)
, mMoveForward(false)
, mMoveBackward(false)
, mMoveLeft(false)
, mMoveRight(false)
, mRotateLeft(false)
, mRotateRight(false)
, mRotateUp(false)
, mRotateDown(false)
{
    // Register as always-active so camera works even when far from origin
    game->AddAlwaysActive(this);
}

void CameraController::OnProcessInput(const Uint8* keyState)
{
    // Just store the input state - movement will happen in OnUpdate with proper deltaTime
    mMoveForward = keyState[SDL_SCANCODE_W];
    mMoveBackward = keyState[SDL_SCANCODE_S];
    mMoveLeft = keyState[SDL_SCANCODE_A];
    mMoveRight = keyState[SDL_SCANCODE_D];
    mRotateLeft = keyState[SDL_SCANCODE_LEFT];
    mRotateRight = keyState[SDL_SCANCODE_RIGHT];
    mRotateUp = keyState[SDL_SCANCODE_UP];
    mRotateDown = keyState[SDL_SCANCODE_DOWN];
}

void CameraController::OnUpdate(float deltaTime)
{
    Game* game = GetGame();
    Vector3 cameraPos = game->GetCameraPos();
    Vector3 cameraForward = game->GetCameraForward();
    Vector3 cameraUp = game->GetCameraUp();
    float yaw = game->GetCameraYaw();
    float pitch = game->GetCameraPitch();
    
    // Debug: log position and deltaTime
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter % 60 == 0) {
        std::cout << "Camera at (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z 
                  << ") - deltaTime: " << deltaTime << std::endl;
    }
    
    // Movement (WASD) - now using actual deltaTime!
    if (mMoveForward)
    {
        cameraPos += cameraForward * CAMERA_MOVE_SPEED * deltaTime;
    }
    if (mMoveBackward)
    {
        cameraPos -= cameraForward * CAMERA_MOVE_SPEED * deltaTime;
    }
    if (mMoveLeft)
    {
        Vector3 right = Vector3::Cross(cameraForward, cameraUp);
        right.Normalize();
        cameraPos += right * CAMERA_MOVE_SPEED * deltaTime;
    }
    if (mMoveRight)
    {
        Vector3 right = Vector3::Cross(cameraForward, cameraUp);
        right.Normalize();
        cameraPos -= right * CAMERA_MOVE_SPEED * deltaTime;
    }
    
    // Rotation (Arrow keys) - now using actual deltaTime!
    if (mRotateLeft)
    {
        yaw += CAMERA_ROTATE_SPEED * deltaTime;
    }
    if (mRotateRight)
    {
        yaw -= CAMERA_ROTATE_SPEED * deltaTime;
    }
    if (mRotateUp)
    {
        pitch += CAMERA_ROTATE_SPEED * deltaTime;
        if (pitch > 89.0f) pitch = 89.0f;
    }
    if (mRotateDown)
    {
        pitch -= CAMERA_ROTATE_SPEED * deltaTime;
        if (pitch < -89.0f) pitch = -89.0f;
    }
    
    // Update camera direction from yaw and pitch
    float yawRad = Math::ToRadians(yaw);
    float pitchRad = Math::ToRadians(pitch);
    
    cameraForward.x = Math::Sin(yawRad) * Math::Cos(pitchRad);
    cameraForward.y = Math::Sin(pitchRad);
    cameraForward.z = -Math::Cos(yawRad) * Math::Cos(pitchRad);  // Negative because -Z is forward
    cameraForward.Normalize();
    
    // Update game camera
    game->SetCameraPos(cameraPos);
    game->SetCameraForward(cameraForward);
    game->SetCameraYaw(yaw);
    game->SetCameraPitch(pitch);
}

// CubeActor implementation
CubeActor::CubeActor(Game* game, const Vector3& color,  unsigned int startingIndex)
: Actor(game)
, mMeshComponent(nullptr)
{
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
    
    // Get shared mesh from renderer cache (only one instance created)
    Mesh* mesh = game->GetRenderer()->LoadMesh("cube");

    mMeshComponent = new MeshComponent(this, *mesh, texture, atlas, startingIndex);
    mMeshComponent->SetColor(color);
}




PyramidActor::PyramidActor(Game* game, const Vector3& color,  unsigned int startingIndex)
: Actor(game)
, mMeshComponent(nullptr)
{

    // Get texture from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");

    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");

    // Get shared mesh from renderer cache (only one instance created)
    Mesh* mesh = game->GetRenderer()->LoadMesh("pyramid");

    // Create mesh component with shared mesh and atlas
    mMeshComponent = new MeshComponent(this, *mesh, texture, atlas, startingIndex);
    mMeshComponent->SetColor(color);
}

void PyramidActor::OnUpdate(float deltaTime) {
 
}

void CubeActor::OnUpdate(float deltaTime){

}

// SpriteActor implementation
SpriteActor::SpriteActor(Game* game)
: Actor(game)
, mSpriteComponent(nullptr)
{
    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Mario.json");
    
    // Get texture index from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Mario.png");
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    
    // Create sprite component with atlas
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
    
    // Setup running animation using atlas tile indices for Run1, Run2, Run3

    mSpriteComponent->AddAnimation("run",{"Run1.png","Run2.png","Run3.png"});
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetAnimFPS(8.0f);
}

void SpriteActor::OnUpdate(float deltaTime) {
 
}



GoombaActor::GoombaActor(Game* game)
: Actor(game)
, mSpriteComponent(nullptr)
{
    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Goomba.json");
    
    // Get texture index from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Goomba.png");
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    
    // Create sprite component with atlas
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

    // Setup running animation with Walk0 and Walk1 frames
    mSpriteComponent->AddAnimation("run", {"Walk0.png", "Walk1.png"});
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetAnimFPS(2.0f);
}

void GoombaActor::OnUpdate(float deltaTime) {
    // Update the sprite component
    mSpriteComponent->Update(deltaTime);
}
