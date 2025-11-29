//
// Created by luiza on 29/11/2025.
//
#pragma once
#include <iostream>
#include "actors/Actor.hpp"
#include "actors/NoteActor.hpp"
#include "components/MeshComponent.hpp"
#include "components/ColliderComponent.hpp"
#include "components/SpriteComponent.hpp"


class ItemActor : public Actor {
public:
    ItemActor(class Game* game);
    void OnUpdate(float deltaTime) override;
    void OnCollision(Vector3 penetration, ColliderComponent* other) override;

private:
    float mAnimTime;
};


class MusicButtonActor : public Actor {
public:
    MusicButtonActor(class Game* game, int midiTarget);

    void OnCollision(Vector3 penetration, ColliderComponent* other) override;

    bool IsActivated() const { return mIsActivated; }

private:
    void Activate();

    int mTargetNote;
    bool mIsActivated;
    class MeshComponent* mMeshComp;
    class OBBCollider* mCollider;
};

