#include "components/DrawComponent.hpp"
#include "Game.hpp"
#include "actors/Actor.hpp"

DrawComponent::DrawComponent(Actor *owner)
    : Component(owner, 50) // Draw components update at order 50
      ,
      mIsVisible(true), mIsBloomed(false), mColor(Color::White),
      mOffset(Vector3::Zero), mScale(Vector3::One) {}

DrawComponent::~DrawComponent() {}
