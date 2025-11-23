#include "Camera.hpp"

#include "Game.hpp"
#include "Renderer.hpp"

Camera::Camera(class Game *game, const Vector3 &eye, const Quaternion rotation, float speed)
    : mGame(game)
    , mPosition(eye)
    , mRotation(rotation)
    , mMode(CameraMode::Fixed)
    , mSpeed(speed)
{
    mGame->GetRenderer()->SetViewMatrix(GetCameraMatrix());

}

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

void Camera::SetIsometricDirection(const IsometricDirections direction) {
    switch (direction) {
        case IsometricDirections::North:
            isometricIndex = 0;
            break;
        case IsometricDirections::NorthEast:
            isometricIndex = 1;
            break;
        case IsometricDirections::East:
            isometricIndex = 2;
            break;
        case IsometricDirections::SouthEast:
            isometricIndex = 3;
            break;
        case IsometricDirections::South:
            isometricIndex = 4;
            break;
        case IsometricDirections::SouthWest:
            isometricIndex = 5;
            break;
        case IsometricDirections::West:
            isometricIndex = 6;
            break;
        case IsometricDirections::NorthWest:
            isometricIndex = 7;
            break;
    }
}


Matrix4 Camera::GetCameraMatrix() const {
    const auto mCameraForward = Vector3::Transform(Vector3::UnitZ, mRotation);
    const auto mCameraUp = Vector3::Transform(Vector3::UnitY, mRotation);
    return Matrix4::CreateLookAt(mPosition, mPosition + mCameraForward, mCameraUp);
}

void Camera::Update(float deltaTime) {
    switch (mMode) {
        case CameraMode::Fixed:
            break;
        case CameraMode::Isometric:
            SetPosition(Vector3::Lerp(GetPosition(), mTargetPosition, mSpeed * deltaTime));
            SetRotation(Quaternion::Lerp(GetRotation(), ISOMETRIC_DIRECTIONS[isometricIndex], mSpeed * deltaTime));
            break;
        case CameraMode::Following:
            SetPosition(Vector3::Lerp(GetPosition(), mTargetPosition, mSpeed * deltaTime));
            SetRotation(Quaternion::Lerp(GetRotation(), mTargetRotation, mSpeed * deltaTime));
            break;
    }
    mGame->GetRenderer()->SetViewMatrix(GetCameraMatrix());
}
