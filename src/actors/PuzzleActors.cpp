//
// Created by luiza on 29/11/2025.
//

#include "actors/PuzzleActors.hpp"
#include "Game.hpp"
#include "Renderer.hpp"
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

  Vector3 pos = GetPosition();
  pos.y += sin(mAnimTime * 3.0f) * 0.01f;
  SetPosition(pos);
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
  }
}

MusicButtonActor::MusicButtonActor(Game *game, int midiTarget)
    : Actor(game), mTargetNote(midiTarget), mIsActivated(false) {

  SetScale(Vector3(2.0f, 2.0f, 2.0f));

  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");
  mMeshComp = new MeshComponent(this, *mesh);
  mMeshComp->SetColor(Vector3(0.5f, 0.5f, 0.5f));

  mCollider = new OBBCollider(this, ColliderLayer::Entity, Vector3::Zero,
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
