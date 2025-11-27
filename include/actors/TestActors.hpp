#pragma once
#include "Game.hpp"
#include "Math.hpp"
#include "actors/Actor.hpp"
#include "actors/NotePlayerActor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/RigidBodyComponent.hpp"
#include "components/SpriteComponent.hpp"

// Simple cube actor with MeshComponent
class CubeActor : public Actor {
public:
  CubeActor(Game *game, const Vector3 &color = Color::White,
            int startingIndex = -1);
  void OnUpdate(float deltaTime) override;

private:
  MeshComponent *mMeshComponent;
  // ColliderComponent* mColliderComponent;
};

class GroundActor : public Actor {
public:
  GroundActor(Game *game, const Vector3 &color = Color::White,
              int startingIndex = -1);
  void OnUpdate(float deltaTime) override;

private:
  MeshComponent *mMeshComponent;
};

// Simple pyramid actor with MeshComponent
class PyramidActor : public Actor {
public:
  PyramidActor(Game *game, const Vector3 &color = Color::White,
               int startingIndex = -1);
  void OnUpdate(float deltaTime) override;

private:
  MeshComponent *mMeshComponent;
};

class GrassCubeActor : public CubeActor {
public:
  GrassCubeActor(Game *game, const Vector3 &color = Color::White)
      : CubeActor(game, color, 0) {}
};

class RockCubeActor : public CubeActor {
public:
  RockCubeActor(Game *game, const Vector3 &color = Color::White)
      : CubeActor(game, color, 4) {}
};

class MarioActor : public Actor {
public:
  MarioActor(Game *game);
  void OnUpdate(float deltaTime) override;

private:
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
  RigidBodyComponent *mRigidBodyComponent;
};

class GoombaActor : public Actor {
public:
  GoombaActor(Game *game);
  void OnUpdate(float deltaTime) override;

private:
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
  NotePlayerActor *mNotePlayerActor;
};

// Test actor with tilted OBB collider
class OBBTestActor : public Actor {
public:
  OBBTestActor(Game *game);
  void OnUpdate(float deltaTime) override;

private:
  ColliderComponent *mColliderComponent;
  MeshComponent *mMeshComponent;
};

// Actor that demonstrates thread-safe MIDI control from keyboard input
class MIDIControlActor : public Actor {
public:
  MIDIControlActor(Game *game);
  void OnProcessInput() override;

private:
  // Debounce flags to prevent repeated key presses
  bool mPrevSpacePressed;
  bool mPrevRPressed;
  bool mPrevJPressed;
  bool mPrevMPressed;
  bool mPrevUPressed;
  bool mPrevPlusPressed;
  bool mPrevMinusPressed;
  bool mPrevPPressed;
};

class MultiDrawablesActor : public Actor {
public:
  MultiDrawablesActor(Game *game);
  void OnUpdate(float deltaTime) override;

private:
  MeshComponent *mMeshComponent1;
  MeshComponent *mMeshComponent2;
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
};