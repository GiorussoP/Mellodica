#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include "components/Component.hpp"
#include "actors/Actor.hpp"
#include "Math.hpp"

class RigidBodyComponent : public Component {

    public:
        RigidBodyComponent(class Actor* owner, float mass = 1.0f, float friction = 0.0f, bool applyGravity = false, int updateOrder = 10);
        virtual ~RigidBodyComponent();

        void Update(float deltaTime) override;

        void ApplyForce(const Vector3& force);
        void ApplyImpulse(const Vector3& impulse);

        void SetMass(float mass) { mMass = mass; }
        float GetMass() const { return mMass; }

        Vector3 GetVelocity() const { return mVelocity; }
        void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }

    private:
        bool mApplyGravity;

        float mMass;
        float mFriction;

        Vector3 mVelocity;
        Vector3 mAcceleration;
};





#endif
