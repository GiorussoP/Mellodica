//
// Created by luiza on 29/11/2025.
//
#pragma once
#include "actors/Actor.hpp"
#include "actors/NoteActor.hpp"
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

class MusicButtonActor : public Actor {
public:
  MusicButtonActor(class Game *game, int midiTarget);

  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

  bool IsActivated() const { return mIsActivated; }

private:
  void Activate();

  int mTargetNote;
  bool mIsActivated;
  class MeshComponent *mMeshComp;
  class OBBCollider *mCollider;
};
