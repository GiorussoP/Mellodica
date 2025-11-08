#include "components/Component.hpp"
#include "actors/Actor.hpp"

Component::Component(Actor *owner, int updateOrder)
    : mOwner(owner), mUpdateOrder(updateOrder), mIsEnabled(true) {
  mOwner->AddComponent(this);
}

Component::~Component() {}

void Component::Update(float deltaTime) { (void)deltaTime; }

void Component::ProcessInput() {}

Game *Component::GetGame() const { return mOwner->GetGame(); }
