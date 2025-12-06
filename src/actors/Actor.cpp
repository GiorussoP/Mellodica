#include "actors/Actor.hpp"
#include "ChunkGrid.hpp"
#include "Game.hpp"
#include "components/Component.hpp"
#include <algorithm>

Actor::Actor(Game *game)
    : mGame(game), mState(ActorState::Active), mPosition(Vector3::Zero),
      mScale(1.0f), mRotation(Quaternion::Identity) {
  // Game handles all registration (renderer, chunk grid, etc.)
  mGame->AddActor(this);
}

Actor::~Actor() {
  // Don't call RemoveActor if we're already being destroyed during game shutdown
  // or if the game is null
  if (mGame) {
    mGame->RemoveActor(this);
  }

  // Delete components - make a copy to avoid iterator invalidation
  std::vector<Component *> componentsToDelete = mComponents;
  mComponents.clear();
  
  for (auto comp : componentsToDelete) {
    delete comp;
  }
}

void Actor::Update(float deltaTime) {
  if (mState == ActorState::Active) {
    // Update components
    for (auto comp : mComponents) {
      if (comp->IsEnabled()) {
        comp->Update(deltaTime);
      }
    }

    // Actor-specific update
    OnUpdate(deltaTime);
  }
}

void Actor::ProcessInput() {
  if (mState == ActorState::Active) {
    // Process components input
    for (auto &comp : mComponents) {
      if (comp->IsEnabled()) {
        comp->ProcessInput();
      }
    }

    // Actor-specific input
    OnProcessInput();
  }
}

void Actor::AddComponent(Component *c) {
  // Insert component maintaining update order
  int order = c->GetUpdateOrder();
  auto iter = mComponents.begin();
  for (; iter != mComponents.end(); ++iter) {
    if (order < (*iter)->GetUpdateOrder()) {
      break;
    }
  }

  mComponents.insert(iter, c);
}

void Actor::SetPosition(const Vector3 pos) {
  mPosition = pos;

  // Automatically update chunk grid when position changes
  mGame->GetChunkGrid()->UpdateActor(this);
}
