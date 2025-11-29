#include "Combatant.hpp"
#include "Game.hpp"
#include "actors/NoteActor.hpp"
#include "render/Renderer.hpp"

Combatant::Combatant(Game *game, int channel, int health,
                     const std::string &texture_name)
    : Actor(game), mHealth(health), mMaxHealth(health), mChannel(channel % 8),
      mCombatantState(CombatantState::Idle), mTargetPosition(GetPosition()),
      mMoveSpeed(COMBATANT_MOVE_SPEED) {

  mGame->AddAlwaysActive(this);
  // Get atlas from renderer cache
  TextureAtlas *atlas = game->GetRenderer()->LoadAtlas(texture_name + ".json");

  // Get texture index from renderer cache
  Texture *texture = game->GetRenderer()->LoadTexture(texture_name + ".png");
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);

  // Create sprite component with atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  // Setup running animation with Walk0 and Walk1 frames
  mSpriteComponent->AddAnimation("run", {"Walk0.png", "Walk1.png"});
  mSpriteComponent->AddAnimation("idle", {"Walk0.png"});
  mSpriteComponent->AddAnimation("dead", {"Dead.png"});
  mSpriteComponent->SetAnimation("idle");

  mSpriteComponent->SetAnimFPS(2.0f);

  mColliderComponent = new SphereCollider(this, ColliderLayer::Entity,
                                          Vector3::Zero, 0.5f, false);

  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);

  mSpriteComponent->SetColor(NOTE_COLORS[channel % 8]);
}

Combatant::~Combatant() {}

void Combatant::OnUpdate(float deltaTime) {
  if (mCombatantState == CombatantState::Dead) {
    mRigidBodyComponent->SetVelocity(Vector3::Zero);
    mSpriteComponent->SetAnimation("dead");
    mSpriteComponent->SetBloomed(false);
    return;
  }

  if (mGame->GetBattleSystem()->IsInBattle() == false && mHealth < mMaxHealth) {
    // Regenerate health outside of battle
    mHealth += 1;
  }

  if (mCombatantState == CombatantState::Idle) {
    mRigidBodyComponent->SetVelocity(Vector3::Zero);
    mSpriteComponent->SetAnimation("idle");
    mSpriteComponent->SetBloomed(false);
    return;
  }

  if (mCombatantState == CombatantState::Attacking) {
    mSpriteComponent->SetBloomed(true);
    mRigidBodyComponent->SetVelocity(Vector3::Zero);
    return;
  }

  if (mCombatantState == CombatantState::Moving) {
    // Move towards target position
    Vector3 direction = (mTargetPosition - GetPosition());
    float distanceToTarget = direction.Length();
    if (distanceToTarget < 0.1f) { // Close enough to target
      SetPosition(mTargetPosition);
      mRigidBodyComponent->SetVelocity(Vector3::Zero);
      mCombatantState = CombatantState::Idle;
      mSpriteComponent->SetAnimation("idle");
      return;
    }
    direction.Normalize();
    Vector3 velocity = direction * mMoveSpeed;
    mRigidBodyComponent->SetVelocity(velocity);
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetBloomed(false);
  }
}

void Combatant::OnCollision(Vector3 penetration, ColliderComponent *other) {
  if (mCombatantState == CombatantState::Dead) {
    return;
  }

  if (other->GetLayer() == ColliderLayer::Enemy ||
      (mGame->GetBattleSystem()->IsInBattle() &&
       (other->GetLayer() == ColliderLayer::Entity ||
        other->GetLayer() == ColliderLayer::Player))) {
    return;
  }
  if (other->GetLayer() == ColliderLayer::Note &&
      mGame->GetBattleSystem()->IsInBattle()) {
    mHealth -= 1; // Template damage value
    if (mHealth <= 0) {
      mHealth = 0;
      mCombatantState = CombatantState::Dead;
    }
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