#include "components/DrawComponent.hpp"
#include "Game.hpp"
#include "actors/Actor.hpp"

DrawComponent::DrawComponent(Actor* owner)
: Component(owner, 50)  // Draw components update at order 50
, mIsVisible(true)
, mColor(Vector3(1.0f, 1.0f, 1.0f))
{
    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    mOwner->GetGame()->RemoveDrawable(this);
}

void DrawComponent::Draw(Renderer* renderer)
{
    (void)renderer;
}
