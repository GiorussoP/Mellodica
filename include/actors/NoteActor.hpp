#ifndef NOTEACTOR_HPP
#define NOTEACTOR_HPP

#include "Actor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/RigidBodyComponent.hpp"

class NotePlayerComponent;

class NoteActor : public Actor {
public:
  NoteActor(class Game *game, unsigned int midChannel, unsigned int midiNote,
            Vector3 direction = Vector3::UnitZ, Vector3 color = Color::White,
            float speed = 10.0f);

  NoteActor(NotePlayerComponent *NotePlayerComponent, class Game *game,
            unsigned int midChannel, unsigned int midiNote,
            Vector3 direction = Vector3::UnitZ, Vector3 color = Color::White,
            float speed = 10.0f)
      : NoteActor(game, midChannel, midiNote, direction, color, speed) {
    mNotePlayerComponent = NotePlayerComponent;
  }

  void Start();
  void OnUpdate(float deltaTime) override;
  void End();
  void OnCollision(Vector3 penetration, ColliderComponent *other) override;

  unsigned int GetMidiChannel() const { return mMidiChannel; }
  unsigned int GetNote() const { return mMidiNote; }

private:
  unsigned int mMidiChannel;
  unsigned int mMidiNote;
  bool mIsPlaying;
  Vector3 mDirection;
  float mSpeed;
  float mLastStepMovement;

  RigidBodyComponent *mRigidBodyComponent;
  ColliderComponent *mColliderComponent;
  MeshComponent *mMeshComponent;

  NotePlayerComponent *mNotePlayerComponent;
};
#endif