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
      mFrontNote(-1) {
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
  mSpriteComponent->AddAnimation("damage", {"Dead.png"});

  mSpriteComponent->SetAnimation("idle");
  mSpriteComponent->SetAnimFPS(8.0f);

  // turn on Camera's isometric mode
  mGame->GetCamera()->SetMode(CameraMode::Isometric);
  mGame->GetCamera()->SetIsometricDirection(
      mGame->GetCamera()->GetIsometricDirection());

  // Not playing notes initially
  mPlayingNotes.fill(false);
}

void Player::OnUpdate(float deltaTime) {
  // Stop during battle transition
  if (mGame->GetBattleSystem()->IsTransitioning()) {
    mRigidBodyComponent->SetVelocity(Vector3::Zero);
    mSpriteComponent->SetAnimation("idle");
    return;
  }

  // Process notes playing
  bool playing = false;
  bool newPlayingNotes[12] = {false};
  for (int i = 0; i < 12; ++i) {
    if (Input::IsKeyDown(notebuttons[i])) {
      playing = true;
      newPlayingNotes[i] = true;
    }
  }

  if (Input::IsKeyDown(SDL_SCANCODE_SPACE)) {
    mFrontNote = mGame->GetBattleSystem()->GetPlayerPositionNote();
    playing = true;
    newPlayingNotes[mFrontNote] = true;
  } else {
    mFrontNote = -1;
  }

  for (int i = 0; i < 12; ++i) {
    if (mPlayingNotes[i] && !newPlayingNotes[i]) {
      if (mGame->GetBattleSystem()
                  ->GetPlayerNotePlayer()
                  ->GetActiveNotes()[i] != nullptr &&
          mGame->GetBattleSystem()
                  ->GetPlayerNotePlayer()
                  ->GetActiveNotes()[i]
                  ->GetMidiChannel() == 12) {

        // Stop note
        mGame->GetBattleSystem()->GetPlayerNotePlayer()->EndNote(
            i + 60); // MIDI note offset
      }
    } else if (newPlayingNotes[i] && !mPlayingNotes[i]) {
      // Play note
      mGame->GetBattleSystem()->GetPlayerNotePlayer()->PlayNote(
          i + 60, 12); // MIDI note offset
    }

    mPlayingNotes[i] = newPlayingNotes[i];
  }

  // Update allies positions
  if (!mGame->GetBattleSystem()->IsInBattle() &&
      !mGame->GetBattleSystem()->IsTransitioning()) {
    Vector3 pos = mPosition - mFront * 2.0f;
    for (auto &ally : mActiveAllies) {
      if (ally->GetCombatantState() == CombatantState::Dead)
        continue;

      if ((ally->GetPosition() - pos).LengthSq() < 9.0f) {
        ally->SetCombatantState(CombatantState::Idle);
      } else {
        ally->SetCombatantState(CombatantState::Moving);
        ally->GoToPositionAtSpeed(pos, PLAYER_MOVE_SPEED);
      }
    }
  }

  // Calculate movement direction
  Vector3 moveDir = Vector3::Zero;
  Vector3 front =
      mGame->GetCamera()->GetMode() == CameraMode::Isometric
          ? mGame->GetCamera()->GetCameraForward().ProjectedOnPlane(
                Vector3::UnitY)
          : mGame->GetCamera()->GetCameraUp().ProjectedOnPlane(Vector3::UnitY);
  front.Normalize();

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
    mSpriteComponent->SetAnimation(playing ? "play" : "run");

    mFront = moveDir;

  } else {
    mSpriteComponent->SetAnimation(playing ? "play" : "idle");
  }

  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetBloomed(playing ? true : mGame->GetRenderer()->IsDark());

  if (!mGame->GetBattleSystem()->IsInBattle()) {
    mGame->GetCamera()->SetTargetPosition(GetPosition() + mFront * 4.0f);
  }

  // DEBUGGING ALLY
  if (Input::WasKeyPressed(SDL_SCANCODE_Z) && mActiveAllies.size() < 8) {
    mActiveAllies.emplace_back(new Combatant(mGame, mActiveAllies.size(), 100));
    mActiveAllies.back()->SetPosition(mPosition + Vector3(2.0f, 0.0f, 0.0f));
    MIDIPlayer::unmuteChannel(mActiveAllies.back()->GetChannel());
  }
}

void Player::OnProcessInput() {
  if (mGame->GetBattleSystem()->IsTransitioning()) {
    mRigidBodyComponent->SetVelocity(Vector3::Zero);
    return;
  }

  mMoveForward = Input::IsKeyDown(SDL_SCANCODE_UP);
  mMoveBackward = Input::IsKeyDown(SDL_SCANCODE_DOWN);
  mMoveLeft = Input::IsKeyDown(SDL_SCANCODE_LEFT);
  mMoveRight = Input::IsKeyDown(SDL_SCANCODE_RIGHT);

  // Camera
  if (Input::WasKeyPressed(SDL_SCANCODE_D)) {
    mGame->GetCamera()->PrevIsometricDirection();
  }
  if (Input::WasKeyPressed(SDL_SCANCODE_A)) {
    mGame->GetCamera()->NextIsometricDirection();
  }
}

void Player::OnCollision(Vector3 penetration, ColliderComponent *other) {
  // Resolve collision by moving the player out of the other collider

  if (other->GetLayer() == ColliderLayer::Enemy ||
      other->GetLayer() == ColliderLayer::Entity) {
    return;
  }

  if (other->GetLayer() == ColliderLayer::Note) {
    // Take Damage
    mSpriteComponent->SetAnimation("damage");
    mSpriteComponent->SetColor(Color::Red);
    mSpriteComponent->SetBloomed(true);
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