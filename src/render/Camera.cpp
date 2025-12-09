#include "render/Camera.hpp"

#include "Game.hpp"
#include "render/Renderer.hpp"

Camera::Camera(class Game *game, const Vector3 &eye, const Quaternion rotation,
               float moveSpeed, float turnSpeed)
    : mGame(game), mPosition(eye), mRotation(rotation),
      mMode(CameraMode::Fixed), mMoveSpeed(moveSpeed), mTurnSpeed(turnSpeed),
      mIsometricDirection(IsometricDirections::North) {}

// All directions normalized with the same Y component for consistent tilt
// Y = -0.42262 gives approximately 25-degree downward angle
// NOTE: This must be in the same order as the IsometricDirections enum class in
// the header file
Quaternion Camera::ISOMETRIC_DIRECTIONS[8] = {
    Math::LookRotation(Vector3::Normalize(Vector3(0.0f, -0.42262f, 0.90631f)),
                       Vector3::UnitY), // N
    Math::LookRotation(
        Vector3::Normalize(Vector3(0.64085f, -0.42262f, 0.64085f)),
        Vector3::UnitY), // NE
    Math::LookRotation(Vector3::Normalize(Vector3(0.90631f, -0.42262f, 0.0f)),
                       Vector3::UnitY), // E
    Math::LookRotation(
        Vector3::Normalize(Vector3(0.64085f, -0.42262f, -0.64085f)),
        Vector3::UnitY), // SE
    Math::LookRotation(Vector3::Normalize(Vector3(0.0f, -0.42262f, -0.90631f)),
                       Vector3::UnitY), // S
    Math::LookRotation(
        Vector3::Normalize(Vector3(-0.64085f, -0.42262f, -0.64085f)),
        Vector3::UnitY), // SW
    Math::LookRotation(Vector3::Normalize(Vector3(-0.90631f, -0.42262f, 0.0f)),
                       Vector3::UnitY), // W
    Math::LookRotation(
        Vector3::Normalize(Vector3(-0.64085f, -0.42262f, 0.64085f)),
        Vector3::UnitY) // NW
};

Matrix4 Camera::GetCameraMatrix() const {
  const auto mCameraForward = Vector3::Transform(Vector3::UnitZ, mRotation);
  const auto mCameraUp = Vector3::Transform(Vector3::UnitY, mRotation);
  return Matrix4::CreateLookAt(mPosition, mPosition + mCameraForward,
                               mCameraUp);
}

void Camera::SetCameraForward(const Vector3 &forward) {
  const auto cameraUp = Vector3::Transform(Vector3::UnitY, mRotation);
  mRotation = Math::LookRotation(forward, cameraUp);
}

void Camera::SetCameraUp(const Vector3 &up) {
  const auto cameraForward = Vector3::Transform(Vector3::UnitZ, mRotation);
  mRotation = Math::LookRotation(up, cameraForward);
}

void Camera::Update(float deltaTime) {
  switch (mMode) {
  case CameraMode::Fixed:
    break;
  case CameraMode::Isometric:
    if (mTargetPosition.LengthSq() > 0.001f &&
        !std::isnan(mTargetPosition.x)) {
      SetPosition(Vector3::Lerp(GetPosition(), mTargetPosition,
                                mMoveSpeed * deltaTime));
    }
    SetRotation(Quaternion::Slerp(
        GetRotation(),
        ISOMETRIC_DIRECTIONS[static_cast<int>(mIsometricDirection)],
        mTurnSpeed * deltaTime));
    break;
  case CameraMode::Following:
    if (mTargetPosition.LengthSq() > 0.001f &&
        !std::isnan(mTargetPosition.x)) {
      SetPosition(Vector3::Lerp(GetPosition(), mTargetPosition,
                                mMoveSpeed * deltaTime));
    }
    SetRotation(Quaternion::Slerp(GetRotation(), mTargetRotation,
                                  mTurnSpeed * deltaTime));
    break;
  }
  mGame->GetRenderer()->SetViewMatrix(GetCameraMatrix());
}
