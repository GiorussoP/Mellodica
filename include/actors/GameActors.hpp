#pragma once
#include "actors/Actor.hpp"
#include "Game.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "Math.hpp"

// Camera movement speed constants
const float CAMERA_MOVE_SPEED = 5.0f;
const float CAMERA_ROTATE_SPEED = 90.0f;

// Camera control class
class CameraController : public Actor
{
public:
    CameraController(Game* game);
    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    
private:
    // Store input state to apply in Update with deltaTime
    bool mMoveForward;
    bool mMoveBackward;
    bool mMoveLeft;
    bool mMoveRight;
    bool mRotateLeft;
    bool mRotateRight;
    bool mRotateUp;
    bool mRotateDown;
};

// Simple cube actor with MeshComponent
class CubeActor : public Actor
{
public:
    CubeActor(Game* game, const Vector3& color = Color::White, unsigned int startingIndex = 0);
    void OnUpdate(float deltaTime) override;
    
private:
    MeshComponent* mMeshComponent;
};



// Simple pyramid actor with MeshComponent
class PyramidActor : public Actor
{
public:
    PyramidActor(Game* game, const Vector3& color = Color::White, unsigned int startingIndex = 0);
    void OnUpdate(float deltaTime) override;

private:
    MeshComponent* mMeshComponent;
};

// Grass cube - all faces use tile 0
class GrassCubeActor : public CubeActor
{
public:
    GrassCubeActor(Game* game, const Vector3& color = Color::White) 
        : CubeActor(game, color, 0) {}
};

// Rock cube - all faces use tile 1
class RockCubeActor : public CubeActor
{
public:
    RockCubeActor(Game* game, const Vector3& color = Color::White)
        : CubeActor(game, color, 4) {}
};

// Rotating sprite actor
class SpriteActor : public Actor
{
public:
    SpriteActor(Game* game);
    void OnUpdate(float deltaTime) override;
    
private:
    SpriteComponent* mSpriteComponent;
};

class GoombaActor : public Actor
{
public:
    GoombaActor(Game* game);
    void OnUpdate(float deltaTime) override;
    
private:
    SpriteComponent* mSpriteComponent;
};


