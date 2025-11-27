#ifndef COMBATANT_HPP

#define COMBATANT_HPP

#include "actors/Actor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/RigidBodyComponent.hpp"
#include "components/SpriteComponent.hpp"

#include <string>

const float COMBATANT_MOVE_SPEED = 5.0f;

enum class CombatantState {
  Idle,
  Attacking,
  Dodging,
  Dead,
};

class Combatant : public Actor {
public:
  Combatant(class Game *game, int channel, int health = 100,
            const std::string &texture_name = "./assets/textures/Goomba");

  ~Combatant();

  void OnUpdate(float deltaTime) override;
  int GetHealth() const { return mHealth; }
  void SetHealth(int health) { mHealth = health; }
  int GetChannel() const { return mChannel; }

  CombatantState GetCombatantState() const { return mCombatantState; }
  void SetCombatantState(CombatantState state) { mCombatantState = state; }

  void GoToPositionAtSpeed(Vector3 &position,
                           float moveSpeed = COMBATANT_MOVE_SPEED) {
    mTargetPosition = position;
    mMoveSpeed =
        Math::Min(moveSpeed, 5.0f); // Cap max speed to prevent teleporting
  }

  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

private:
  int mHealth;
  int mChannel;

  CombatantState mCombatantState;

  ColliderComponent *mColliderComponent;
  SpriteComponent *mSpriteComponent;
  RigidBodyComponent *mRigidBodyComponent;

  Vector3 mTargetPosition;
  float mMoveSpeed;
};

#endif