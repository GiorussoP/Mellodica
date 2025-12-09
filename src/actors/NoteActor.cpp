#include "actors/NoteActor.hpp"
#include "Game.hpp"
#include "MIDI/SynthEngine.hpp"
#include "components/ColliderComponent.hpp"
#include "render/Renderer.hpp"

#include "MIDI/MIDIPlayer.hpp"
#include "actors/ShineActor.hpp"

const float SHINE_TIME = 0.5f;

NoteActor::NoteActor(Game *game, unsigned int midChannel, unsigned int midiNote,
                     Vector3 direction, Vector3 color, float speed)
    : Actor(game), mMidiChannel(midChannel), mMidiNote(midiNote),
      mIsPlaying(false), mDirection(direction), mSpeed(speed),
      mLastStepMovement(0.0f), mNotePlayerActor(nullptr), mShineActor(nullptr) {

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

NoteActor::~NoteActor() {
  if (mNotePlayerActor) {
    mNotePlayerActor->MarkNoteDead(this);
  }
  // Don't delete mShineActor - it's managed by the game's actor list
  // Just clear our pointer to it
  mShineActor = nullptr;
}

void NoteActor::Start() {
  mIsPlaying = true;
  mRigidBodyComponent->SetVelocity(Vector3::Normalize(mDirection) * mSpeed);

  mShineActor = new ShineActor(mGame, mMeshComponent->GetColor(), false);
  mShineActor->SetPosition(mPosition - mDirection * mScale.z * 0.5f);
  mShineActor->Start(SHINE_TIME);
  mShineActor->GetComponent<SpriteComponent>()->SetBloomed(true);
}

//

void NoteActor::OnUpdate(float deltaTime) {

  mLastStepMovement = mSpeed * deltaTime;
  if (mIsPlaying) {
    SetScale(mScale + Vector3(0.0f, 0.0f, mLastStepMovement));
    SetPosition(mPosition - 0.5f * mDirection * mLastStepMovement);
  }

  if (mGame->GetPlayer() &&
      (mScale.z < 0.1f ||
       Vector3::Distance(mPosition, mGame->GetPlayer()->GetPosition()) >
           50.0f)) {
    mIsPlaying = false;
    SetState(ActorState::Destroy);
    // Enable auto-destroy on shine - it will destroy after animation finishes
    if (mShineActor) {
      mShineActor->SetAutoDestroy(true);
    }
    if (mNotePlayerActor)
      mNotePlayerActor->MarkNoteDead(this);
  }
}

void NoteActor::End() { mIsPlaying = false; }

void NoteActor::OnCollision(Vector3 penetration, ColliderComponent *other) {

  if (other->GetLayer() == ColliderLayer::Hole) {
    return;
  }

  if (other->GetLayer() == ColliderLayer::Entity) {
    Combatant *otherCombatant = dynamic_cast<Combatant *>(other->GetOwner());
    if (otherCombatant &&
        otherCombatant->GetCombatantState() == CombatantState::Dead)
      return;
  } else if (other->GetLayer() == ColliderLayer::Note &&
             (!mGame->GetBattleSystem() ||
              !mGame->GetBattleSystem()->IsInBattle()) &&
             Vector3::Dot(
                 dynamic_cast<NoteActor *>(other->GetOwner())->mDirection,
                 mDirection) > 0.9f) {
    return;
  }

  // Check if ShineActor still exists before accessing it
  if (mShineActor && mShineActor->GetState() != ActorState::Destroy) {
    mShineActor->Start(SHINE_TIME);
    mShineActor->SetPosition(mPosition + mDirection * mScale.z * 0.5f);
  }

  if (mIsPlaying) {
    SetScale(mScale - Vector3(0.0f, 0.0f, mLastStepMovement));
    SetPosition(mPosition - 0.5f * mDirection * mLastStepMovement);
  } else {
    SetScale(mScale - Vector3(0.0f, 0.0f, mLastStepMovement));
    SetPosition(mPosition - mDirection * mLastStepMovement * 0.5f);
  }

  if (!mGame->GetBattleSystem() || !mGame->GetBattleSystem()->IsInBattle()) {
    MIDIPlayer::playSequence(
        {{0.0f, 14, static_cast<int>(mMidiNote), true, 20},
         {mLastStepMovement, 14, static_cast<int>(mMidiNote), false, 20}});
  }

  // Unused parameters
  (void)penetration;
  (void)other;
}
