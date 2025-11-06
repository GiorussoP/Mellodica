#include "components/ColliderComponent.hpp"
#include "actors/Actor.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"
#include "render/Mesh.hpp"
#include "Math.hpp"

// ============================================================================
// ColliderComponent Base Class
// ============================================================================

ColliderComponent::ColliderComponent(Actor* owner, ColliderLayer layer, ColliderType type, bool isStatic, int updateOrder)
    : Component(owner, updateOrder)
    , mLayer(layer)
    , mType(type)
    , mIsStatic(isStatic)
{
}

ColliderComponent::~ColliderComponent()
{
}


// ============================================================================
// AABBCollider (Axis-Aligned Bounding Box)
// ============================================================================

AABBCollider::AABBCollider(Actor* owner, ColliderLayer layer, Vector3 offset, Vector3 size, bool isStatic, int updateOrder)
    : ColliderComponent(owner, layer, ColliderType::AABB, isStatic, updateOrder)
    , mOffset(offset)
    , mSize(size)
{
}

AABBCollider::~AABBCollider()
{
}

Vector3 AABBCollider::GetMin() const
{
    return mOwner->GetPosition() + mOffset - mSize;
}

Vector3 AABBCollider::GetMax() const
{
    return mOwner->GetPosition() + mOffset + mSize;
}

bool AABBCollider::Intersect(const ColliderComponent& other) const
{
    if (other.GetType() == ColliderType::AABB)
    {
        const AABBCollider& otherAABB = static_cast<const AABBCollider&>(other);
        
        Vector3 aMin = GetMin();
        Vector3 aMax = GetMax();
        Vector3 bMin = otherAABB.GetMin();
        Vector3 bMax = otherAABB.GetMax();
        
        // AABB vs AABB intersection test
        bool noOverlap = aMin.x > bMax.x || 
                        bMin.x > aMax.x || 
                        aMin.y > bMax.y || 
                        bMin.y > aMax.y || 
                        aMin.z > bMax.z || 
                        bMin.z > aMax.z;
        
        return !noOverlap;
    }
    else if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
        
        // Find the closest point on the AABB to the sphere center
        Vector3 sphereCenter = sphere.GetCenter();
        Vector3 closestPoint;
        
        Vector3 aMin = GetMin();
        Vector3 aMax = GetMax();
        
        closestPoint.x = Math::Max(aMin.x, Math::Min(sphereCenter.x, aMax.x));
        closestPoint.y = Math::Max(aMin.y, Math::Min(sphereCenter.y, aMax.y));
        closestPoint.z = Math::Max(aMin.z, Math::Min(sphereCenter.z, aMax.z));
        
        // Check if the distance is less than the sphere radius
        float distSq = (closestPoint - sphereCenter).LengthSq();
        float radius = sphere.GetRadius();
        
        return distSq <= (radius * radius);
    }
    else if (other.GetType() == ColliderType::OBB)
    {
        // OBB vs AABB - let the OBB handle it
        return other.Intersect(*this);
    }
    
    return false;
}

Vector3 AABBCollider::DetectCollision(const ColliderComponent& other) const
{
    if (!Intersect(other))
    {
        return Vector3::Zero;
    }
    
    if (other.GetType() == ColliderType::AABB)
    {
        const AABBCollider& otherAABB = static_cast<const AABBCollider&>(other);
        
        Vector3 aMin = GetMin();
        Vector3 aMax = GetMax();
        Vector3 bMin = otherAABB.GetMin();
        Vector3 bMax = otherAABB.GetMax();
        
        // Calculate overlap on each axis
        float overlapX1 = aMax.x - bMin.x;
        float overlapX2 = bMax.x - aMin.x;
        float overlapY1 = aMax.y - bMin.y;
        float overlapY2 = bMax.y - aMin.y;
        float overlapZ1 = aMax.z - bMin.z;
        float overlapZ2 = bMax.z - aMin.z;
        
        float overlapX = Math::Min(overlapX1, overlapX2);
        float overlapY = Math::Min(overlapY1, overlapY2);
        float overlapZ = Math::Min(overlapZ1, overlapZ2);
        
        // Find the axis with minimum penetration
        Vector3 penetration = Vector3::Zero;
        
        if (overlapX < overlapY && overlapX < overlapZ)
        {
            // Resolve on X axis
            penetration.x = (aMax.x + aMin.x < bMax.x + bMin.x) ? -overlapX : overlapX;
        }
        else if (overlapY < overlapZ)
        {
            // Resolve on Y axis
            penetration.y = (aMax.y + aMin.y < bMax.y + bMin.y) ? -overlapY : overlapY;
        }
        else
        {
            // Resolve on Z axis
            penetration.z = (aMax.z + aMin.z < bMax.z + bMin.z) ? -overlapZ : overlapZ;
        }
        
        return penetration;
    }
    else if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
        
        Vector3 sphereCenter = sphere.GetCenter();
        Vector3 closestPoint;
        
        Vector3 aMin = GetMin();
        Vector3 aMax = GetMax();
        
        closestPoint.x = Math::Max(aMin.x, Math::Min(sphereCenter.x, aMax.x));
        closestPoint.y = Math::Max(aMin.y, Math::Min(sphereCenter.y, aMax.y));
        closestPoint.z = Math::Max(aMin.z, Math::Min(sphereCenter.z, aMax.z));
        
        Vector3 direction = sphereCenter - closestPoint;
        float dist = direction.Length();
        
        if (dist > 0.0f)
        {
            direction.Normalize();
            float penetrationDepth = sphere.GetRadius() - dist;
            return direction * penetrationDepth;
        }
        
        // Sphere center is inside the box - push out on smallest axis
        Vector3 center = (aMin + aMax) * 0.5f;
        Vector3 diff = sphereCenter - center;
        Vector3 halfSize = (aMax - aMin) * 0.5f;
        
        // Calculate penetration depth on each axis
        float dx = halfSize.x + sphere.GetRadius() - Math::Abs(diff.x);
        float dy = halfSize.y + sphere.GetRadius() - Math::Abs(diff.y);
        float dz = halfSize.z + sphere.GetRadius() - Math::Abs(diff.z);
        
        // Push out on the axis with smallest penetration
        if (dx < dy && dx < dz)
        {
            // Push on X axis - use sign of diff.x to determine direction
            return Vector3(diff.x >= 0.0f ? dx : -dx, 0.0f, 0.0f);
        }
        else if (dy < dz)
        {
            // Push on Y axis - use sign of diff.y to determine direction
            return Vector3(0.0f, diff.y >= 0.0f ? dy : -dy, 0.0f);
        }
        else
        {
            // Push on Z axis - use sign of diff.z to determine direction
            return Vector3(0.0f, 0.0f, diff.z >= 0.0f ? dz : -dz);
        }
    }
    
    return Vector3::Zero;
}

void AABBCollider::DebugDraw(class Renderer* renderer)
{
    // Get the cube mesh from the renderer
    Mesh* cubeMesh = renderer->LoadMesh("cube");
    if (!cubeMesh)
    {
        return;
    }
    
    // Calculate center and scale
    Vector3 center = mOwner->GetPosition() + mOffset;
    Vector3 scale = mSize * 2.0f; // mSize is half-extents, so multiply by 2 for full size
    
    // Draw with no rotation (AABB is axis-aligned)
    renderer->DrawSingleMesh(cubeMesh, center, scale);
}

// ============================================================================
// OBBCollider (Oriented Bounding Box)
// ============================================================================

OBBCollider::OBBCollider(Actor* owner, ColliderLayer layer, Vector3 offset, Vector3 size, bool isStatic, int updateOrder)
    : ColliderComponent(owner, layer, ColliderType::OBB, isStatic, updateOrder)
    , mOffset(offset)
    , mSize(size)
{
}

OBBCollider::~OBBCollider()
{
}

Vector3 OBBCollider::GetCenter() const
{
    // Transform offset by rotation
    Vector3 rotatedOffset = Vector3::Transform(mOffset, mOwner->GetRotation());
    return mOwner->GetPosition() + rotatedOffset;
}

bool OBBCollider::Intersect(const ColliderComponent& other) const
{
    if (other.GetType() == ColliderType::OBB)
    {
        // OBB vs OBB using Separating Axis Theorem (SAT)
        // This is a simplified version - full SAT implementation would test all 15 axes
        const OBBCollider& otherOBB = static_cast<const OBBCollider&>(other);
        
        // For simplicity, we'll use a conservative bounding sphere test
        Vector3 centerA = GetCenter();
        Vector3 centerB = otherOBB.GetCenter();
        
        float radiusA = mSize.Length();
        float radiusB = otherOBB.mSize.Length();
        
        float distSq = (centerA - centerB).LengthSq();
        float radiusSum = radiusA + radiusB;
        
        return distSq <= (radiusSum * radiusSum);
    }
    else if (other.GetType() == ColliderType::AABB)
    {
        // Convert AABB to OBB for testing (AABB is just an OBB with no rotation)
        const AABBCollider& aabb = static_cast<const AABBCollider&>(other);
        
        Vector3 centerA = GetCenter();
        Vector3 centerB = aabb.GetMin() + aabb.GetSize();
        
        float radiusA = mSize.Length();
        float radiusB = aabb.GetSize().Length();
        
        float distSq = (centerA - centerB).LengthSq();
        float radiusSum = radiusA + radiusB;
        
        return distSq <= (radiusSum * radiusSum);
    }
    else if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
        
        // Simplified: treat OBB as a bounding sphere
        Vector3 centerA = GetCenter();
        Vector3 centerB = sphere.GetCenter();
        
        float radiusA = mSize.Length();
        float radiusB = sphere.GetRadius();
        
        float distSq = (centerA - centerB).LengthSq();
        float radiusSum = radiusA + radiusB;
        
        return distSq <= (radiusSum * radiusSum);
    }
    
    return false;
}

Vector3 OBBCollider::DetectCollision(const ColliderComponent& other) const
{
    if (!Intersect(other))
    {
        return Vector3::Zero;
    }
    
    // Simplified collision response - push apart based on center distance
    Vector3 centerA = GetCenter();
    Vector3 centerB;
    
    if (other.GetType() == ColliderType::OBB)
    {
        const OBBCollider& otherOBB = static_cast<const OBBCollider&>(other);
        centerB = otherOBB.GetCenter();
    }
    else if (other.GetType() == ColliderType::AABB)
    {
        const AABBCollider& aabb = static_cast<const AABBCollider&>(other);
        centerB = aabb.GetMin() + aabb.GetSize();
    }
    else if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
        centerB = sphere.GetCenter();
    }
    
    Vector3 direction = centerA - centerB;
    float dist = direction.Length();
    
    if (dist > 0.0f)
    {
        direction.Normalize();
        
        // Estimate penetration depth
        float radiusA = mSize.Length();
        float radiusB = 0.0f;
        
        if (other.GetType() == ColliderType::OBB)
        {
            radiusB = static_cast<const OBBCollider&>(other).mSize.Length();
        }
        else if (other.GetType() == ColliderType::AABB)
        {
            radiusB = static_cast<const AABBCollider&>(other).GetSize().Length();
        }
        else if (other.GetType() == ColliderType::Sphere)
        {
            radiusB = static_cast<const SphereCollider&>(other).GetRadius();
        }
        
        float penetrationDepth = radiusA + radiusB - dist;
        return direction * penetrationDepth;
    }
    
    return Vector3::Zero;
}

void OBBCollider::DebugDraw(class Renderer* renderer)
{
    // Get the cube mesh from the renderer
    Mesh* cubeMesh = renderer->LoadMesh("cube");
    if (!cubeMesh)
    {
        return;
    }
    
    Vector3 center = GetCenter();
    Vector3 scale = mSize * 2.0f;
    Quaternion rotation = mOwner->GetRotation();
    
    renderer->DrawSingleMesh(cubeMesh, center, scale, rotation);
}

// ============================================================================
// SphereCollider
// ============================================================================

SphereCollider::SphereCollider(Actor* owner, ColliderLayer layer, Vector3 offset, float radius, bool isStatic, int updateOrder)
    : ColliderComponent(owner, layer, ColliderType::Sphere, isStatic, updateOrder)
    , mOffset(offset)
    , mRadius(radius)
{
}

SphereCollider::~SphereCollider()
{
}

Vector3 SphereCollider::GetCenter() const
{
    // Transform offset by rotation (for consistency with OBB)
    Vector3 rotatedOffset = Vector3::Transform(mOffset, mOwner->GetRotation());
    return mOwner->GetPosition() + rotatedOffset;
}

bool SphereCollider::Intersect(const ColliderComponent& other) const
{
    if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& otherSphere = static_cast<const SphereCollider&>(other);
        
        Vector3 centerA = GetCenter();
        Vector3 centerB = otherSphere.GetCenter();
        
        float radiusSum = mRadius + otherSphere.mRadius;
        float distSq = (centerA - centerB).LengthSq();
        
        return distSq <= (radiusSum * radiusSum);
    }
    else if (other.GetType() == ColliderType::AABB)
    {
        // AABB handles this case
        return other.Intersect(*this);
    }
    else if (other.GetType() == ColliderType::OBB)
    {
        // OBB handles this case
        return other.Intersect(*this);
    }
    
    return false;
}

Vector3 SphereCollider::DetectCollision(const ColliderComponent& other) const
{
    if (!Intersect(other))
    {
        return Vector3::Zero;
    }
    
    if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& otherSphere = static_cast<const SphereCollider&>(other);
        
        Vector3 centerA = GetCenter();
        Vector3 centerB = otherSphere.GetCenter();
        
        Vector3 direction = centerA - centerB;
        float dist = direction.Length();
        
        if (dist > 0.0f)
        {
            direction.Normalize();
            float penetrationDepth = mRadius + otherSphere.mRadius - dist;
            return direction * penetrationDepth;
        }
        
        // Centers are at same position - push up by default
        return Vector3(0.0f, mRadius + otherSphere.mRadius, 0.0f);
    }
    else if (other.GetType() == ColliderType::AABB)
    {
        // AABB returns penetration from sphere's perspective, so use it directly
        return other.DetectCollision(*this);
    }
    else if (other.GetType() == ColliderType::OBB)
    {
        // OBB handles this, but we need to negate the result
        return other.DetectCollision(*this) * -1.0f;
    }
    
    return Vector3::Zero;
}

void SphereCollider::DebugDraw(Renderer* renderer)
{
    // Get the sphere mesh from the renderer
    Mesh* sphereMesh = renderer->LoadMesh("sphere");
    if (!sphereMesh)
    {
        return;
    }
    
    // Calculate center and scale
    Vector3 center = GetCenter();
    float diameter = mRadius * 2.0f; // mRadius is radius, so multiply by 2 for diameter
    Vector3 scale(diameter, diameter, diameter);
    
    // Draw sphere (no rotation needed for a sphere)
    renderer->DrawSingleMesh(sphereMesh, center, scale);
}
