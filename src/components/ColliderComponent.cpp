#include "components/ColliderComponent.hpp"
#include "Game.hpp"
#include "Math.hpp"
#include "actors/Actor.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"

// ============================================================================
// ColliderComponent Base Class
// ============================================================================

ColliderComponent::ColliderComponent(Actor *owner, ColliderLayer layer,
                                     ColliderType type, bool isStatic,
                                     int updateOrder)
    : Component(owner, updateOrder), mLayer(layer), mType(type),
      mIsStatic(isStatic) {}

ColliderComponent::~ColliderComponent() {}

// ============================================================================
// AABBCollider (Axis-Aligned Bounding Box)
// ============================================================================

AABBCollider::AABBCollider(Actor *owner, ColliderLayer layer, Vector3 offset,
                           Vector3 size, bool isStatic, int updateOrder)
    : ColliderComponent(owner, layer, ColliderType::AABB, isStatic,
                        updateOrder),
      mOffset(offset), mSize(size) {}

AABBCollider::~AABBCollider() {}

Vector3 AABBCollider::GetMin() const {
  Vector3 scale = mOwner->GetScale();
  // Apply scale to both offset and size (size is half-extents)
  Vector3 scaledOffset =
      Vector3(mOffset.x * scale.x, mOffset.y * scale.y, mOffset.z * scale.z);
  Vector3 scaledSize =
      Vector3(mSize.x * scale.x, mSize.y * scale.y, mSize.z * scale.z);
  return mOwner->GetPosition() + scaledOffset - scaledSize;
}

Vector3 AABBCollider::GetMax() const {
  Vector3 scale = mOwner->GetScale();
  // Apply scale to both offset and size (size is half-extents)
  Vector3 scaledOffset =
      Vector3(mOffset.x * scale.x, mOffset.y * scale.y, mOffset.z * scale.z);
  Vector3 scaledSize =
      Vector3(mSize.x * scale.x, mSize.y * scale.y, mSize.z * scale.z);
  return mOwner->GetPosition() + scaledOffset + scaledSize;
}

bool AABBCollider::Intersect(const ColliderComponent &other) const {
  if (other.GetType() == ColliderType::AABB) {
    const AABBCollider &otherAABB = static_cast<const AABBCollider &>(other);

    Vector3 aMin = GetMin();
    Vector3 aMax = GetMax();
    Vector3 bMin = otherAABB.GetMin();
    Vector3 bMax = otherAABB.GetMax();

    // AABB vs AABB intersection test
    bool noOverlap = aMin.x > bMax.x || bMin.x > aMax.x || aMin.y > bMax.y ||
                     bMin.y > aMax.y || aMin.z > bMax.z || bMin.z > aMax.z;

    return !noOverlap;
  } else if (other.GetType() == ColliderType::Sphere) {
    const SphereCollider &sphere = static_cast<const SphereCollider &>(other);

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
  } else if (other.GetType() == ColliderType::OBB) {
    // OBB vs AABB - let the OBB handle it
    return other.Intersect(*this);
  }

  return false;
}

Vector3 AABBCollider::DetectCollision(const ColliderComponent &other) const {
  if (!Intersect(other)) {
    return Vector3::Zero;
  }

  if (other.GetType() == ColliderType::AABB) {
    const AABBCollider &otherAABB = static_cast<const AABBCollider &>(other);

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

    if (overlapX < overlapY && overlapX < overlapZ) {
      // Resolve on X axis
      penetration.x =
          (aMax.x + aMin.x < bMax.x + bMin.x) ? -overlapX : overlapX;
    } else if (overlapY < overlapZ) {
      // Resolve on Y axis
      penetration.y =
          (aMax.y + aMin.y < bMax.y + bMin.y) ? -overlapY : overlapY;
    } else {
      // Resolve on Z axis
      penetration.z =
          (aMax.z + aMin.z < bMax.z + bMin.z) ? -overlapZ : overlapZ;
    }

    return penetration;
  } else if (other.GetType() == ColliderType::Sphere) {
    const SphereCollider &sphere = static_cast<const SphereCollider &>(other);

    Vector3 sphereCenter = sphere.GetCenter();
    Vector3 closestPoint;

    Vector3 aMin = GetMin();
    Vector3 aMax = GetMax();

    closestPoint.x = Math::Max(aMin.x, Math::Min(sphereCenter.x, aMax.x));
    closestPoint.y = Math::Max(aMin.y, Math::Min(sphereCenter.y, aMax.y));
    closestPoint.z = Math::Max(aMin.z, Math::Min(sphereCenter.z, aMax.z));

    Vector3 direction = sphereCenter - closestPoint;
    float dist = direction.Length();

    if (dist > 0.0f) {
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
    if (dx < dy && dx < dz) {
      // Push on X axis - negate to push AABB away from sphere
      return Vector3(diff.x >= 0.0f ? -dx : dx, 0.0f, 0.0f);
    } else if (dy < dz) {
      // Push on Y axis - negate to push AABB away from sphere
      return Vector3(0.0f, diff.y >= 0.0f ? -dy : dy, 0.0f);
    } else {
      // Push on Z axis - negate to push AABB away from sphere
      return Vector3(0.0f, 0.0f, diff.z >= 0.0f ? -dz : dz);
    }
  } else if (other.GetType() == ColliderType::OBB) {
    // OBB handles this, but we need to negate the result
    return other.DetectCollision(*this) * -1.0f;
  }

  return Vector3::Zero;
}

void AABBCollider::DebugDraw(class Renderer *renderer) {
  // Get the cube mesh from the renderer
  Mesh *cubeMesh = renderer->LoadMesh("cube");
  if (!cubeMesh) {
    return;
  }

  // Calculate center and scale
  Vector3 ownerScale = mOwner->GetScale();
  Vector3 scaledOffset =
      Vector3(mOffset.x * ownerScale.x, mOffset.y * ownerScale.y,
              mOffset.z * ownerScale.z);
  Vector3 center = mOwner->GetPosition() + scaledOffset;
  // mSize is half-extents, multiply by 2 for full size, then apply owner's
  // scale
  Vector3 drawScale =
      Vector3(mSize.x * 2.0f * ownerScale.x, mSize.y * 2.0f * ownerScale.y,
              mSize.z * 2.0f * ownerScale.z);

  // Draw with no rotation (AABB is axis-aligned)
  renderer->DrawSingleMesh(cubeMesh, center, drawScale);
}

// ============================================================================
// OBBCollider (Oriented Bounding Box)
// ============================================================================

OBBCollider::OBBCollider(Actor *owner, ColliderLayer layer, Vector3 offset,
                         Vector3 size, bool isStatic, int updateOrder)
    : ColliderComponent(owner, layer, ColliderType::OBB, isStatic, updateOrder),
      mOffset(offset), mSize(size) {}

OBBCollider::~OBBCollider() {}

Vector3 OBBCollider::GetCenter() const {
  // Apply scale to offset, then transform by rotation
  Vector3 scale = mOwner->GetScale();
  Vector3 scaledOffset =
      Vector3(mOffset.x * scale.x, mOffset.y * scale.y, mOffset.z * scale.z);
  Vector3 rotatedOffset =
      Vector3::Transform(scaledOffset, mOwner->GetRotation());
  return mOwner->GetPosition() + rotatedOffset;
}

bool OBBCollider::Intersect(const ColliderComponent &other) const {
  if (other.GetType() == ColliderType::OBB) {
    // OBB vs OBB using Separating Axis Theorem (SAT)
    const OBBCollider &otherOBB = static_cast<const OBBCollider &>(other);

    Vector3 centerA = GetCenter();
    Vector3 centerB = otherOBB.GetCenter();

    // Get rotation matrices
    Matrix4 rotA = Matrix4::CreateFromQuaternion(mOwner->GetRotation());
    Matrix4 rotB =
        Matrix4::CreateFromQuaternion(otherOBB.mOwner->GetRotation());

    // Get axes for both OBBs
    Vector3 axesA[3] = {
        Vector3(rotA.mat[0][0], rotA.mat[0][1], rotA.mat[0][2]), // X axis
        Vector3(rotA.mat[1][0], rotA.mat[1][1], rotA.mat[1][2]), // Y axis
        Vector3(rotA.mat[2][0], rotA.mat[2][1], rotA.mat[2][2])  // Z axis
    };

    Vector3 axesB[3] = {
        Vector3(rotB.mat[0][0], rotB.mat[0][1], rotB.mat[0][2]),
        Vector3(rotB.mat[1][0], rotB.mat[1][1], rotB.mat[1][2]),
        Vector3(rotB.mat[2][0], rotB.mat[2][1], rotB.mat[2][2])};

    // Apply owner's scale to extents (use absolute value to handle negative
    // scales)
    Vector3 scaleA = mOwner->GetScale();
    Vector3 scaleB = otherOBB.mOwner->GetScale();
    Vector3 extentsA =
        Vector3(mSize.x * Math::Abs(scaleA.x), mSize.y * Math::Abs(scaleA.y),
                mSize.z * Math::Abs(scaleA.z));
    Vector3 extentsB = Vector3(otherOBB.mSize.x * Math::Abs(scaleB.x),
                               otherOBB.mSize.y * Math::Abs(scaleB.y),
                               otherOBB.mSize.z * Math::Abs(scaleB.z));
    Vector3 centerDiff = centerB - centerA;

    // Test 15 axes (3 from A, 3 from B, 9 cross products)
    // For simplicity, we'll test the 6 main axes (good enough for most cases)

    // Test axes from OBB A
    for (int i = 0; i < 3; i++) {
      float ra = (i == 0) ? extentsA.x : (i == 1) ? extentsA.y : extentsA.z;
      float rb = extentsB.x * Math::Abs(Vector3::Dot(axesB[0], axesA[i])) +
                 extentsB.y * Math::Abs(Vector3::Dot(axesB[1], axesA[i])) +
                 extentsB.z * Math::Abs(Vector3::Dot(axesB[2], axesA[i]));

      if (Math::Abs(Vector3::Dot(centerDiff, axesA[i])) > ra + rb)
        return false;
    }

    // Test axes from OBB B
    for (int i = 0; i < 3; i++) {
      float rb = (i == 0) ? extentsB.x : (i == 1) ? extentsB.y : extentsB.z;
      float ra = extentsA.x * Math::Abs(Vector3::Dot(axesA[0], axesB[i])) +
                 extentsA.y * Math::Abs(Vector3::Dot(axesA[1], axesB[i])) +
                 extentsA.z * Math::Abs(Vector3::Dot(axesA[2], axesB[i]));

      if (Math::Abs(Vector3::Dot(centerDiff, axesB[i])) > ra + rb)
        return false;
    }

    // Test edge-edge cross product axes (9 total: 3 from A x 3 from B)
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        Vector3 axis = Vector3::Cross(axesA[i], axesB[j]);
        float axisLength = axis.Length();

        // Skip near-parallel edges (cross product near zero)
        if (axisLength < 0.001f) {
          continue;
        }

        axis.Normalize();

        // Project extents onto this axis
        float ra = extentsA.x * Math::Abs(Vector3::Dot(axesA[0], axis)) +
                   extentsA.y * Math::Abs(Vector3::Dot(axesA[1], axis)) +
                   extentsA.z * Math::Abs(Vector3::Dot(axesA[2], axis));

        float rb = extentsB.x * Math::Abs(Vector3::Dot(axesB[0], axis)) +
                   extentsB.y * Math::Abs(Vector3::Dot(axesB[1], axis)) +
                   extentsB.z * Math::Abs(Vector3::Dot(axesB[2], axis));

        if (Math::Abs(Vector3::Dot(centerDiff, axis)) > ra + rb)
          return false;
      }
    }

    return true;
  } else if (other.GetType() == ColliderType::AABB) {
    // OBB-AABB collision using SAT
    const AABBCollider &aabb = static_cast<const AABBCollider &>(other);

    Vector3 obbCenter = GetCenter();
    Vector3 aabbCenter = (aabb.GetMin() + aabb.GetMax()) * 0.5f;
    Vector3 aabbExtents = (aabb.GetMax() - aabb.GetMin()) * 0.5f;

    // Get OBB axes
    Matrix4 rotOBB = Matrix4::CreateFromQuaternion(mOwner->GetRotation());
    Vector3 obbAxes[3] = {
        Vector3(rotOBB.mat[0][0], rotOBB.mat[0][1], rotOBB.mat[0][2]),
        Vector3(rotOBB.mat[1][0], rotOBB.mat[1][1], rotOBB.mat[1][2]),
        Vector3(rotOBB.mat[2][0], rotOBB.mat[2][1], rotOBB.mat[2][2])};

    // AABB axes are just world axes
    Vector3 aabbAxes[3] = {Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ};

    // Apply scale to OBB extents (use absolute value to handle negative scales)
    Vector3 scale = mOwner->GetScale();
    Vector3 obbExtents =
        Vector3(mSize.x * Math::Abs(scale.x), mSize.y * Math::Abs(scale.y),
                mSize.z * Math::Abs(scale.z));

    Vector3 centerDiff = aabbCenter - obbCenter;

    // Test OBB face normals (3 axes)
    for (int i = 0; i < 3; i++) {
      float obbProj = (i == 0)   ? obbExtents.x
                      : (i == 1) ? obbExtents.y
                                 : obbExtents.z;
      float aabbProj =
          aabbExtents.x * Math::Abs(Vector3::Dot(aabbAxes[0], obbAxes[i])) +
          aabbExtents.y * Math::Abs(Vector3::Dot(aabbAxes[1], obbAxes[i])) +
          aabbExtents.z * Math::Abs(Vector3::Dot(aabbAxes[2], obbAxes[i]));

      float centerProj = Math::Abs(Vector3::Dot(centerDiff, obbAxes[i]));

      if (centerProj > obbProj + aabbProj)
        return false;
    }

    // Test AABB face normals (3 axes)
    for (int i = 0; i < 3; i++) {
      float aabbProj = (i == 0)   ? aabbExtents.x
                       : (i == 1) ? aabbExtents.y
                                  : aabbExtents.z;
      float obbProj =
          obbExtents.x * Math::Abs(Vector3::Dot(obbAxes[0], aabbAxes[i])) +
          obbExtents.y * Math::Abs(Vector3::Dot(obbAxes[1], aabbAxes[i])) +
          obbExtents.z * Math::Abs(Vector3::Dot(obbAxes[2], aabbAxes[i]));

      float centerProj = Math::Abs(Vector3::Dot(centerDiff, aabbAxes[i]));

      if (centerProj > aabbProj + obbProj)
        return false;
    }

    // Test edge-edge cross products (9 axes)
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        Vector3 axis = Vector3::Cross(obbAxes[i], aabbAxes[j]);
        float axisLength = axis.Length();

        if (axisLength < 0.001f)
          continue;

        axis.Normalize();

        float obbProj =
            obbExtents.x * Math::Abs(Vector3::Dot(obbAxes[0], axis)) +
            obbExtents.y * Math::Abs(Vector3::Dot(obbAxes[1], axis)) +
            obbExtents.z * Math::Abs(Vector3::Dot(obbAxes[2], axis));

        float aabbProj =
            aabbExtents.x * Math::Abs(Vector3::Dot(aabbAxes[0], axis)) +
            aabbExtents.y * Math::Abs(Vector3::Dot(aabbAxes[1], axis)) +
            aabbExtents.z * Math::Abs(Vector3::Dot(aabbAxes[2], axis));

        if (Math::Abs(Vector3::Dot(centerDiff, axis)) > obbProj + aabbProj)
          return false;
      }
    }

    return true;
  } else if (other.GetType() == ColliderType::Sphere) {
    const SphereCollider &sphere = static_cast<const SphereCollider &>(other);

    Vector3 obbCenter = GetCenter();
    Vector3 sphereCenter = sphere.GetCenter();

    // Transform sphere center into OBB's local space
    Vector3 diff = sphereCenter - obbCenter;
    Quaternion invRot = mOwner->GetRotation();
    invRot.Conjugate();
    Vector3 localSphereCenter = Vector3::Transform(diff, invRot);

    // Apply owner's scale to OBB extents
    Vector3 scale = mOwner->GetScale();
    Vector3 scaledSize =
        Vector3(mSize.x * scale.x, mSize.y * scale.y, mSize.z * scale.z);

    // Find closest point on OBB (in local space) to sphere center
    Vector3 closestLocal;
    closestLocal.x =
        Math::Max(-scaledSize.x, Math::Min(localSphereCenter.x, scaledSize.x));
    closestLocal.y =
        Math::Max(-scaledSize.y, Math::Min(localSphereCenter.y, scaledSize.y));
    closestLocal.z =
        Math::Max(-scaledSize.z, Math::Min(localSphereCenter.z, scaledSize.z));

    // Distance from sphere center to closest point on OBB
    float distSq = (localSphereCenter - closestLocal).LengthSq();
    float radius = sphere.GetRadius();

    return distSq <= (radius * radius);
  }

  return false;
}

Vector3 OBBCollider::DetectCollision(const ColliderComponent &other) const {
  if (!Intersect(other)) {
    return Vector3::Zero;
  }

  Vector3 obbCenter = GetCenter();
  Quaternion rotation = mOwner->GetRotation();
  Quaternion invRot = rotation;
  invRot.Conjugate();

  if (other.GetType() == ColliderType::OBB) {
    // OBB-OBB: find minimum penetration axis using SAT
    const OBBCollider &otherOBB = static_cast<const OBBCollider &>(other);
    Vector3 centerB = otherOBB.GetCenter();

    // Get rotation matrices
    Matrix4 rotA = Matrix4::CreateFromQuaternion(mOwner->GetRotation());
    Matrix4 rotB =
        Matrix4::CreateFromQuaternion(otherOBB.mOwner->GetRotation());

    // Get axes for both OBBs
    Vector3 axesA[3] = {
        Vector3(rotA.mat[0][0], rotA.mat[0][1], rotA.mat[0][2]),
        Vector3(rotA.mat[1][0], rotA.mat[1][1], rotA.mat[1][2]),
        Vector3(rotA.mat[2][0], rotA.mat[2][1], rotA.mat[2][2])};

    Vector3 axesB[3] = {
        Vector3(rotB.mat[0][0], rotB.mat[0][1], rotB.mat[0][2]),
        Vector3(rotB.mat[1][0], rotB.mat[1][1], rotB.mat[1][2]),
        Vector3(rotB.mat[2][0], rotB.mat[2][1], rotB.mat[2][2])};

    // Apply owner's scale to extents
    // Apply owner's scale to extents (use absolute value to handle negative
    // scales)
    Vector3 scaleA = mOwner->GetScale();
    Vector3 scaleB = otherOBB.mOwner->GetScale();
    Vector3 extentsA =
        Vector3(mSize.x * Math::Abs(scaleA.x), mSize.y * Math::Abs(scaleA.y),
                mSize.z * Math::Abs(scaleA.z));
    Vector3 extentsB = Vector3(otherOBB.mSize.x * Math::Abs(scaleB.x),
                               otherOBB.mSize.y * Math::Abs(scaleB.y),
                               otherOBB.mSize.z * Math::Abs(scaleB.z));
    Vector3 centerDiff = centerB - obbCenter;

    float minPenetration = FLT_MAX;
    Vector3 minAxis;

    // Test axes from OBB A
    for (int i = 0; i < 3; i++) {
      float ra = (i == 0) ? extentsA.x : (i == 1) ? extentsA.y : extentsA.z;
      float rb = extentsB.x * Math::Abs(Vector3::Dot(axesB[0], axesA[i])) +
                 extentsB.y * Math::Abs(Vector3::Dot(axesB[1], axesA[i])) +
                 extentsB.z * Math::Abs(Vector3::Dot(axesB[2], axesA[i]));

      float separation =
          Math::Abs(Vector3::Dot(centerDiff, axesA[i])) - (ra + rb);
      float penetration = -(separation);

      if (penetration < minPenetration && penetration > 0.0f) {
        minPenetration = penetration;
        minAxis = axesA[i];
        // Make sure axis points from B to A (push A away from B)
        if (Vector3::Dot(centerDiff, minAxis) > 0.0f)
          minAxis = minAxis * -1.0f;
      }
    }

    // Test axes from OBB B
    for (int i = 0; i < 3; i++) {
      float rb = (i == 0) ? extentsB.x : (i == 1) ? extentsB.y : extentsB.z;
      float ra = extentsA.x * Math::Abs(Vector3::Dot(axesA[0], axesB[i])) +
                 extentsA.y * Math::Abs(Vector3::Dot(axesA[1], axesB[i])) +
                 extentsA.z * Math::Abs(Vector3::Dot(axesA[2], axesB[i]));

      float separation =
          Math::Abs(Vector3::Dot(centerDiff, axesB[i])) - (ra + rb);
      float penetration = -(separation);

      if (penetration < minPenetration && penetration > 0.0f) {
        minPenetration = penetration;
        minAxis = axesB[i];
        // Make sure axis points from B to A (push A away from B)
        if (Vector3::Dot(centerDiff, minAxis) > 0.0f)
          minAxis = minAxis * -1.0f;
      }
    }

    // Test edge-edge cross product axes (9 total: 3 from A x 3 from B)
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        Vector3 axis = Vector3::Cross(axesA[i], axesB[j]);
        float axisLength = axis.Length();

        // Skip near-parallel edges (cross product near zero)
        if (axisLength < 0.001f) {
          continue;
        }

        axis.Normalize();

        // Project extents onto this axis
        float ra = extentsA.x * Math::Abs(Vector3::Dot(axesA[0], axis)) +
                   extentsA.y * Math::Abs(Vector3::Dot(axesA[1], axis)) +
                   extentsA.z * Math::Abs(Vector3::Dot(axesA[2], axis));

        float rb = extentsB.x * Math::Abs(Vector3::Dot(axesB[0], axis)) +
                   extentsB.y * Math::Abs(Vector3::Dot(axesB[1], axis)) +
                   extentsB.z * Math::Abs(Vector3::Dot(axesB[2], axis));

        float separation =
            Math::Abs(Vector3::Dot(centerDiff, axis)) - (ra + rb);
        float penetration = -(separation);

        if (penetration < minPenetration && penetration > 0.0f) {
          minPenetration = penetration;
          minAxis = axis;
          // Make sure axis points from B to A (push A away from B)
          if (Vector3::Dot(centerDiff, minAxis) > 0.0f)
            minAxis = minAxis * -1.0f;
        }
      }
    }

    if (minPenetration < FLT_MAX) {
      // Return penetration for OBB A (pointing away from B, pushing A out)
      return minAxis * minPenetration;
    }

    return Vector3(0.0f, extentsA.y + extentsB.y, 0.0f);
  } else if (other.GetType() == ColliderType::AABB) {
    // OBB-AABB: Use SAT to find minimum penetration axis
    const AABBCollider &aabb = static_cast<const AABBCollider &>(other);

    Vector3 aabbCenter = (aabb.GetMin() + aabb.GetMax()) * 0.5f;
    Vector3 aabbExtents = (aabb.GetMax() - aabb.GetMin()) * 0.5f;

    // Get OBB axes
    Matrix4 rotOBB = Matrix4::CreateFromQuaternion(mOwner->GetRotation());
    Vector3 obbAxes[3] = {
        Vector3(rotOBB.mat[0][0], rotOBB.mat[0][1], rotOBB.mat[0][2]),
        Vector3(rotOBB.mat[1][0], rotOBB.mat[1][1], rotOBB.mat[1][2]),
        Vector3(rotOBB.mat[2][0], rotOBB.mat[2][1], rotOBB.mat[2][2])};

    // AABB axes are just world axes
    Vector3 aabbAxes[3] = {Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ};

    // Apply scale to OBB extents (use absolute value to handle negative scales)
    Vector3 scale = mOwner->GetScale();
    Vector3 obbExtents =
        Vector3(mSize.x * Math::Abs(scale.x), mSize.y * Math::Abs(scale.y),
                mSize.z * Math::Abs(scale.z));

    Vector3 centerDiff = aabbCenter - obbCenter;

    float minPenetration = FLT_MAX;
    Vector3 minAxis;

    // Test OBB face normals (3 axes)
    for (int i = 0; i < 3; i++) {
      float obbProj = (i == 0)   ? obbExtents.x
                      : (i == 1) ? obbExtents.y
                                 : obbExtents.z;
      float aabbProj =
          aabbExtents.x * Math::Abs(Vector3::Dot(aabbAxes[0], obbAxes[i])) +
          aabbExtents.y * Math::Abs(Vector3::Dot(aabbAxes[1], obbAxes[i])) +
          aabbExtents.z * Math::Abs(Vector3::Dot(aabbAxes[2], obbAxes[i]));

      float centerProj = Vector3::Dot(centerDiff, obbAxes[i]);
      float separation = Math::Abs(centerProj) - (obbProj + aabbProj);
      float penetration = -(separation);

      if (penetration < minPenetration && penetration > 0.0f) {
        minPenetration = penetration;
        minAxis = obbAxes[i];
        // Make sure axis points from AABB to OBB (push OBB away from AABB)
        if (centerProj > 0.0f)
          minAxis = minAxis * -1.0f;
      }
    }

    // Test AABB face normals (3 axes)
    for (int i = 0; i < 3; i++) {
      float aabbProj = (i == 0)   ? aabbExtents.x
                       : (i == 1) ? aabbExtents.y
                                  : aabbExtents.z;
      float obbProj =
          obbExtents.x * Math::Abs(Vector3::Dot(obbAxes[0], aabbAxes[i])) +
          obbExtents.y * Math::Abs(Vector3::Dot(obbAxes[1], aabbAxes[i])) +
          obbExtents.z * Math::Abs(Vector3::Dot(obbAxes[2], aabbAxes[i]));

      float centerProj = Vector3::Dot(centerDiff, aabbAxes[i]);
      float separation = Math::Abs(centerProj) - (aabbProj + obbProj);
      float penetration = -(separation);

      if (penetration < minPenetration && penetration > 0.0f) {
        minPenetration = penetration;
        minAxis = aabbAxes[i];
        // Make sure axis points from AABB to OBB (push OBB away from AABB)
        if (centerProj > 0.0f)
          minAxis = minAxis * -1.0f;
      }
    }

    // Test edge-edge cross products (9 axes) - usually not needed for
    // axis-aligned cases
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        Vector3 axis = Vector3::Cross(obbAxes[i], aabbAxes[j]);
        float axisLength = axis.Length();

        if (axisLength < 0.001f)
          continue;

        axis.Normalize();

        float obbProj =
            obbExtents.x * Math::Abs(Vector3::Dot(obbAxes[0], axis)) +
            obbExtents.y * Math::Abs(Vector3::Dot(obbAxes[1], axis)) +
            obbExtents.z * Math::Abs(Vector3::Dot(obbAxes[2], axis));

        float aabbProj =
            aabbExtents.x * Math::Abs(Vector3::Dot(aabbAxes[0], axis)) +
            aabbExtents.y * Math::Abs(Vector3::Dot(aabbAxes[1], axis)) +
            aabbExtents.z * Math::Abs(Vector3::Dot(aabbAxes[2], axis));

        float centerProj = Vector3::Dot(centerDiff, axis);
        float separation = Math::Abs(centerProj) - (obbProj + aabbProj);
        float penetration = -(separation);

        if (penetration < minPenetration && penetration > 0.0f) {
          minPenetration = penetration;
          minAxis = axis;
          // Make sure axis points from AABB to OBB (push OBB away from AABB)
          if (centerProj > 0.0f)
            minAxis = minAxis * -1.0f;
        }
      }
    }

    if (minPenetration < FLT_MAX) {
      return minAxis * minPenetration;
    }

    // Fallback (should not reach here if Intersect returned true)
    return Vector3(0.0f, 0.01f, 0.0f);
  } else if (other.GetType() == ColliderType::Sphere) {
    // OBB-Sphere: find closest point on OBB to sphere center
    const SphereCollider &sphere = static_cast<const SphereCollider &>(other);
    Vector3 sphereCenter = sphere.GetCenter();

    // Transform sphere center into OBB's local space
    Vector3 diff = sphereCenter - obbCenter;
    Vector3 localSphereCenter = Vector3::Transform(diff, invRot);

    // Apply owner's scale to OBB extents
    Vector3 scale = mOwner->GetScale();
    Vector3 scaledSize =
        Vector3(mSize.x * scale.x, mSize.y * scale.y, mSize.z * scale.z);

    // Find closest point on OBB to sphere center (in local space)
    Vector3 closestLocal;
    closestLocal.x =
        Math::Max(-scaledSize.x, Math::Min(localSphereCenter.x, scaledSize.x));
    closestLocal.y =
        Math::Max(-scaledSize.y, Math::Min(localSphereCenter.y, scaledSize.y));
    closestLocal.z =
        Math::Max(-scaledSize.z, Math::Min(localSphereCenter.z, scaledSize.z));

    // Calculate penetration in local space (negate to push OBB away from
    // sphere)
    Vector3 localDirection = localSphereCenter - closestLocal;
    float dist = localDirection.Length();

    if (dist > 0.0f) {
      localDirection.Normalize();
      float penetrationDepth = sphere.GetRadius() - dist;
      // Negate direction to push OBB away from sphere
      Vector3 localPenetration = localDirection * penetrationDepth * -1.0f;

      // Transform back to world space
      return Vector3::Transform(localPenetration, rotation);
    }

    // Sphere center is exactly on OBB surface - push OBB away along negative Y
    return Vector3::Transform(Vector3(0.0f, -sphere.GetRadius(), 0.0f),
                              rotation);
  }

  return Vector3::Zero;
}

void OBBCollider::DebugDraw(class Renderer *renderer) {
  // Get the cube mesh from the renderer
  Mesh *cubeMesh = renderer->LoadMesh("cube");
  if (!cubeMesh) {
    return;
  }

  Vector3 center = GetCenter();
  Vector3 ownerScale = mOwner->GetScale();
  // mSize is half-extents, multiply by 2 for full size, then apply owner's
  // scale
  Vector3 drawScale =
      Vector3(mSize.x * 2.0f * ownerScale.x, mSize.y * 2.0f * ownerScale.y,
              mSize.z * 2.0f * ownerScale.z);
  Quaternion rotation = mOwner->GetRotation();

  renderer->DrawSingleMesh(cubeMesh, center, drawScale, rotation);
}

// ============================================================================
// SphereCollider
// ============================================================================

SphereCollider::SphereCollider(Actor *owner, ColliderLayer layer,
                               Vector3 offset, float radius, bool isStatic,
                               int updateOrder)
    : ColliderComponent(owner, layer, ColliderType::Sphere, isStatic,
                        updateOrder),
      mOffset(offset), mRadius(radius) {}

SphereCollider::~SphereCollider() {}

Vector3 SphereCollider::GetCenter() const {
  // Apply scale to offset and transform by rotation
  Vector3 scale = mOwner->GetScale();
  Vector3 scaledOffset =
      Vector3(mOffset.x * scale.x, mOffset.y * scale.y, mOffset.z * scale.z);
  Vector3 rotatedOffset =
      Vector3::Transform(scaledOffset, mOwner->GetRotation());
  return mOwner->GetPosition() + rotatedOffset;
}

float SphereCollider::GetRadius() const {
  // Apply scale to radius (use max absolute scale component for uniform
  // scaling)
  Vector3 scale = mOwner->GetScale();
  float maxScale = Math::Max(Math::Abs(scale.x),
                             Math::Max(Math::Abs(scale.y), Math::Abs(scale.z)));
  return mRadius * maxScale;
}

bool SphereCollider::Intersect(const ColliderComponent &other) const {
  if (other.GetType() == ColliderType::Sphere) {
    const SphereCollider &otherSphere =
        static_cast<const SphereCollider &>(other);

    Vector3 centerA = GetCenter();
    Vector3 centerB = otherSphere.GetCenter();

    // GetRadius() already applies scale
    float radiusSum = GetRadius() + otherSphere.GetRadius();
    float distSq = (centerA - centerB).LengthSq();

    return distSq <= (radiusSum * radiusSum);
  } else if (other.GetType() == ColliderType::AABB) {
    // AABB handles this case
    return other.Intersect(*this);
  } else if (other.GetType() == ColliderType::OBB) {
    // OBB handles this case
    return other.Intersect(*this);
  }

  return false;
}

Vector3 SphereCollider::DetectCollision(const ColliderComponent &other) const {
  if (!Intersect(other)) {
    return Vector3::Zero;
  }

  if (other.GetType() == ColliderType::Sphere) {
    const SphereCollider &otherSphere =
        static_cast<const SphereCollider &>(other);

    Vector3 centerA = GetCenter();
    Vector3 centerB = otherSphere.GetCenter();

    Vector3 direction = centerA - centerB;
    float dist = direction.Length();

    // GetRadius() already applies scale
    float radiusSum = GetRadius() + otherSphere.GetRadius();

    if (dist > 0.0f) {
      direction.Normalize();
      float penetrationDepth = radiusSum - dist;
      return direction * penetrationDepth;
    }

    // Centers are at same position - push up by default
    return Vector3(0.0f, radiusSum, 0.0f);
  } else if (other.GetType() == ColliderType::AABB) {
    // AABB returns penetration for itself (to push AABB away from sphere)
    // Negate it to get penetration for sphere (to push sphere away from AABB)
    return other.DetectCollision(*this) * -1.0f;
  } else if (other.GetType() == ColliderType::OBB) {
    // OBB handles this, but we need to negate the result
    return other.DetectCollision(*this) * -1.0f;
  }

  return Vector3::Zero;
}

void SphereCollider::DebugDraw(Renderer *renderer) {
  // Get the sphere mesh from the renderer
  Mesh *sphereMesh = renderer->LoadMesh("sphere");
  if (!sphereMesh) {
    return;
  }

  // Calculate center and scale
  Vector3 center = GetCenter();
  float diameter =
      mRadius * 2.0f; // mRadius is radius, so multiply by 2 for diameter
  Vector3 scale(diameter, diameter, diameter);

  // Draw sphere (no rotation needed for a sphere)
  renderer->DrawSingleMesh(sphereMesh, center, scale);
}
