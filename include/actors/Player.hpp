#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Actor.hpp"
#include "components/RigidBodyComponent.hpp"
#include "components/ColliderComponent.hpp"
#include "components/SpriteComponent.hpp"


class Player : public Actor {
public:
    Player(class Game* game);
    void OnUpdate(float deltaTime) override;
    void OnProcessInput(const Uint8* keyState) override;
    void OnCollision(Vector3 penetration, ColliderComponent* other) override;
    
private:
    float mMoveSpeed;
    float mTurnSpeed;
    float mCameraSpeed;
    float mCameraTurnSpeed;

    bool mMoveForward;
    bool mMoveBackward;
    bool mMoveLeft;
    bool mMoveRight;
    bool mRotateLeft;
    bool mRotateRight;
    bool mRotateUp;
    bool mRotateDown;

    float mCameraYaw;
    float mCameraPitch;


    RigidBodyComponent* mRigidBodyComponent;
    ColliderComponent* mColliderComponent;
    SpriteComponent* mSpriteComponent;
};




#endif