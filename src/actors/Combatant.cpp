#include "Combatant.hpp"
#include "../../include/actors/Combatant.hpp"

#include "Game.hpp"
#include "actors/NoteActor.hpp"
#include "render/Renderer.hpp"

Combatant::Combatant(Game *game, int channel, int health, CombatantType type)
    : Actor(game), mHealth(health), mMaxHealth(health), mChannel(channel % 8),
      mCombatantState(CombatantState::Idle), mTargetPosition(GetPosition()),
      mMoveSpeed(COMBATANT_MOVE_SPEED), mSpriteComponent(nullptr),
      mCombatantType(type){
  mGame->AddAlwaysActive(this);
  mColliderComponent = new SphereCollider(this, ColliderLayer::Entity,
                                          Vector3::Zero, 0.5f, false);
  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
}

Combatant::~Combatant() {}

void Combatant::OnUpdate(float deltaTime) {
  if (mCombatantState == CombatantState::Dead) {
    mRigidBodyComponent->SetVelocity(Vector3::Zero);
    mSpriteComponent->SetAnimation("dead");
    mSpriteComponent->SetBloomed(false);
    return;
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
      mSpriteComponent->SetBloomed(false);
      return;
    }
    direction.Normalize();
    Vector3 velocity = direction * mMoveSpeed;
    mRigidBodyComponent->SetVelocity(velocity);
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

void Combatant::SetSpriteColorByChannel() {
  if (mSpriteComponent) {
    mSpriteComponent->SetColor(NOTE_COLORS[mChannel % 16]);
  }
}