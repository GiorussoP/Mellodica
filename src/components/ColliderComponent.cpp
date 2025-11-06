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
            // Return penetration for AABB (negate to push AABB away from sphere)
            return direction * penetrationDepth * -1.0f;
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
            // Push on X axis - negate to push AABB away from sphere
            return Vector3(diff.x >= 0.0f ? -dx : dx, 0.0f, 0.0f);
        }
        else if (dy < dz)
        {
            // Push on Y axis - negate to push AABB away from sphere
            return Vector3(0.0f, diff.y >= 0.0f ? -dy : dy, 0.0f);
        }
        else
        {
            // Push on Z axis - negate to push AABB away from sphere
            return Vector3(0.0f, 0.0f, diff.z >= 0.0f ? -dz : dz);
        }
    }
    else if (other.GetType() == ColliderType::OBB)
    {
        // OBB handles this, but we need to negate the result
        return other.DetectCollision(*this) * -1.0f;
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
        const OBBCollider& otherOBB = static_cast<const OBBCollider&>(other);
        
        Vector3 centerA = GetCenter();
        Vector3 centerB = otherOBB.GetCenter();
        
        // Get rotation matrices
        Matrix4 rotA = Matrix4::CreateFromQuaternion(mOwner->GetRotation());
        Matrix4 rotB = Matrix4::CreateFromQuaternion(otherOBB.mOwner->GetRotation());
        
        // Get axes for both OBBs
        Vector3 axesA[3] = {
            Vector3(rotA.mat[0][0], rotA.mat[0][1], rotA.mat[0][2]), // X axis
            Vector3(rotA.mat[1][0], rotA.mat[1][1], rotA.mat[1][2]), // Y axis
            Vector3(rotA.mat[2][0], rotA.mat[2][1], rotA.mat[2][2])  // Z axis
        };
        
        Vector3 axesB[3] = {
            Vector3(rotB.mat[0][0], rotB.mat[0][1], rotB.mat[0][2]),
            Vector3(rotB.mat[1][0], rotB.mat[1][1], rotB.mat[1][2]),
            Vector3(rotB.mat[2][0], rotB.mat[2][1], rotB.mat[2][2])
        };
        
        Vector3 extentsA = mSize;
        Vector3 extentsB = otherOBB.mSize;
        Vector3 centerDiff = centerB - centerA;
        
        // Test 15 axes (3 from A, 3 from B, 9 cross products)
        // For simplicity, we'll test the 6 main axes (good enough for most cases)
        
        // Test axes from OBB A
        for (int i = 0; i < 3; i++)
        {
            float ra = (i == 0) ? extentsA.x : (i == 1) ? extentsA.y : extentsA.z;
            float rb = Math::Abs(Vector3::Dot(axesB[0] * extentsB.x, axesA[i])) +
                      Math::Abs(Vector3::Dot(axesB[1] * extentsB.y, axesA[i])) +
                      Math::Abs(Vector3::Dot(axesB[2] * extentsB.z, axesA[i]));
            
            if (Math::Abs(Vector3::Dot(centerDiff, axesA[i])) > ra + rb)
                return false;
        }
        
        // Test axes from OBB B
        for (int i = 0; i < 3; i++)
        {
            float rb = (i == 0) ? extentsB.x : (i == 1) ? extentsB.y : extentsB.z;
            float ra = Math::Abs(Vector3::Dot(axesA[0] * extentsA.x, axesB[i])) +
                      Math::Abs(Vector3::Dot(axesA[1] * extentsA.y, axesB[i])) +
                      Math::Abs(Vector3::Dot(axesA[2] * extentsA.z, axesB[i]));
            
            if (Math::Abs(Vector3::Dot(centerDiff, axesB[i])) > ra + rb)
                return false;
        }
        
        return true;
    }
    else if (other.GetType() == ColliderType::AABB)
    {
        // OBB-AABB collision: transform AABB into OBB's local space
        const AABBCollider& aabb = static_cast<const AABBCollider&>(other);
        
        Vector3 obbCenter = GetCenter();
        Vector3 aabbCenter = aabb.GetMin() + aabb.GetSize();
        
        // Vector from OBB center to AABB center in world space
        Vector3 diff = aabbCenter - obbCenter;
        
        // Transform diff into OBB's local space (inverse rotation)
        Quaternion invRot = mOwner->GetRotation();
        invRot.Conjugate(); // Conjugate of unit quaternion is its inverse
        Vector3 localDiff = Vector3::Transform(diff, invRot);
        
        // In OBB's local space, this becomes an AABB-AABB test
        // OBB's local AABB: [-mSize, mSize]
        // Find closest point on OBB to AABB center in local space
        Vector3 closestLocal;
        closestLocal.x = Math::Max(-mSize.x, Math::Min(localDiff.x, mSize.x));
        closestLocal.y = Math::Max(-mSize.y, Math::Min(localDiff.y, mSize.y));
        closestLocal.z = Math::Max(-mSize.z, Math::Min(localDiff.z, mSize.z));
        
        // Check if this closest point is within the AABB's extents
        Vector3 closestWorld = Vector3::Transform(closestLocal, mOwner->GetRotation()) + obbCenter;
        
        Vector3 aabbMin = aabb.GetMin();
        Vector3 aabbMax = aabb.GetMax();
        
        // Check if closest point on OBB is inside or near the AABB
        if (closestWorld.x < aabbMin.x - 0.001f || closestWorld.x > aabbMax.x + 0.001f ||
            closestWorld.y < aabbMin.y - 0.001f || closestWorld.y > aabbMax.y + 0.001f ||
            closestWorld.z < aabbMin.z - 0.001f || closestWorld.z > aabbMax.z + 0.001f)
        {
            return false;
        }
        
        return true;
    }
    else if (other.GetType() == ColliderType::Sphere)
    {
        const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
        
        Vector3 obbCenter = GetCenter();
        Vector3 sphereCenter = sphere.GetCenter();
        
        // Transform sphere center into OBB's local space
        Vector3 diff = sphereCenter - obbCenter;
        Quaternion invRot = mOwner->GetRotation();
        invRot.Conjugate();
        Vector3 localSphereCenter = Vector3::Transform(diff, invRot);
        
        // Find closest point on OBB (in local space) to sphere center
        Vector3 closestLocal;
        closestLocal.x = Math::Max(-mSize.x, Math::Min(localSphereCenter.x, mSize.x));
        closestLocal.y = Math::Max(-mSize.y, Math::Min(localSphereCenter.y, mSize.y));
        closestLocal.z = Math::Max(-mSize.z, Math::Min(localSphereCenter.z, mSize.z));
        
        // Distance from sphere center to closest point on OBB
        float distSq = (localSphereCenter - closestLocal).LengthSq();
        float radius = sphere.GetRadius();
        
        return distSq <= (radius * radius);
    }
    
    return false;
}

Vector3 OBBCollider::DetectCollision(const ColliderComponent& other) const
{
    if (!Intersect(other))
    {
        return Vector3::Zero;
    }
    
    Vector3 obbCenter = GetCenter();
    Quaternion rotation = mOwner->GetRotation();
    Quaternion invRot = rotation;
    invRot.Conjugate();
    
    if (other.GetType() == ColliderType::OBB)
    {
        // OBB-OBB: find minimum penetration axis using SAT
        const OBBCollider& otherOBB = static_cast<const OBBCollider&>(other);
        Vector3 centerB = otherOBB.GetCenter();
        
        // Get rotation matrices
        Matrix4 rotA = Matrix4::CreateFromQuaternion(mOwner->GetRotation());
        Matrix4 rotB = Matrix4::CreateFromQuaternion(otherOBB.mOwner->GetRotation());
        
        // Get axes for both OBBs
        Vector3 axesA[3] = {
            Vector3(rotA.mat[0][0], rotA.mat[0][1], rotA.mat[0][2]),
            Vector3(rotA.mat[1][0], rotA.mat[1][1], rotA.mat[1][2]),
            Vector3(rotA.mat[2][0], rotA.mat[2][1], rotA.mat[2][2])
        };
        
        Vector3 axesB[3] = {
            Vector3(rotB.mat[0][0], rotB.mat[0][1], rotB.mat[0][2]),
            Vector3(rotB.mat[1][0], rotB.mat[1][1], rotB.mat[1][2]),
            Vector3(rotB.mat[2][0], rotB.mat[2][1], rotB.mat[2][2])
        };
        
        Vector3 extentsA = mSize;
        Vector3 extentsB = otherOBB.mSize;
        Vector3 centerDiff = centerB - obbCenter;
        
        float minPenetration = FLT_MAX;
        Vector3 minAxis;
        
        // Test axes from OBB A
        for (int i = 0; i < 3; i++)
        {
            float ra = (i == 0) ? extentsA.x : (i == 1) ? extentsA.y : extentsA.z;
            float rb = Math::Abs(Vector3::Dot(axesB[0] * extentsB.x, axesA[i])) +
                      Math::Abs(Vector3::Dot(axesB[1] * extentsB.y, axesA[i])) +
                      Math::Abs(Vector3::Dot(axesB[2] * extentsB.z, axesA[i]));
            
            float separation = Math::Abs(Vector3::Dot(centerDiff, axesA[i])) - (ra + rb);
            float penetration = -(separation);
            
            if (penetration < minPenetration && penetration > 0.0f)
            {
                minPenetration = penetration;
                minAxis = axesA[i];
                // Make sure axis points from B to A (push A away from B)
                if (Vector3::Dot(centerDiff, minAxis) > 0.0f)
                    minAxis = minAxis * -1.0f;
            }
        }
        
        // Test axes from OBB B
        for (int i = 0; i < 3; i++)
        {
            float rb = (i == 0) ? extentsB.x : (i == 1) ? extentsB.y : extentsB.z;
            float ra = Math::Abs(Vector3::Dot(axesA[0] * extentsA.x, axesB[i])) +
                      Math::Abs(Vector3::Dot(axesA[1] * extentsA.y, axesB[i])) +
                      Math::Abs(Vector3::Dot(axesA[2] * extentsA.z, axesB[i]));
            
            float separation = Math::Abs(Vector3::Dot(centerDiff, axesB[i])) - (ra + rb);
            float penetration = -(separation);
            
            if (penetration < minPenetration && penetration > 0.0f)
            {
                minPenetration = penetration;
                minAxis = axesB[i];
                // Make sure axis points from B to A (push A away from B)
                if (Vector3::Dot(centerDiff, minAxis) > 0.0f)
                    minAxis = minAxis * -1.0f;
            }
        }
        
        if (minPenetration < FLT_MAX)
        {
            // Return penetration for OBB A (pointing away from B, pushing A out)
            return minAxis * minPenetration;
        }
        
        return Vector3(0.0f, mSize.y + otherOBB.mSize.y, 0.0f);
    }
    else if (other.GetType() == ColliderType::AABB)
    {
        // OBB-AABB: transform AABB into OBB's local space
        const AABBCollider& aabb = static_cast<const AABBCollider&>(other);
        Vector3 aabbCenter = aabb.GetMin() + aabb.GetSize();
        
        // Transform AABB center into OBB's local space
        Vector3 diff = aabbCenter - obbCenter;
        Vector3 localAABBCenter = Vector3::Transform(diff, invRot);
        
        // Find closest point on OBB to AABB center (in local space)
        Vector3 closestLocal;
        closestLocal.x = Math::Max(-mSize.x, Math::Min(localAABBCenter.x, mSize.x));
        closestLocal.y = Math::Max(-mSize.y, Math::Min(localAABBCenter.y, mSize.y));
        closestLocal.z = Math::Max(-mSize.z, Math::Min(localAABBCenter.z, mSize.z));
        
        // Calculate penetration in local space (from OBB toward AABB - then negate for OBB's perspective)
        Vector3 localDirection = localAABBCenter - closestLocal;
        float dist = localDirection.Length();
        
        // If AABB center is inside OBB, push OBB out on smallest axis
        bool insideX = Math::Abs(localAABBCenter.x) < mSize.x;
        bool insideY = Math::Abs(localAABBCenter.y) < mSize.y;
        bool insideZ = Math::Abs(localAABBCenter.z) < mSize.z;
        
        Vector3 localPenetration;
        
        if (insideX && insideY && insideZ)
        {
            // AABB center is inside OBB - push OBB out on smallest penetration axis
            float dx = mSize.x - Math::Abs(localAABBCenter.x);
            float dy = mSize.y - Math::Abs(localAABBCenter.y);
            float dz = mSize.z - Math::Abs(localAABBCenter.z);
            
            if (dx < dy && dx < dz)
            {
                // Push OBB away from AABB center (negate the sign)
                localPenetration = Vector3(localAABBCenter.x >= 0.0f ? -dx : dx, 0.0f, 0.0f);
            }
            else if (dy < dz)
            {
                localPenetration = Vector3(0.0f, localAABBCenter.y >= 0.0f ? -dy : dy, 0.0f);
            }
            else
            {
                localPenetration = Vector3(0.0f, 0.0f, localAABBCenter.z >= 0.0f ? -dz : dz);
            }
        }
        else if (dist > 0.0f)
        {
            // AABB center is outside OBB
            localDirection.Normalize();
            // For simplicity, use a small penetration depth
            // This case should rarely happen if Intersect is working correctly
            localPenetration = localDirection * -0.01f;
        }
        else
        {
            // Default case
            localPenetration = Vector3(0.0f, -0.01f, 0.0f);
        }
        
        // Transform penetration back to world space
        Vector3 worldPenetration = Vector3::Transform(localPenetration, rotation);
        return worldPenetration;
    }
    else if (other.GetType() == ColliderType::Sphere)
    {
        // OBB-Sphere: find closest point on OBB to sphere center
        const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
        Vector3 sphereCenter = sphere.GetCenter();
        
        // Transform sphere center into OBB's local space
        Vector3 diff = sphereCenter - obbCenter;
        Vector3 localSphereCenter = Vector3::Transform(diff, invRot);
        
        // Find closest point on OBB to sphere center (in local space)
        Vector3 closestLocal;
        closestLocal.x = Math::Max(-mSize.x, Math::Min(localSphereCenter.x, mSize.x));
        closestLocal.y = Math::Max(-mSize.y, Math::Min(localSphereCenter.y, mSize.y));
        closestLocal.z = Math::Max(-mSize.z, Math::Min(localSphereCenter.z, mSize.z));
        
        // Calculate penetration in local space (negate to push OBB away from sphere)
        Vector3 localDirection = localSphereCenter - closestLocal;
        float dist = localDirection.Length();
        
        if (dist > 0.0f)
        {
            localDirection.Normalize();
            float penetrationDepth = sphere.GetRadius() - dist;
            // Negate direction to push OBB away from sphere
            Vector3 localPenetration = localDirection * penetrationDepth * -1.0f;
            
            // Transform back to world space
            return Vector3::Transform(localPenetration, rotation);
        }
        
        // Sphere center is exactly on OBB surface - push OBB away along negative Y
        return Vector3::Transform(Vector3(0.0f, -sphere.GetRadius(), 0.0f), rotation);
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
        // AABB returns penetration for itself (to push AABB away from sphere)
        // Negate it to get penetration for sphere (to push sphere away from AABB)
        return other.DetectCollision(*this) * -1.0f;
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
