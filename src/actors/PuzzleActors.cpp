//
// Created by luiza on 29/11/2025.
//

#include "actors/PuzzleActors.hpp"
#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "render/Renderer.hpp"
#include "actors/Player.hpp"
#include "actors/ShineActor.hpp"

ItemActor::ItemActor(Game *game) : Actor(game), mAnimTime(0.0f) {
  SetScale(Vector3(0.5f, 0.5f, 0.5f));

  Mesh *mesh = game->GetRenderer()->LoadMesh("sphere");
  mMeshComponent = new MeshComponent(this, *mesh);
  mMeshComponent->SetColor(Vector3(1.0f, 1.0f, 0.0f));
  mMeshComponent->SetBloomed(true);

  mCollider = new SphereCollider(this, ColliderLayer::Entity, Vector3::Zero,
                                 0.5f, false);
}

void ItemActor::OnUpdate(float deltaTime) {

  mAnimTime += deltaTime;

  Quaternion rot = GetRotation();
  Quaternion inc = Quaternion(Vector3::UnitY, deltaTime * 2.0f);
  SetRotation(Quaternion::Concatenate(rot, inc));

  if (mGame->GetBattleSystem()->IsInBattle()) {
    mMeshComponent->SetOffset(Vector3(0.0f, 1.0f, 0.0f));
  } else {
    mMeshComponent->SetOffset(
        Vector3(0.0f, sin(mAnimTime * 3.0f) * 0.5f, 0.0f));
  }
}

void ItemActor::OnCollision(Vector3 penetration, ColliderComponent *other) {
  if (other->GetLayer() == ColliderLayer::Player) {
    std::cout << "Item Coletado!" << std::endl;

    // AQUI: lógica de inventário / poderes

    // Animação de brilho ao coletar
    auto shine = new ShineActor(mGame, mMeshComponent->GetColor());
    shine->SetPosition(mPosition);
    shine->Start(0.5f);
    SetState(ActorState::Destroy);
  }
}

HPItemActor::HPItemActor(Game *game) : ItemActor(game) {
  mMeshComponent->SetColor(Vector3(0.0f, 1.0f, 0.0f));
}

void HPItemActor::OnCollision(Vector3 penetration, ColliderComponent *other) {
  if (other->GetLayer() == ColliderLayer::Player) {
    std::cout << "HP Item Coletado! Vida restaurada." << std::endl;

    Player *player = dynamic_cast<Player *>(other->GetOwner());
    if (player) {
      player->AddHealth(100);
    }
    ItemActor::OnCollision(penetration, other);

    // Power-up sound effect
    MIDIPlayer::playSequence({{0.0f, 15, 62, true, 100},
                              {0.0f, 15, 66, true, 80},
                              {0.1f, 15, 62, false},
                              {0.0f, 15, 66, false},
                              {0.0f, 15, 69, true, 100},
                              {0.0f, 15, 74, true, 80},
                              {0.1f, 15, 69, false},
                              {0.0f, 15, 74, false},
                              {0.0f, 15, 78, true, 120},
                              {0.2f, 15, 78, false}});
  }
}

void BreakableBox::OnUpdate(float deltaTime) {
  // Restore color to white when not being hit
  if (auto meshComp = GetComponent<MeshComponent>()) {
    meshComp->SetColor(Color::White);
    meshComp->SetBloomed(false);
  }
}

void BreakableBox::OnCollision(Vector3 penetration, ColliderComponent *other) {
  if (other->GetLayer() == ColliderLayer::Note) {

    // Turn red and bloom when hit
    if (auto meshComp = GetComponent<MeshComponent>()) {
      meshComp->SetColor(Color::Red);
      meshComp->SetBloomed(true);
    }

    // Decrease health
    mHealth -= 1;
    if (mHealth < 0) {
      mHealth = 0;
    }

    // Only destroy if health reaches 0
    if (mHealth <= 0) {
      auto shine = new ShineActor(mGame, Vector3(0.3f, 0.1f, 0.0f));

      shine->SetPosition(mPosition);
      shine->GetComponent<SpriteComponent>()->SetBloomed(false);
      shine->Start(0.5f);
      shine->SetScale(Vector3(1.5f, -1.5f, 1.5f));

      SetState(ActorState::Destroy);

      // Play destroying sound on drum channel 13
      MIDIPlayer::playSequence(
          {{0.0f, 13, 38, true, 127}, {0.2f, 13, 38, false}});
    }
  } else
    MovableBox::OnCollision(penetration, other);
}

void MovableBox::OnCollision(Vector3 penetration, ColliderComponent *other) {
  // Don't respond to collisions if box is already in a hole
  if (other->GetLayer() != ColliderLayer::Hole &&
      other->GetLayer() != ColliderLayer::Note &&
      other->GetLayer() != ColliderLayer::Entity) {

    if (mIsInHole)
      return;

    // Move Only on the strongest component direction
    Vector3 move = penetration;
    if (std::abs(penetration.x) > std::abs(penetration.z)) {
      move.z = 0.0f;
    } else {
      move.x = 0.0f;
    }

    move.y = 0.0f;

    if (move.Length() > 0.001f) {
      MIDIPlayer::playSequence(
          {{0.0f, 13, 29, true, 20}, {0.1f, 13, 29, false}});
      mPosition += move;
    }
  } else if (other->GetLayer() == ColliderLayer::Hole) {

    // Get hole's AABB bounds
    const AABBCollider *holeCollider = static_cast<const AABBCollider *>(other);
    Vector3 holeMin = holeCollider->GetMin();
    Vector3 holeMax = holeCollider->GetMax();

    // Check if box center is fully inside the hole's horizontal bounds
    bool fullyInsideX = mPosition.x - mScale.x / 2.01f >= holeMin.x &&
                        mPosition.x + mScale.x / 2.01f < holeMax.x;
    bool fullyInsideZ = mPosition.z - mScale.z / 2.01f >= holeMin.z &&
                        mPosition.z + mScale.z / 2.01f < holeMax.z;

    if (fullyInsideX && fullyInsideZ) {
      // Box is fully over hole - mark it and start falling
      mIsInHole = true;

      if (mPosition.y > 0.5f) {
        mPosition.y = Math::Lerp(mPosition.y, 0.5f, 0.1f);

        if (mPosition.y <= 0.6f) {
          mPosition.y = 0.5f;
        } else {
          // Note pitch proportional to height
          int note = static_cast<int>(Math::Lerp(30.0f, 90.0f, mPosition.y));
          // Play falling sound on channel 15
          MIDIPlayer::playSequence(
              {{0.0f, 15, note, true, 100}, {0.01f, 15, note, false}});
        }
      }
    }
    // else: Box is not fully over the hole - ignore collision and let player
    // keep pushing
  }
}

MusicButtonActor::MusicButtonActor(Game *game, int midiTarget)
    : Actor(game), mTargetNote(midiTarget), mIsActivated(false) {

  SetScale(Vector3(2.0f, 2.0f, 2.0f));

  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");
  mMeshComp = new MeshComponent(this, *mesh);
  mMeshComp->SetColor(Vector3(0.5f, 0.5f, 0.5f));

  mCollider = new OBBCollider(this, ColliderLayer::Ground, Vector3::Zero,
                              Vector3(0.5f, 0.5f, 0.5f), false);
}

void MusicButtonActor::OnCollision(Vector3 penetration,
                                   ColliderComponent *other) {

  if (mIsActivated)
    return;

  if (other->GetLayer() == ColliderLayer::Note) {
    NoteActor *note = dynamic_cast<NoteActor *>(other->GetOwner());

    if (note) {

      int hitNote = note->GetNote();
      if (hitNote % 12 == mTargetNote % 12) {
        Activate();
      }
    }
  }
}

void MusicButtonActor::Activate() {
  mIsActivated = true;
  std::cout << "Puzzle Resolvido! Nota correta: " << mTargetNote << std::endl;

  // Só faz o bloco brilhar
  // mMeshComp->SetColor(Vector3(0.0f, 1.0f, 0.0f));
  // mMeshComp->SetBloomed(true);

  ShineActor *shine = new ShineActor(GetGame(), mMeshComp->GetColor());
  shine->SetPosition(GetPosition());
  shine->Start(0.5f);

  // Item
  ItemActor *item = new ItemActor(GetGame());

  item->SetPosition(GetPosition() + Vector3(0.0f, 0.0f, 0.0f));

  SetState(ActorState::Destroy);
}
