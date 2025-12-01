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

class SolidCubeActor : public CubeActor {
public:
  SolidCubeActor(Game *game, const Vector3 &color = Color::White,
                 int startingIndex = -1);
  void OnUpdate(float deltaTime) override;

private:
  ColliderComponent *mColliderComponent;
};

class DirtCubeActor : public SolidCubeActor {
public:
  DirtCubeActor(Game *game, const Vector3 &color = Color::White)
      : SolidCubeActor(game, color, 0) {}
};

class GrassCubeActor : public SolidCubeActor {
public:
  GrassCubeActor(Game *game, const Vector3 &color = Color::White)
      : SolidCubeActor(game, color, 2) {}
};

class RockCubeActor : public SolidCubeActor {
public:
  RockCubeActor(Game *game, const Vector3 &color = Color::White)
      : SolidCubeActor(game, color, 4) {}
};

class SandCubeActor : public SolidCubeActor {
public:
  SandCubeActor(Game *game, const Vector3 &color = Color::White)
      : SolidCubeActor(game, color, 6) {}
};

class TreeActor : public Actor {
public:
  TreeActor(Game *game);

private:
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
};

class SmallRockActor : public Actor {
public:
  SmallRockActor(Game *game);

private:
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
};

class MediumRockActor : public Actor {
public:
  MediumRockActor(Game *game);

private:
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
};

class BushActor : public Actor {
public:
  BushActor(Game *game);

private:
  SpriteComponent *mSpriteComponent;
  ColliderComponent *mColliderComponent;
};

class GrassActorA : public Actor {
public:
  GrassActorA(Game *game);

private:
  SpriteComponent *mSpriteComponent;
};

class GrassActorB : public Actor {
public:
  GrassActorB(Game *game);

private:
  SpriteComponent *mSpriteComponent;
};

class GrassActorC : public Actor {
public:
  GrassActorC(Game *game);

private:
  SpriteComponent *mSpriteComponent;
};

class WallActor : public Actor {
public:
  WallActor(Game *game, const Vector3 &color = Color::White,
            int startingIndex = -1);
  void OnUpdate(float deltaTime) override;

private:
  MeshComponent *mMeshComponent;
};

class SolidWallActor : public WallActor {
public:
  SolidWallActor(Game *game, const Vector3 &color = Color::White,
                 int startingIndex = -1);
  void OnUpdate(float deltaTime) override;

private:
  ColliderComponent *mColliderComponent;
};

class GrassWall : public SolidWallActor {
public:
  GrassWall(Game *game, const Vector3 &color = Color::White)
      : SolidWallActor(game, color, 0) {}
};

class RockWall : public SolidWallActor {
public:
  RockWall(Game *game, const Vector3 &color = Color::White)
      : SolidWallActor(game, color, 4) {}
};

class DoorWall : public SolidWallActor {
public:
  DoorWall(Game *game, const Vector3 &color = Color::White)
      : SolidWallActor(game, color, 16) {}
};

class WindowWall : public SolidWallActor {
public:
  WindowWall(Game *game, const Vector3 &color = Color::White)
      : SolidWallActor(game, color, 12) {}
};

class EntranceWall : public WallActor {
public:
  EntranceWall(Game *game, const Vector3 &color = Color::White)
      : WallActor(game, color, 8) {}
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

class HouseActor : public Actor {
public:
  HouseActor(Game *game);

private:
  MeshComponent *mMeshComponent1;
  MeshComponent *mMeshComponent2;
  ColliderComponent *mColliderComponent;
};

// TODO: change to use collider
class TriggerActor : public Actor {
public:
  TriggerActor(Game *game);
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

private:
  bool mTriggered;
};