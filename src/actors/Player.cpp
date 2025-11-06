#include "Player.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "TextureAtlas.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"


Player::Player(Game* game)
    : Actor(game)
    , mMoveSpeed(600.0f)
    , mTurnSpeed(180.0f)
    , mCameraSpeed(5.0f)
    , mCameraTurnSpeed(90.0f)
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
    
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.5f, false);  // Disable gravity for now
    mColliderComponent = new SphereCollider(this, ColliderLayer::Player, Vector3::Zero, 0.5f);

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
    
    // Only normalize if there's movement
    if (moveDir.LengthSq() > 0.0f)
    {
        moveDir.Normalize();
        // Set velocity directly for responsive player control
        mRigidBodyComponent->ApplyForce(moveDir * mMoveSpeed * deltaTime);
    }
    else
    {
        // Stop moving if no input
        mRigidBodyComponent->SetVelocity(Vector3::Zero);
    }
    

    // Rotation (Arrow keys)
    if (mRotateLeft)
    {
        mCameraYaw +=  mCameraTurnSpeed * deltaTime;
    }
    if (mRotateRight)
    {
        mCameraYaw -= mCameraTurnSpeed * deltaTime;
    }
    if (mRotateUp)
    {
        mCameraPitch += mCameraTurnSpeed * deltaTime;
        if (mCameraPitch > 89.0f) mCameraPitch = 89.0f;
    }
    if (mRotateDown)
    {
        mCameraPitch -= mCameraTurnSpeed * deltaTime;
        if (mCameraPitch < -89.0f) mCameraPitch = -89.0f;
    }
    
    // Update camera direction from yaw and pitch
    float yawRad = Math::ToRadians(mCameraYaw);
    float pitchRad = Math::ToRadians(mCameraPitch);

    Vector3 cameraForward;
    cameraForward.x = Math::Sin(yawRad) * Math::Cos(pitchRad);
    cameraForward.y = Math::Sin(pitchRad);
    cameraForward.z = -Math::Cos(yawRad) * Math::Cos(pitchRad);  // Negative because -Z is forward
    cameraForward.Normalize();
    


   
    // Update game camera
    mGame->SetCameraPos(Vector3::Lerp(mGame->GetCameraPos(), mPosition, mCameraSpeed * deltaTime));
    mGame->SetCameraForward(cameraForward);
    mGame->SetCameraYaw(mCameraYaw);
    mGame->SetCameraPitch(mCameraPitch);
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
    SetPosition(GetPosition() + penetration);
    
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