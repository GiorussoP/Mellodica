#ifndef COLLIDER_COMPONENT_HPP
#define COLLIDER_COMPONENT_HPP

#include "Component.hpp"
#include "Math.hpp"


enum class ColliderType {
    AABB,
    OBB,
    Sphere
};

enum class ColliderLayer {
    Player,
    Ground,
    Entity,
    Enemy,
    Note
};

// Collider component interface
class ColliderComponent : public Component {
public:
    ColliderComponent(Actor* owner, ColliderLayer layer, ColliderType type = ColliderType::AABB, bool isStatic = false, int updateOrder = 10);
    virtual ~ColliderComponent();

    // Check if two colliders intersect (virtual to allow different implementations)
    virtual bool Intersect(const ColliderComponent& other) const = 0;

    // Detect collision and return penetration vector (virtual to allow different implementations)
    virtual Vector3 DetectCollision(const ColliderComponent& other) const = 0;

    // Debug draw the collider bounds (virtual to allow different implementations)
    virtual void DebugDraw(class Renderer* renderer) override = 0;

    ColliderLayer GetLayer() const { return mLayer; }
    ColliderType GetType() const { return mType; }
    bool IsStatic() const { return mIsStatic; }

protected:
    ColliderLayer mLayer;
    ColliderType mType;
    bool mIsStatic;
};


// Axis-Aligned Bounding Box collider
class AABBCollider : public ColliderComponent {
public:
    AABBCollider(Actor* owner, ColliderLayer layer, Vector3 offset, Vector3 size, bool isStatic = true, int updateOrder = 10);
    ~AABBCollider();

    bool Intersect(const ColliderComponent& other) const override;
    Vector3 DetectCollision(const ColliderComponent& other) const override;
    void DebugDraw(class Renderer* renderer) override;

    Vector3 GetOffset() const { return mOffset; }
    Vector3 GetSize() const { return mSize; }
    
    // Get world-space min/max points
    Vector3 GetMin() const;
    Vector3 GetMax() const;

private:
    Vector3 mOffset;  // Offset from actor position
    Vector3 mSize;    // Half-extents (width/2, height/2, depth/2)
};


// Oriented Bounding Box collider
class OBBCollider : public ColliderComponent {
public:
    OBBCollider(Actor* owner, ColliderLayer layer, Vector3 offset, Vector3 size, bool isStatic = false, int updateOrder = 10);
    ~OBBCollider();

    bool Intersect(const ColliderComponent& other) const override;
    Vector3 DetectCollision(const ColliderComponent& other) const override;
    void DebugDraw(class Renderer* renderer) override;

    Vector3 GetOffset() const { return mOffset; }
    Vector3 GetSize() const { return mSize; }
    
    // Get world-space center
    Vector3 GetCenter() const;

private:
    Vector3 mOffset;  // Offset from actor position
    Vector3 mSize;    // Half-extents (width/2, height/2, depth/2)
};


// Sphere collider
class SphereCollider : public ColliderComponent {
public:
    SphereCollider(Actor* owner, ColliderLayer layer, Vector3 offset, float radius, bool isStatic = false, int updateOrder = 10);
    ~SphereCollider();

    bool Intersect(const ColliderComponent& other) const override;
    Vector3 DetectCollision(const ColliderComponent& other) const override;
    void DebugDraw(class Renderer* renderer) override;

    Vector3 GetOffset() const { return mOffset; }
    float GetRadius() const { return mRadius; }
    
    // Get world-space center
    Vector3 GetCenter() const;

private:
    Vector3 mOffset;  // Offset from actor position
    float mRadius;    // Sphere radius
};

// 

#endif