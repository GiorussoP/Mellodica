#include "RigidBodyComponent.hpp"
#include "ColliderComponent.hpp"

const float GRAVITY = 2500.0f;
const float MAX_SPEED = 750.0f;

RigidBodyComponent::RigidBodyComponent(Actor *owner, float mass, float friction,
                                       bool applyGravity, int updateOrder)
    : Component(owner, updateOrder), mMass(mass), mApplyGravity(applyGravity),
      mFriction(friction), mVelocity(Vector3::Zero),
      mAcceleration(Vector3::Zero) {}

RigidBodyComponent::~RigidBodyComponent() {}

void RigidBodyComponent::ApplyForce(const Vector3 &force) {
  mAcceleration += force * (1.0f / mMass);
  ;
}

void RigidBodyComponent::ApplyImpulse(const Vector3 &impulse) {
  mVelocity += impulse * (1.0f / mMass);
}

void RigidBodyComponent::Update(float deltaTime) {
  // Apply gravity if enabled
  if (mApplyGravity) {
    mAcceleration += Vector3(0.0f, -1.0f, 0.0f) * GRAVITY;
  }

  if (mFriction != 0.0f && mVelocity.LengthSq() > 0.01f * 0.01f) {
    ApplyForce(Vector3::Normalize(mVelocity) * -mFriction);
  }

  // Update velocity, euler integration
  mVelocity += mAcceleration * deltaTime;

  // Clamp velocity to max speed
  if (mVelocity.LengthSq() > MAX_SPEED * MAX_SPEED) {
    mVelocity = Vector3::Normalize(mVelocity) * MAX_SPEED;
  }

  if (mVelocity.LengthSq() < 0.001f) {
    mVelocity = Vector3::Zero;
  }

  // Update owner actor's position
  mOwner->SetPosition(mOwner->GetPosition() +
                      mVelocity.ProjectedOnPlane(Vector3::UnitY) * deltaTime);

  // Reset acceleration for next frame
  mAcceleration = Vector3::Zero;
}