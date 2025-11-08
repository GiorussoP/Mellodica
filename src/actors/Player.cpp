#include "Player.hpp"
#include "Game.hpp"
#include "Renderer.hpp"
#include "SpriteComponent.hpp"
#include "TestActors.hpp"
#include "Texture.hpp"
#include "TextureAtlas.hpp"

constexpr float PLAYER_MOVE_SPEED = 7.0f;
constexpr float PLAYER_ACCELERATION = 50.0f;
constexpr float PLAYER_FRICTION = 30.0f;
constexpr float CAMERA_MOVE_SPEED = 5.0f;
constexpr float CAMERA_TURN_SPEED = 90.0f;

Player::Player(Game *game)
    : Actor(game), mMoveForward(false), mMoveBackward(false), mMoveLeft(false),
      mMoveRight(false), mRotateLeft(false), mRotateRight(false),
      mRotateUp(false), mRotateDown(false), mTestCube(nullptr) {
  // Mark player as always active so it's always visible
  game->AddAlwaysActive(this);

  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, PLAYER_FRICTION,
                                               false); // Disable gravity

  mColliderComponent = new SphereCollider(this, ColliderLayer::Player,
                                          Vector3::Zero, 0.5f, false);

  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Mario.json");
  // Get texture index from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Mario.png");
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);

  // Create sprite component with atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  // Setup running animation using atlas tile indices for Run1, Run2, Run3
  mSpriteComponent->AddAnimation("run", {"Run1.png", "Run2.png", "Run3.png"});

  mSpriteComponent->AddAnimation("idle", {"Idle.png"});

  mSpriteComponent->SetAnimation("idle");
  mSpriteComponent->SetAnimFPS(8.0f);

  // mSpriteComponent->SetBloomed(true);
}

void Player::OnUpdate(float deltaTime) {
  // Calculate movement direction
  Vector3 moveDir = Vector3::Zero;
  Vector3 front = Vector3::Normalize(
      mGame->GetCameraForward().ProjectedOnPlane(Vector3::UnitY));
  if (mMoveForward) {
    moveDir += front;
  }
  if (mMoveBackward) {
    moveDir -= front;
  }

  Vector3 right = Vector3::Normalize(Vector3::Cross(Vector3::UnitY, front));
  if (mMoveLeft) {
    moveDir -= right;
  }
  if (mMoveRight) {
    moveDir += right;
  }

  if (mMoveLeft - mMoveRight == -1) {
    SetScale(Vector3(1.0, 1.0f, 1.0f));
  } else if (mMoveLeft - mMoveRight == 1) {
    SetScale(Vector3(-1.0f, 1.0f, 1.0f));
  }

  // Only normalize if there's movement
  if (moveDir.LengthSq() > 0.0f) {
    moveDir.Normalize();
    // Set velocity directly for responsive player control
    mRigidBodyComponent->ApplyForce(moveDir * PLAYER_ACCELERATION);

    if (mRigidBodyComponent->GetVelocity().LengthSq() >
        PLAYER_MOVE_SPEED * PLAYER_MOVE_SPEED) {
      mRigidBodyComponent->SetVelocity(
          Vector3::Normalize(mRigidBodyComponent->GetVelocity()) *
          PLAYER_MOVE_SPEED);
    }

    SetRotation(Math::LookRotation(moveDir));
    mSpriteComponent->SetAnimation("run");
  } else {
    mSpriteComponent->SetAnimation("idle");
  }

  // Rotation (Arrow keys) - rotate camera forward vector
  Vector3 cameraForward = mGame->GetCameraForward();
  Vector3 cameraUp = mGame->GetCameraUp();
  Vector3 cameraRight = Vector3::Cross(cameraUp, cameraForward);
  cameraRight.Normalize();

  const float TURN_SPEED_RAD = Math::ToRadians(CAMERA_TURN_SPEED) * deltaTime;

  if (mRotateLeft) {
    // Rotate around up axis (yaw left)
    Quaternion rotation(cameraUp, -TURN_SPEED_RAD);
    cameraForward = Vector3::Transform(cameraForward, rotation);
  }
  if (mRotateRight) {
    // Rotate around up axis (yaw right)
    Quaternion rotation(cameraUp, TURN_SPEED_RAD);
    cameraForward = Vector3::Transform(cameraForward, rotation);
  }
  if (mRotateUp) {
    // Rotate around right axis (pitch up)
    Quaternion rotation(cameraRight, -TURN_SPEED_RAD);
    Vector3 newForward = Vector3::Transform(cameraForward, rotation);
    // Clamp pitch to prevent flipping (check if new forward is not too
    // vertical)
    if (Math::Abs(newForward.y) < 0.999f) {
      cameraForward = newForward;
    }
  }
  if (mRotateDown) {
    // Rotate around right axis (pitch down)
    Quaternion rotation(cameraRight, TURN_SPEED_RAD);
    Vector3 newForward = Vector3::Transform(cameraForward, rotation);
    // Clamp pitch to prevent flipping
    if (Math::Abs(newForward.y) < 0.999f) {
      cameraForward = newForward;
    }
  }

  cameraForward.Normalize();

  // Update game camera
  mGame->SetCameraPos(Vector3::Lerp(mGame->GetCameraPos(), mPosition,
                                    CAMERA_MOVE_SPEED * deltaTime));
  mGame->SetCameraForward(cameraForward);

  // DEBUG
  if (mTestCube) {
    mTestCube->SetScale(mTestCube->GetScale() +
                        Vector3(8.0f, 0.0f, 0.0f) * deltaTime);
    mTestCube->SetPosition(mTestCube->GetPosition() -
                           Vector3(4.0f, 0.0f, 0.0f) * deltaTime);
  }
}

void Player::OnProcessInput() {
  mMoveForward = Input::IsKeyDown(SDL_SCANCODE_W);
  mMoveBackward = Input::IsKeyDown(SDL_SCANCODE_S);
  mMoveLeft = Input::IsKeyDown(SDL_SCANCODE_A);
  mMoveRight = Input::IsKeyDown(SDL_SCANCODE_D);
  mRotateLeft = Input::IsKeyDown(SDL_SCANCODE_LEFT);
  mRotateRight = Input::IsKeyDown(SDL_SCANCODE_RIGHT);
  mRotateUp = Input::IsKeyDown(SDL_SCANCODE_UP);
  mRotateDown = Input::IsKeyDown(SDL_SCANCODE_DOWN);

  if (Input::WasKeyPressed(SDL_SCANCODE_V)) {
    // Testing instancing
    mTestCube = new CubeActor(mGame, Color::White);
    mTestCube->SetPosition(mPosition - Vector3(1.0f, 0.0f, 0.0f));
    mTestCube->SetScale(Vector3(0.5f, 0.5f, 0.5f));
    mTestCube->GetComponent<MeshComponent>()->SetBloomed(true);
  }

  if (Input::WasKeyReleased(SDL_SCANCODE_V)) {
    if (mTestCube) {
      mTestCube->SetState(ActorState::Destroy);
      mTestCube = nullptr;
    }
  }
}

void Player::OnCollision(Vector3 penetration, ColliderComponent *other) {
  // Resolve collision by moving the player out of the other collider
  // The penetration vector tells us how much to move to separate the colliders
  SetPosition(GetPosition() + penetration.ProjectedOnPlane(Vector3::UnitY));

  // Stop velocity in the direction of the collision to prevent sliding back in
  if (mRigidBodyComponent && penetration.LengthSq() > 0.0f) {
    Vector3 currentVelocity = mRigidBodyComponent->GetVelocity();
    Vector3 penetrationDir = Vector3::Normalize(penetration);

    // Project velocity onto the penetration direction
    float velocityInPenetrationDir =
        Vector3::Dot(currentVelocity, penetrationDir);

    // If velocity is pushing us back into the collision, remove that component
    if (velocityInPenetrationDir < 0.0f) {
      // Remove the velocity component in the penetration direction
      Vector3 correctedVelocity =
          currentVelocity - (penetrationDir * velocityInPenetrationDir);
      mRigidBodyComponent->SetVelocity(correctedVelocity);
    }
  }

  (void)other; // Suppress unused parameter warning for now
}