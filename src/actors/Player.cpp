#include "Player.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "TextureAtlas.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"

// Local camera move speed for player
constexpr float CAMERA_MOVE_SPEED = 5.0f;

Player::Player(Game* game)
    : Actor(game)
    , mMoveSpeed(600.0f)
    , mTurnSpeed(180.0f)
    , mMoveForward(false)
    , mMoveBackward(false)
    , mMoveLeft(false)
    , mMoveRight(false)
    , mRotateLeft(false)
    , mRotateRight(false)
    , mRotateUp(false)
    , mRotateDown(false)
{
    // Mark player as always active so it's always visible
    game->AddAlwaysActive(this);
    
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f, false);  // Disable gravity for now
    //mColliderComponent = new SphereCollider(this, ColliderLayer::Player, Vector3::Zero, 0.5f);

    mColliderComponent = new AABBCollider(this,ColliderLayer::Player,Vector3::Zero,Vector3(0.5f),false);

    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Mario.json");
    // Get texture index from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Mario.png");
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    
    // Create sprite component with atlas
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
    
    // Setup running animation using atlas tile indices for Run1, Run2, Run3
    mSpriteComponent->AddAnimation("run",{"Run1.png","Run2.png","Run3.png"});

    mSpriteComponent->AddAnimation("idle", {"Idle.png"});

    mSpriteComponent->SetAnimation("idle");
    mSpriteComponent->SetAnimFPS(8.0f);
}

void Player::OnUpdate(float deltaTime)
{
    // Calculate movement direction
    Vector3 moveDir = Vector3::Zero;
    Vector3 front = Vector3::Normalize(mGame->GetCameraForward().ProjectedOnPlane(Vector3::UnitY));
    if (mMoveForward)
    {
        moveDir += front;
    }
    if (mMoveBackward)
    {
        moveDir -= front;
    }
    

    Vector3 right = Vector3::Normalize(Vector3::Cross(Vector3::UnitY, front));
    if (mMoveLeft)
    {
        moveDir -= right;
    }
    if (mMoveRight)
    {
        moveDir += right;
    }

    if(mMoveLeft - mMoveRight == -1){
        SetScale(Vector3(1.0, 1.0f,1.0f));
    }
    else if(mMoveLeft - mMoveRight == 1){
        SetScale(Vector3(-1.0f, 1.0f, 1.0f));
    }
    
    // Only normalize if there's movement
    if (moveDir.LengthSq() > 0.0f)
    {
        moveDir.Normalize();
        // Set velocity directly for responsive player control
        mRigidBodyComponent->ApplyForce(moveDir * mMoveSpeed * deltaTime);

        if(mRigidBodyComponent->GetVelocity().LengthSq()>PLAYER_MOVE_SPEED * PLAYER_MOVE_SPEED){
            mRigidBodyComponent->SetVelocity(Vector3::Normalize(mRigidBodyComponent->GetVelocity()) * PLAYER_MOVE_SPEED);
        }

        SetRotation(Math::LookRotation(moveDir));
        mSpriteComponent->SetAnimation("run");
    }
    else {
        mSpriteComponent->SetAnimation("idle");
    }
    

    // Rotation (Arrow keys) - rotate camera forward vector
    Vector3 cameraForward = mGame->GetCameraForward();
    Vector3 cameraUp = mGame->GetCameraUp();
    Vector3 cameraRight = Vector3::Cross(cameraUp, cameraForward);
    cameraRight.Normalize();
    
    const float TURN_SPEED_RAD = Math::ToRadians(CAMERA_TURN_SPEED) * deltaTime;
    
    if (mRotateLeft)
    {
        // Rotate around up axis (yaw left)
        Quaternion rotation(cameraUp, -TURN_SPEED_RAD);
        cameraForward = Vector3::Transform(cameraForward, rotation);
    }
    if (mRotateRight)
    {
        // Rotate around up axis (yaw right)
        Quaternion rotation(cameraUp, TURN_SPEED_RAD);
        cameraForward = Vector3::Transform(cameraForward, rotation);
    }
    if (mRotateUp)
    {
        // Rotate around right axis (pitch up)
        Quaternion rotation(cameraRight, -TURN_SPEED_RAD);
        Vector3 newForward = Vector3::Transform(cameraForward, rotation);
        // Clamp pitch to prevent flipping (check if new forward is not too vertical)
        if (Math::Abs(newForward.y) < 0.999f)
        {
            cameraForward = newForward;
        }
    }
    if (mRotateDown)
    {
        // Rotate around right axis (pitch down)
        Quaternion rotation(cameraRight, TURN_SPEED_RAD);
        Vector3 newForward = Vector3::Transform(cameraForward, rotation);
        // Clamp pitch to prevent flipping
        if (Math::Abs(newForward.y) < 0.999f)
        {
            cameraForward = newForward;
        }
    }
    
    cameraForward.Normalize();

    // Update game camera
    mGame->SetCameraPos(Vector3::Lerp(mGame->GetCameraPos(), mPosition, CAMERA_MOVE_SPEED * deltaTime));
    mGame->SetCameraForward(cameraForward);
}

void Player::OnProcessInput(const Uint8* keyState)
{
    mMoveForward = keyState[SDL_SCANCODE_W];
    mMoveBackward = keyState[SDL_SCANCODE_S];
    mMoveLeft = keyState[SDL_SCANCODE_A];
    mMoveRight = keyState[SDL_SCANCODE_D];
    mRotateLeft = keyState[SDL_SCANCODE_LEFT];
    mRotateRight = keyState[SDL_SCANCODE_RIGHT];
    mRotateUp = keyState[SDL_SCANCODE_UP];
    mRotateDown = keyState[SDL_SCANCODE_DOWN];
}

void Player::OnCollision(Vector3 penetration, ColliderComponent* other)
{
    // Resolve collision by moving the player out of the other collider
    // The penetration vector tells us how much to move to separate the colliders
    SetPosition(GetPosition() + penetration.ProjectedOnPlane(Vector3::UnitY));
    
    // Stop velocity in the direction of the collision to prevent sliding back in
    if (mRigidBodyComponent && penetration.LengthSq() > 0.0f)
    {
        Vector3 currentVelocity = mRigidBodyComponent->GetVelocity();
        Vector3 penetrationDir = Vector3::Normalize(penetration);
        
        // Project velocity onto the penetration direction
        float velocityInPenetrationDir = Vector3::Dot(currentVelocity, penetrationDir);
        
        // If velocity is pushing us back into the collision, remove that component
        if (velocityInPenetrationDir < 0.0f)
        {
            // Remove the velocity component in the penetration direction
            Vector3 correctedVelocity = currentVelocity - (penetrationDir * velocityInPenetrationDir);
            mRigidBodyComponent->SetVelocity(correctedVelocity);
        }
    }
    
    (void)other; // Suppress unused parameter warning for now
}