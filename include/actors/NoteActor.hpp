#ifndef NOTEACTOR_HPP
#define NOTEACTOR_HPP

#include "Actor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/RigidBodyComponent.hpp"

const Vector3 NOTE_COLORS[16] = {
    Color::Red,     Color::Green, Color::Yellow, Color::Blue,
    Color::Magenta, Color::Cyan,  Color::Purple, Color::Orange,
    Color::White,   Color::White, Color::White,  Color::White,
    Color::White,   Color::White, Color::White,  Color::White};

class NotePlayerActor;

class NoteActor : public Actor {
public:
  NoteActor(class Game *game, unsigned int midChannel, unsigned int midiNote,
            Vector3 direction = Vector3::UnitZ, Vector3 color = Color::White,
            float speed = 1.0f);

  NoteActor(NotePlayerActor *NotePlayerActor, class Game *game,
            unsigned int midChannel, unsigned int midiNote,
            Vector3 direction = Vector3::UnitZ, Vector3 color = Color::White,
            float speed = 1.0f)
      : NoteActor(game, midChannel, midiNote, direction, color, speed) {
    mNotePlayerActor = NotePlayerActor;
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

  class ShineActor *mShineActor;

  RigidBodyComponent *mRigidBodyComponent;
  ColliderComponent *mColliderComponent;
  MeshComponent *mMeshComponent;

  NotePlayerActor *mNotePlayerActor;
};
#endif