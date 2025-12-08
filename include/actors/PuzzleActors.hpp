//
// Created by luiza on 29/11/2025.
//
#pragma once
#include "MelodyComponent.hpp"
#include "actors/Actor.hpp"
#include "actors/NoteActor.hpp"
#include "actors/SceneActors.hpp"
#include "components/ColliderComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include <iostream>

class ItemActor : public Actor {
public:
  ItemActor(class Game *game);
  void OnUpdate(float deltaTime) override;
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

private:
  float mAnimTime;

protected:
  class MeshComponent *mMeshComponent;
  class SphereCollider *mCollider;
};

class HPItemActor : public ItemActor {
public:
  HPItemActor(class Game *game);
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;
};

class MovableBox : public SolidCubeActor {
public:
  MovableBox(Game *game, const Vector3 &color = Vector3(0.5f))
      : SolidCubeActor(game, color, 8), mIsInHole(false) {
    mColliderComponent->SetStatic(false);
  };
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

protected:
  bool mIsInHole;
};

class BreakableBox : public MovableBox {
public:
  BreakableBox(Game *game, const Vector3 &color = Color::White)
      : MovableBox(game, color), mHealth(50) {
    mColliderComponent->SetStatic(false);
  };
  void OnUpdate(float deltaTime) override;
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

private:
  int mHealth;
};

class MusicButtonActor : public Actor {
public:
  MusicButtonActor(class Game *game, std::vector<int> targetMelody);

  void OnUpdate(float deltaTime) override;
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

  bool IsActivated() const { return mIsActivated; }

private:
  void Activate();

  bool mIsActivated;
  class MelodyComponent *mMelodyComp;
  class MeshComponent *mMeshComp;
  class OBBCollider *mCollider;
};
