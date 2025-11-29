#ifndef COMBATANT_HPP

#define COMBATANT_HPP

#include "actors/Actor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/RigidBodyComponent.hpp"
#include "components/SpriteComponent.hpp"

#include <string>

const float COMBATANT_MOVE_SPEED = 5.0f;

enum class CombatantState { Idle, Attacking, Dead, Moving };

class Combatant : public Actor {
public:
  Combatant(class Game *game, int channel, int health = 1000);
  virtual ~Combatant();

  virtual void InitializeSprite() = 0;

  void OnUpdate(float deltaTime) override;
  int GetHealth() const { return mHealth; }
  void SetHealth(int health) { mHealth = health; }
  void SetMaxHealth(int maxHealth) { mMaxHealth = maxHealth; }
  int GetMaxHealth() const { return mMaxHealth; }
  int GetChannel() const { return mChannel; }

  CombatantState GetCombatantState() const { return mCombatantState; }
  void SetCombatantState(CombatantState state) { mCombatantState = state; }

  void GoToPositionAtSpeed(Vector3 &position,
                           float moveSpeed = COMBATANT_MOVE_SPEED) {
    mTargetPosition = position;
    mMoveSpeed = moveSpeed;
  }

  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

  void SetSpriteColorByChannel();

protected:
  SpriteComponent *mSpriteComponent;

private:
  int mHealth;
  int mMaxHealth;
  int mChannel;

  CombatantState mCombatantState;

  ColliderComponent *mColliderComponent;
  RigidBodyComponent *mRigidBodyComponent;

  Vector3 mTargetPosition;
  float mMoveSpeed;
};

#endif