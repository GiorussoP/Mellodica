#include "actors/GameActors.hpp"
#include "render/TextureAtlas.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"
#include "render/Texture.hpp"
#include <iostream>


// CameraController implementation
CameraController::CameraController(Game* game)
: Actor(game)
{
}

void CameraController::OnProcessInput(const Uint8* keyState)
{
    Game* game = GetGame();
    Vector3 cameraPos = game->GetCameraPos();
    Vector3 cameraForward = game->GetCameraForward();
    Vector3 cameraUp = game->GetCameraUp();
    float yaw = game->GetCameraYaw();
    float pitch = game->GetCameraPitch();
    
    // Movement (WASD)
    if (keyState[SDL_SCANCODE_W])
    {
        cameraPos += cameraForward * CAMERA_MOVE_SPEED * (1.0f / 60.0f);
    }
    if (keyState[SDL_SCANCODE_S])
    {
        cameraPos -= cameraForward * CAMERA_MOVE_SPEED * (1.0f / 60.0f);
    }
    if (keyState[SDL_SCANCODE_A])
    {
        Vector3 right = Vector3::Cross(cameraForward, cameraUp);
        right.Normalize();
        cameraPos += right * CAMERA_MOVE_SPEED * (1.0f / 60.0f);
    }
    if (keyState[SDL_SCANCODE_D])
    {
        Vector3 right = Vector3::Cross(cameraForward, cameraUp);
        right.Normalize();
        cameraPos -= right * CAMERA_MOVE_SPEED * (1.0f / 60.0f);
    }
    
    // Rotation (Arrow keys)
    if (keyState[SDL_SCANCODE_LEFT])
    {
        yaw += CAMERA_ROTATE_SPEED * (1.0f / 60.0f);
    }
    if (keyState[SDL_SCANCODE_RIGHT])
    {
        yaw -= CAMERA_ROTATE_SPEED * (1.0f / 60.0f);
    }
    if (keyState[SDL_SCANCODE_UP])
    {
        pitch += CAMERA_ROTATE_SPEED * (1.0f / 60.0f);
        if (pitch > 89.0f) pitch = 89.0f;
    }
    if (keyState[SDL_SCANCODE_DOWN])
    {
        pitch -= CAMERA_ROTATE_SPEED * (1.0f / 60.0f);
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
    // Rotate pyramid around Y axis (like CubeActor)
    float rotation = GetRotation();
    rotation -= Math::ToRadians(45.0) * deltaTime;  // Rotate 45 degrees per second
    SetRotation(rotation);
}

void CubeActor::OnUpdate(float deltaTime){
    // Rotate cube around Y axis
    float rotation = GetRotation();
    rotation += Math::ToRadians(45.0) * deltaTime;  // Rotate 45 degrees per second
    SetRotation(rotation);
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
    std::vector<int> runAnimFrames = {
        atlas->GetTileIndex("Run1.png"),
        atlas->GetTileIndex("Run2.png"),
        atlas->GetTileIndex("Run3.png")
    };
    mSpriteComponent->AddAnimation("run", runAnimFrames);
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetAnimFPS(8.0f);
    mSpriteComponent->SetApplyLighting(false);  // Disable lighting for sprite
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

    // Setup running animation with frames 1, 2 (Run1, Run2)
    mSpriteComponent->AddAnimation("run", {1, 2});
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetAnimFPS(2.0f);
    mSpriteComponent->SetApplyLighting(false);  // Disable lighting for sprite
}

void GoombaActor::OnUpdate(float deltaTime) {
    // Update the sprite component
    mSpriteComponent->Update(deltaTime);
}
