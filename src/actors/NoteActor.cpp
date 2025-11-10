#include "actors/NoteActor.hpp"
#include "Game.hpp"
#include "Renderer.hpp"
#include "SynthEngine.hpp"
#include "components/ColliderComponent.hpp"

NoteActor::NoteActor(Game *game, unsigned int midChannel, unsigned int midiNote,
                     Vector3 direction, Vector3 color, float speed)
    : Actor(game), mMidiChannel(midChannel), mMidiNote(midiNote),
      mIsPlaying(false), mDirection(direction), mSpeed(speed),
      mLastStepMovement(0.0f) {

  mGame->AddAlwaysActive(this);

  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
  mColliderComponent = new OBBCollider(this, ColliderLayer::Note, Vector3::Zero,
                                       Vector3(0.5f, 0.5f, 0.5f), false);

  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");

  mMeshComponent = new MeshComponent(this, *mesh, nullptr, nullptr, -1);
  mMeshComponent->SetColor(color);
  mMeshComponent->SetBloomed(true);
  // mColliderComponent = new

  SetScale(Vector3(0.4f, 0.4f, 0.1f));
  SetRotation(Math::LookRotation(mDirection));
}

void NoteActor::Start() {
  mIsPlaying = true;
  mRigidBodyComponent->SetVelocity(Vector3::Normalize(mDirection) * mSpeed);
}

void NoteActor::OnUpdate(float deltaTime) {

  mLastStepMovement = mSpeed * deltaTime;
  if (mIsPlaying)
    SetScale(mScale + Vector3(0.0f, 0.0f, 2.0f * mLastStepMovement));

  if (mScale.z <= 0.1f ||
      Vector3::Distance(mPosition, Vector3::Zero) > 100.0f) {
    SetState(ActorState::Destroy);
  }
}

void NoteActor::End() { mIsPlaying = false; }

void NoteActor::OnCollision(Vector3 penetration, ColliderComponent *other) {
  if (other->GetLayer() != ColliderLayer::Ground) {
    return;
  }

  if (mIsPlaying) {
    SetScale(mScale - Vector3(0.0f, 0.0f, 2.0 * mLastStepMovement));
    SetPosition(mPosition - mDirection * mLastStepMovement);
  } else {
    SetScale(mScale - Vector3(0.0f, 0.0f, mLastStepMovement));
    SetPosition(mPosition - mDirection * mLastStepMovement * 0.5f);
  }

  // Unused parameters
  (void)penetration;
  (void)other;
}