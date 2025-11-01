#include "components/Component.hpp"
#include "actors/Actor.hpp"

Component::Component(Actor* owner, int updateOrder)
: mOwner(owner)
, mUpdateOrder(updateOrder)
, mIsEnabled(true)
{
    mOwner->AddComponent(this);
}

Component::~Component()
{
}

void Component::Update(float deltaTime)
{
    (void)deltaTime;
}

void Component::ProcessInput(const Uint8* keyState)
{
    (void)keyState;
}

Game* Component::GetGame() const
{
    return mOwner->GetGame();
}
