#include "Player.hpp"
#include "Game.hpp"
#include "MIDIPlayer.hpp"
#include "NoteActor.hpp"
#include "Renderer.hpp"
#include "SpriteComponent.hpp"
#include "SynthEngine.hpp"
#include "TestActors.hpp"

#include "Texture.hpp"
#include "TextureAtlas.hpp"

constexpr float PLAYER_MOVE_SPEED = 7.0f;
constexpr float PLAYER_ACCELERATION = 50.0f;
constexpr float PLAYER_FRICTION = 30.0f;

constexpr SDL_Scancode notebuttons[12] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,     SDL_SCANCODE_4,
    SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,     SDL_SCANCODE_8,
    SDL_SCANCODE_9, SDL_SCANCODE_0, SDL_SCANCODE_MINUS, SDL_SCANCODE_EQUALS};

Player::Player(Game *game)
    : Actor(game), mMoveForward(false), mMoveBackward(false), mMoveLeft(false),
      mMoveRight(false), mRotateLeft(false), mRotateRight(false),
      mRotateUp(false), mRotateDown(false), mFront(Vector3::UnitZ),
      mCameraDirection(0) {
  // Mark player as always active so it's always visible
  game->AddAlwaysActive(this);

  // Initialize all note pointers to nullptr
  // mActiveNotes.fill(nullptr);

  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, PLAYER_FRICTION,
                                               false); // Disable gravity

  mColliderComponent = new SphereCollider(this, ColliderLayer::Player,
                                          Vector3::Zero, 0.5f, false);

  // mColliderComponent =
  //     new OBBCollider(this, ColliderLayer::Player, Vector3::Zero,
  //                    Vector3(0.5f, 0.5f, 0.5f), false);

  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Mario.json");
  // Get texture index from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Mario.png");
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);

  // Create sprite component with atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  // Setup running animation using atlas tile indices for
  // Run1, Run2, Run3
  mSpriteComponent->AddAnimation("run", {"Run1.png", "Run2.png", "Run3.png"});
  mSpriteComponent->AddAnimation("play", {"Stomp1.png", "Stomp2.png"});
  mSpriteComponent->AddAnimation("idle", {"Idle.png"});

  mSpriteComponent->SetAnimation("idle");
  mSpriteComponent->SetAnimFPS(8.0f);

  mNotePlayerComponent = new NotePlayerComponent(this, false);

  // turn on Camera's isometric mode
  mGame->GetCamera()->SetMode(CameraMode::Isometric);
  mGame->GetCamera()->SetIsometricDirection(static_cast<IsometricDirections>(mCameraDirection));
}

void Player::OnUpdate(float deltaTime) {
  // Calculate movement direction
  Vector3 moveDir = Vector3::Zero;
  Vector3 front = Vector3::Normalize(
      mGame->GetCamera()->GetCameraForward().ProjectedOnPlane(Vector3::UnitY));
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

  bool playing = false;
  for (auto note : mNotePlayerComponent->GetActiveNotes()) {
    if (note != nullptr) {
      playing = true;
      break;
    }
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
    mSpriteComponent->SetAnimation(playing ? "play" : "run");

    mFront = moveDir;

  } else {
    mSpriteComponent->SetAnimation(playing ? "play" : "idle");
  }

  mGame->GetCamera()->SetMode(CameraMode::Isometric);
  mGame->GetCamera()->SetTargetPosition(GetPosition() + mFront * 4.0f);
  mGame->GetCamera()->SetIsometricDirection(static_cast<IsometricDirections>(mCameraDirection));
}

void Player::OnProcessInput() {
  mMoveForward = Input::IsKeyDown(SDL_SCANCODE_UP);
  mMoveBackward = Input::IsKeyDown(SDL_SCANCODE_DOWN);
  mMoveLeft = Input::IsKeyDown(SDL_SCANCODE_LEFT);
  mMoveRight = Input::IsKeyDown(SDL_SCANCODE_RIGHT);

  // Camera
  if (Input::WasKeyPressed(SDL_SCANCODE_D)) {
    mCameraDirection = (mCameraDirection + 7) % 8;
  }
  if (Input::WasKeyPressed(SDL_SCANCODE_A)) {
    mCameraDirection = (mCameraDirection + 1) % 8;
  }

  Vector3 right = Vector3::Cross(Vector3::UnitY, mFront);

  for (int i = 0; i < 12; ++i) {
    if (Input::WasKeyPressed(notebuttons[i])) {
      if (mNotePlayerComponent->PlayNote(60 + i, 12)) {
        SynthEngine::startNote(12, 60 + i);
      };
    } else if (Input::WasKeyReleased(notebuttons[i])) {
      if (mNotePlayerComponent->EndNote(60 + i)) {
        // mActiveNotes[i]->End();
        // mActiveNotes[i] = nullptr;
        SynthEngine::stopNote(12, 60 + i);
      }
    }
  }
}

void Player::OnCollision(Vector3 penetration, ColliderComponent *other) {
  // Resolve collision by moving the player out of the other collider
  // The penetration vector tells us how much to move to separate the
  // colliders
  if (other->GetLayer() == ColliderLayer::Note) {
    // Ignore collisions with notes for now
    return;
  }
  SetPosition(GetPosition() + penetration.ProjectedOnPlane(Vector3::UnitY));

  // Stop velocity in the direction of the collision to prevent sliding back
  // in
  if (mRigidBodyComponent && penetration.LengthSq() > 0.0f) {
    Vector3 currentVelocity = mRigidBodyComponent->GetVelocity();
    Vector3 penetrationDir = Vector3::Normalize(penetration);

    // Project velocity onto the penetration direction
    float velocityInPenetrationDir =
        Vector3::Dot(currentVelocity, penetrationDir);

    // If velocity is pushing us back into the collision, remove that
    // component
    if (velocityInPenetrationDir < 0.0f) {
      // Remove the velocity component in the penetration direction
      Vector3 correctedVelocity =
          currentVelocity - (penetrationDir * velocityInPenetrationDir);
      mRigidBodyComponent->SetVelocity(correctedVelocity);
    }
  }

  (void)other; // Suppress unused parameter warning for now
}