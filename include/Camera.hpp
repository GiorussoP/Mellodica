#pragma once
#include "Actor.hpp"
#include "Math.hpp"


enum class CameraMode {
    Isometric, // Camera is following a target position, with isometric direction
    Fixed, // Camera is fixed at a spot
    Following, // Camera follows a target position and a target rotation
};

enum class IsometricDirections {
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest
};

constexpr float CAMERA_MOVE_SPEED = 2.0f;
constexpr float CAMERA_TURN_SPEED = 2.0f;

class Camera {
public:
    Camera(class Game *game, const Vector3 &eye, const Quaternion rotation = Quaternion::Identity, float moveSpeed = CAMERA_MOVE_SPEED, float turnSpeed = CAMERA_TURN_SPEED);
    void Update(float deltaTime);

    // Get/set position
    Vector3 GetPosition() const { return mPosition; }
    void SetPosition(const Vector3& pos) { mPosition = pos; }

    Quaternion GetRotation() const { return mRotation; }
    void SetRotation (const Quaternion& rotation) { mRotation = rotation; }

    void SetTargetPosition(const Vector3& pos) {mTargetPosition = pos; }
    Vector3 GetTargetPosition() const { return mTargetPosition; }

    Quaternion GetTargetRotation() const { return mTargetRotation; }
    void SetTargetRotation(const Quaternion& rotation) { mTargetRotation = rotation; }

    void SetIsometricDirection(const IsometricDirections direction);

    // Get mode
    CameraMode GetMode() const { return mMode; }
    void SetMode(const CameraMode mode) { mMode = mode; }

    // legacy methods: (not using rotations)
    void SetCameraForward(const Vector3& forward);
    void SetCameraUp(const Vector3& up);
    Vector3 GetCameraForward() const {
        return Vector3::Transform(Vector3::UnitZ, mRotation);
    }
    Vector3 GetCameraUp() const {
        return Vector3::Transform(Vector3::UnitY, mRotation);
    }


private:
    // Game
    class Game* mGame;

    Vector3 mPosition;
    Quaternion mRotation;

    Vector3 mTargetPosition;
    Quaternion mTargetRotation;

    CameraMode mMode;

    float mMoveSpeed;
    float mTurnSpeed;

    static Quaternion ISOMETRIC_DIRECTIONS[8];
    int isometricIndex;

    Matrix4 GetCameraMatrix() const;
};

