#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Actor.hpp"
#include "actors/Combatant.hpp"
#include "actors/NoteActor.hpp"
#include "actors/NotePlayerActor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/RigidBodyComponent.hpp"
#include "components/SpriteComponent.hpp"
#include <array>

class Player : public Actor {
public:
  Player(class Game *game);
  void OnUpdate(float deltaTime) override;
  void OnProcessInput() override;
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

  std::vector<Combatant *> &GetActiveAllies() { return mActiveAllies; }

  int getHealth() const { return mHealth; }
  int getMaxHealth() const { return mMaxHealth; }

  float getEnergy() const { return mEnergy; }
  float getMaxEnergy() const { return mMaxEnergy; }

private:
  bool mMoveForward;
  bool mMoveBackward;
  bool mMoveLeft;
  bool mMoveRight;
  bool mRotateLeft;
  bool mRotateRight;
  bool mRotateUp;
  bool mRotateDown;

  int mHealth;
  int mMaxHealth;

  float mEnergy;
  float mMaxEnergy;

  RigidBodyComponent *mRigidBodyComponent;
  ColliderComponent *mColliderComponent;
  SpriteComponent *mSpriteComponent;

  Vector3 mFront;

  unsigned int mCameraDirection;

  std::vector<Combatant *> mActiveAllies;

  std::array<bool, 12> mPlayingNotes;
  unsigned int mFrontNote;
};

#endif