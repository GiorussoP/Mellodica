// Math.h
// Math library mostly for vectors, matrices, and quaternions

#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include <limits>
#include <memory.h>

namespace Math {
// NOLINTBEGIN
constexpr float Pi = 3.1415926535f;
constexpr float TwoPi = Pi * 2.0f;
constexpr float PiOver2 = Pi / 2.0f;
constexpr float Infinity = std::numeric_limits<float>::infinity();
constexpr float NegInfinity = -std::numeric_limits<float>::infinity();
// NOLINTEND

[[nodiscard]] inline constexpr float ToRadians(float degrees) {
  return degrees * Pi / 180.0f;
}

[[nodiscard]] inline constexpr float ToDegrees(float radians) {
  return radians * 180.0f / Pi;
}

template <typename T> [[nodiscard]] constexpr T Max(T a, T b) {
  return (a < b ? b : a);
}

template <typename T> [[nodiscard]] constexpr T Min(T a, T b) {
  return (a < b ? a : b);
}

template <typename T>
[[nodiscard]] constexpr T Clamp(T value, T lower, T upper) {
  return Min(upper, Max(lower, value));
}

[[nodiscard]] inline float Abs(float value) { return fabs(value); }

[[nodiscard]] inline float Cos(float angle) { return cosf(angle); }

[[nodiscard]] inline float Sin(float angle) { return sinf(angle); }

[[nodiscard]] inline float Tan(float angle) { return tanf(angle); }

[[nodiscard]] inline float Acos(float value) { return acosf(value); }

[[nodiscard]] inline float Atan2(float y, float x) { return atan2f(y, x); }

[[nodiscard]] inline float Cot(float angle) { return 1.0f / Tan(angle); }

[[nodiscard]] inline constexpr float Lerp(float a, float b, float f) {
  return a + f * (b - a);
}

[[nodiscard]] inline float Sqrt(float value) { return sqrtf(value); }

[[nodiscard]] inline float Fmod(float numer, float denom) {
  return fmod(numer, denom);
}

[[nodiscard]] inline constexpr float Sgn(float a) {
  if (a > 0.0f)
    return 1.0f;
  if (a < 0.0f)
    return -1.0f;
  return 0.0f;
}
} // namespace Math

// 2D Vector
class Vector2 {
public:
  // NOLINTBEGIN
  float x;
  float y;
  // NOLINTEND

  constexpr Vector2() : x(0.0f), y(0.0f) {}

  explicit constexpr Vector2(float inX, float inY) : x(inX), y(inY) {}

  explicit constexpr Vector2(float inXY) : x(inXY), y(inXY) {}

  explicit constexpr Vector2(int inX, int inY)
      : x(static_cast<float>(inX)), y(static_cast<float>(inY)) {}

  // Cast to a const float pointer
  const float *GetAsFloatPtr() const {
    return reinterpret_cast<const float *>(&x);
  }

  // Set both components in one line
  void Set(float inX, float inY) {
    x = inX;
    y = inY;
  }

  // Vector addition (a + b)
  [[nodiscard]] friend Vector2 operator+(const Vector2 &a, const Vector2 &b) {
    return Vector2(a.x + b.x, a.y + b.y);
  }

  // Vector subtraction (a - b)
  [[nodiscard]] friend Vector2 operator-(const Vector2 &a, const Vector2 &b) {
    return Vector2(a.x - b.x, a.y - b.y);
  }

  // Component-wise multiplication
  // (a.x * b.x, ...)
  [[nodiscard]] friend Vector2 operator*(const Vector2 &a, const Vector2 &b) {
    return Vector2(a.x * b.x, a.y * b.y);
  }

  // Scalar multiplication
  [[nodiscard]] friend Vector2 operator*(const Vector2 &vec, float scalar) {
    return Vector2(vec.x * scalar, vec.y * scalar);
  }

  // Scalar multiplication
  [[nodiscard]] friend Vector2 operator*(float scalar, const Vector2 &vec) {
    return Vector2(vec.x * scalar, vec.y * scalar);
  }

  // Scalar *=
  Vector2 &operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  // Vector +=
  Vector2 &operator+=(const Vector2 &right) {
    x += right.x;
    y += right.y;
    return *this;
  }

  // Vector -=
  Vector2 &operator-=(const Vector2 &right) {
    x -= right.x;
    y -= right.y;
    return *this;
  }

  // Length squared of vector
  [[nodiscard]] float LengthSq() const { return (x * x + y * y); }

  // Length of vector
  [[nodiscard]] float Length() const { return (Math::Sqrt(LengthSq())); }

  // Normalize this vector
  void Normalize() {
    float length = Length();
    x /= length;
    y /= length;
  }

  // Normalize the provided vector
  [[nodiscard]] static Vector2 Normalize(const Vector2 &vec) {
    Vector2 temp = vec;
    temp.Normalize();
    return temp;
  }

  // Dot product between two vectors (a dot b)
  [[nodiscard]] static float Dot(const Vector2 &a, const Vector2 &b) {
    return (a.x * b.x + a.y * b.y);
  }

  // Lerp from A to B by f
  [[nodiscard]] static Vector2 Lerp(const Vector2 &a, const Vector2 &b,
                                    float f) {
    return a + f * (b - a);
  }

  // Reflect V about (normalized) N
  [[nodiscard]] static Vector2 Reflect(const Vector2 &v, const Vector2 &n) {
    return v - 2.0f * Vector2::Dot(v, n) * n;
  }

  // Transform vector by matrix
  [[nodiscard]] static Vector2
  Transform(const Vector2 &vec, const class Matrix3 &mat, float w = 1.0f);

  // Get distance between two points
  [[nodiscard]] static float Distance(const Vector2 &a, const Vector2 &b) {
    return (a - b).Length();
  }

  // NOLINTBEGIN
  static const Vector2 Zero;
  static const Vector2 One;
  static const Vector2 UnitX;
  static const Vector2 UnitY;
  static const Vector2 NegUnitX;
  static const Vector2 NegUnitY;
  // NOLINTEND
};

inline constexpr Vector2 Vector2::Zero(0.0f, 0.0f);
inline constexpr Vector2 Vector2::One(1.0f, 1.0f);
inline constexpr Vector2 Vector2::UnitX(1.0f, 0.0f);
inline constexpr Vector2 Vector2::UnitY(0.0f, 1.0f);
inline constexpr Vector2 Vector2::NegUnitX(-1.0f, 0.0f);
inline constexpr Vector2 Vector2::NegUnitY(0.0f, -1.0f);

// 3D Vector
class Vector3 {
public:
  // NOLINTBEGIN
  float x;
  float y;
  float z;
  // NOLINTEND

  constexpr Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

  explicit constexpr Vector3(float inX, float inY, float inZ)
      : x(inX), y(inY), z(inZ) {}

  explicit constexpr Vector3(float inXYZ) : x(inXYZ), y(inXYZ), z(inXYZ) {}

  explicit constexpr Vector3(int inX, int inY, int inZ)
      : x(static_cast<float>(inX)), y(static_cast<float>(inY)),
        z(static_cast<float>(inZ)) {}

  // Cast to a const float pointer
  const float *GetAsFloatPtr() const {
    return reinterpret_cast<const float *>(&x);
  }

  // Set all three components in one line
  void Set(float inX, float inY, float inZ) {
    x = inX;
    y = inY;
    z = inZ;
  }

  // Vector addition (a + b)
  [[nodiscard]] friend Vector3 operator+(const Vector3 &a, const Vector3 &b) {
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
  }

  // Vector subtraction (a - b)
  [[nodiscard]] friend Vector3 operator-(const Vector3 &a, const Vector3 &b) {
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
  }

  // Component-wise multiplication
  [[nodiscard]] friend Vector3 operator*(const Vector3 &left,
                                         const Vector3 &right) {
    return Vector3(left.x * right.x, left.y * right.y, left.z * right.z);
  }

  // Scalar multiplication
  [[nodiscard]] friend Vector3 operator*(const Vector3 &vec, float scalar) {
    return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
  }

  // Scalar multiplication
  [[nodiscard]] friend Vector3 operator*(float scalar, const Vector3 &vec) {
    return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
  }

  // Scalar *=
  Vector3 &operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  // Vector +=
  Vector3 &operator+=(const Vector3 &right) {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
  }

  // Vector -=
  Vector3 &operator-=(const Vector3 &right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
  }

  // Length squared of vector
  [[nodiscard]] float LengthSq() const { return (x * x + y * y + z * z); }

  // Length of vector
  [[nodiscard]] float Length() const { return (Math::Sqrt(LengthSq())); }

  // Normalize this vector
  void Normalize() {
    float length = Length();
    x /= length;
    y /= length;
    z /= length;
  }

  // Project on plane defined by normal
  void ProjectOnPlane(const Vector3 &normal) {
    Vector3 n = normal;
    n.Normalize();
    float dist = Vector3::Dot(*this, n);
    *this -= dist * n;
  }

  // Project this vector onto the plane defined by the normal
  [[nodiscard]] Vector3 ProjectedOnPlane(const Vector3 &normal) const {
    Vector3 n = normal;
    n.Normalize();
    float dist = Vector3::Dot(*this, n);
    return *this - dist * n;
  }

  // Normalize the provided vector
  [[nodiscard]] static Vector3 Normalize(const Vector3 &vec) {
    Vector3 temp = vec;
    temp.Normalize();
    return temp;
  }

  // Dot product between two vectors (a dot b)
  [[nodiscard]] static float Dot(const Vector3 &a, const Vector3 &b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
  }

  // Cross product between two vectors (a cross b)
  [[nodiscard]] static Vector3 Cross(const Vector3 &a, const Vector3 &b) {
    Vector3 temp;
    temp.x = a.y * b.z - a.z * b.y;
    temp.y = a.z * b.x - a.x * b.z;
    temp.z = a.x * b.y - a.y * b.x;
    return temp;
  }

  // Lerp from A to B by f
  [[nodiscard]] static Vector3 Lerp(const Vector3 &a, const Vector3 &b,
                                    float f) {
    return a + f * (b - a);
  }

  // Reflect V about (normalized) N
  [[nodiscard]] static Vector3 Reflect(const Vector3 &v, const Vector3 &n) {
    return v - 2.0f * Vector3::Dot(v, n) * n;
  }

  [[nodiscard]] static Vector3
  Transform(const Vector3 &vec, const class Matrix4 &mat, float w = 1.0f);
  // This will transform the vector and renormalize the w component
  [[nodiscard]] static Vector3 TransformWithPerspDiv(const Vector3 &vec,
                                                     const class Matrix4 &mat,
                                                     float w = 1.0f);

  // Transform a Vector3 by a quaternion
  [[nodiscard]] static Vector3 Transform(const Vector3 &v,
                                         const class Quaternion &q);

  // Get distance between two points
  [[nodiscard]] static float Distance(const Vector3 &a, const Vector3 &b) {
    return (a - b).Length();
  }

  // NOLINTBEGIN
  static const Vector3 Zero;
  static const Vector3 One;
  static const Vector3 UnitX;
  static const Vector3 UnitY;
  static const Vector3 UnitZ;
  static const Vector3 NegUnitX;
  static const Vector3 NegUnitY;
  static const Vector3 NegUnitZ;
  static const Vector3 Infinity;
  static const Vector3 NegInfinity;
  // NOLINTEND
};

inline constexpr Vector3 Vector3::Zero(0.0f, 0.0f, 0.f);
inline constexpr Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
inline constexpr Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
inline constexpr Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
inline constexpr Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);
inline constexpr Vector3 Vector3::NegUnitX(-1.0f, 0.0f, 0.0f);
inline constexpr Vector3 Vector3::NegUnitY(0.0f, -1.0f, 0.0f);
inline constexpr Vector3 Vector3::NegUnitZ(0.0f, 0.0f, -1.0f);
inline constexpr Vector3 Vector3::Infinity(Math::Infinity, Math::Infinity,
                                           Math::Infinity);
inline constexpr Vector3 Vector3::NegInfinity(Math::NegInfinity,
                                              Math::NegInfinity,
                                              Math::NegInfinity);

// 3D Vector
class Vector4 {
public:
  // NOLINTBEGIN
  float x;
  float y;
  float z;
  float w;
  // NOLINTEND

  constexpr Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

  explicit constexpr Vector4(float inX, float inY, float inZ, float inW)
      : x(inX), y(inY), z(inZ), w(inW) {}

  explicit constexpr Vector4(float inXYZW)
      : x(inXYZW), y(inXYZW), z(inXYZW), w(inXYZW) {}

  explicit constexpr Vector4(int inX, int inY, int inZ, int inW)
      : x(static_cast<float>(inX)), y(static_cast<float>(inY)),
        z(static_cast<float>(inZ)), w(static_cast<float>(inW)) {}

  explicit constexpr Vector4(const Vector3 &v, float inW)
      : x(v.x), y(v.y), z(v.z), w(inW) {}

  // Cast to a const float pointer
  const float *GetAsFloatPtr() const {
    return reinterpret_cast<const float *>(&x);
  }

  // Set all three components in one line
  void Set(float inX, float inY, float inZ, float inW) {
    x = inX;
    y = inY;
    z = inZ;
    w = inW;
  }

  // Vector addition (a + b)
  [[nodiscard]] friend Vector4 operator+(const Vector4 &a, const Vector4 &b) {
    return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
  }

  // Vector subtraction (a - b)
  [[nodiscard]] friend Vector4 operator-(const Vector4 &a, const Vector4 &b) {
    return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.z - b.z);
  }

  // Component-wise multiplication
  [[nodiscard]] friend Vector4 operator*(const Vector4 &left,
                                         const Vector4 &right) {
    return Vector4(left.x * right.x, left.y * right.y, left.z * right.z,
                   left.w * right.w);
  }

  // Scalar multiplication
  [[nodiscard]] friend Vector4 operator*(const Vector4 &vec, float scalar) {
    return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar,
                   vec.w * scalar);
  }

  // Scalar multiplication
  [[nodiscard]] friend Vector4 operator*(float scalar, const Vector4 &vec) {
    return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar,
                   vec.w * scalar);
  }

  // Scalar *=
  Vector4 &operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
  }

  // Vector +=
  Vector4 &operator+=(const Vector4 &right) {
    x += right.x;
    y += right.y;
    z += right.z;
    w += right.w;
    return *this;
  }

  // Vector -=
  Vector4 &operator-=(const Vector4 &right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    z -= right.w;
    return *this;
  }

  // Length squared of vector
  [[nodiscard]] float LengthSq() const {
    return (x * x + y * y + z * z + w * w);
  }

  // Length of vector
  [[nodiscard]] float Length() const { return (Math::Sqrt(LengthSq())); }

  // Normalize this vector
  void Normalize() {
    float length = Length();
    x /= length;
    y /= length;
    z /= length;
    w /= length;
  }

  // Normalize the provided vector
  [[nodiscard]] static Vector4 Normalize(const Vector4 &vec) {
    Vector4 temp = vec;
    temp.Normalize();
    return temp;
  }

  // Dot product between two vectors (a dot b)
  [[nodiscard]] static float Dot(const Vector4 &a, const Vector4 &b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
  }

  // Lerp from A to B by f
  [[nodiscard]] static Vector4 Lerp(const Vector4 &a, const Vector4 &b,
                                    float f) {
    return a + f * (b - a);
  }

  [[nodiscard]] static Vector4 Transform(const Vector4 &vec,
                                         const class Matrix4 &mat);

  // NOLINTBEGIN
  static const Vector4 Zero;
  static const Vector4 One;
  static const Vector4 UnitX;
  static const Vector4 UnitY;
  static const Vector4 UnitZ;
  static const Vector4 UnitW;
  static const Vector4 UnitRect;
  static const Vector4 NegUnitX;
  static const Vector4 NegUnitY;
  static const Vector4 NegUnitZ;
  static const Vector4 NegUnitW;
  static const Vector4 Infinity;
  static const Vector4 NegInfinity;
  // NOLINTEND
};

inline constexpr Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
inline constexpr Vector4 Vector4::One(1.0f, 1.0f, 1.0f, 1.0f);
inline constexpr Vector4 Vector4::UnitRect(0.0f, 0.0f, 1.0f, 1.0f);
inline constexpr Vector4 Vector4::UnitX(1.0f, 0.0f, 0.0f, 0.0f);
inline constexpr Vector4 Vector4::UnitY(0.0f, 1.0f, 0.0f, 0.0f);
inline constexpr Vector4 Vector4::UnitZ(0.0f, 0.0f, 1.0f, 1.0f);
inline constexpr Vector4 Vector4::UnitW(0.0f, 0.0f, 0.0f, 1.0f);
inline constexpr Vector4 Vector4::NegUnitX(-1.0f, 0.0f, 0.0f, 0.0f);
inline constexpr Vector4 Vector4::NegUnitY(0.0f, -1.0f, 0.0f, 0.0f);
inline constexpr Vector4 Vector4::NegUnitZ(0.0f, 0.0f, -1.0f, 0.0f);
inline constexpr Vector4 Vector4::NegUnitW(0.0f, 0.0f, 0.0f, -1.0f);
inline constexpr Vector4 Vector4::Infinity(Math::Infinity, Math::Infinity,
                                           Math::Infinity, Math::Infinity);
inline constexpr Vector4 Vector4::NegInfinity(Math::NegInfinity,
                                              Math::NegInfinity,
                                              Math::NegInfinity,
                                              Math::NegInfinity);

// 3x3 Matrix
class Matrix3 {
public:
  float mat[3][3]; // NOLINT

  // NOLINTBEGIN
  Matrix3() { *this = Matrix3::Identity; }

  explicit Matrix3(float inMat[3][3]) { memcpy(mat, inMat, 9 * sizeof(float)); }
  // NOLINTEND

  // Cast to a const float pointer
  const float *GetAsFloatPtr() const {
    return reinterpret_cast<const float *>(&mat[0][0]);
  }

  // Matrix multiplication
  [[nodiscard]] friend Matrix3 operator*(const Matrix3 &left,
                                         const Matrix3 &right) {
    Matrix3 retVal;
    // row 0
    retVal.mat[0][0] = left.mat[0][0] * right.mat[0][0] +
                       left.mat[0][1] * right.mat[1][0] +
                       left.mat[0][2] * right.mat[2][0];

    retVal.mat[0][1] = left.mat[0][0] * right.mat[0][1] +
                       left.mat[0][1] * right.mat[1][1] +
                       left.mat[0][2] * right.mat[2][1];

    retVal.mat[0][2] = left.mat[0][0] * right.mat[0][2] +
                       left.mat[0][1] * right.mat[1][2] +
                       left.mat[0][2] * right.mat[2][2];

    // row 1
    retVal.mat[1][0] = left.mat[1][0] * right.mat[0][0] +
                       left.mat[1][1] * right.mat[1][0] +
                       left.mat[1][2] * right.mat[2][0];

    retVal.mat[1][1] = left.mat[1][0] * right.mat[0][1] +
                       left.mat[1][1] * right.mat[1][1] +
                       left.mat[1][2] * right.mat[2][1];

    retVal.mat[1][2] = left.mat[1][0] * right.mat[0][2] +
                       left.mat[1][1] * right.mat[1][2] +
                       left.mat[1][2] * right.mat[2][2];

    // row 2
    retVal.mat[2][0] = left.mat[2][0] * right.mat[0][0] +
                       left.mat[2][1] * right.mat[1][0] +
                       left.mat[2][2] * right.mat[2][0];

    retVal.mat[2][1] = left.mat[2][0] * right.mat[0][1] +
                       left.mat[2][1] * right.mat[1][1] +
                       left.mat[2][2] * right.mat[2][1];

    retVal.mat[2][2] = left.mat[2][0] * right.mat[0][2] +
                       left.mat[2][1] * right.mat[1][2] +
                       left.mat[2][2] * right.mat[2][2];

    return retVal;
  }

  Matrix3 &operator*=(const Matrix3 &right) {
    *this = *this * right;
    return *this;
  }

  // Create a scale matrix with x and y scales
  [[nodiscard]] static Matrix3 CreateScale(float xScale, float yScale) {
    float temp[3][3] = {
        {xScale, 0.0f, 0.0f},
        {0.0f, yScale, 0.0f},
        {0.0f, 0.0f, 1.0f},
    };
    return Matrix3(temp);
  }

  [[nodiscard]] static Matrix3 CreateScale(const Vector2 &scaleVector) {
    return CreateScale(scaleVector.x, scaleVector.y);
  }

  // Create a scale matrix with a uniform factor
  [[nodiscard]] static Matrix3 CreateScale(float scale) {
    return CreateScale(scale, scale);
  }

  // Create a rotation matrix about the Z axis
  // theta is in radians
  [[nodiscard]] static Matrix3 CreateRotation(float theta) {
    float temp[3][3] = {
        {Math::Cos(theta), Math::Sin(theta), 0.0f},
        {-Math::Sin(theta), Math::Cos(theta), 0.0f},
        {0.0f, 0.0f, 1.0f},
    };
    return Matrix3(temp);
  }

  // Create a translation matrix (on the xy-plane)
  [[nodiscard]] static Matrix3 CreateTranslation(const Vector2 &trans) {
    float temp[3][3] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {trans.x, trans.y, 1.0f},
    };
    return Matrix3(temp);
  }

  static const Matrix3 Identity; // NOLINT
};

// Matrix3 Identity definition
inline constexpr float g_Matrix3Identity[3][3] = {
    {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
inline const Matrix3 Matrix3::Identity =
    Matrix3(const_cast<float (*)[3]>(g_Matrix3Identity));

// 4x4 Matrix
class Matrix4 {
public:
  float mat[4][4]; // NOLINT

  // NOLINTBEGIN
  Matrix4() { *this = Matrix4::Identity; }

  explicit Matrix4(float inMat[4][4]) {
    memcpy(mat, inMat, 16 * sizeof(float));
  }
  // NOLINTEND

  // Cast to a const float pointer
  const float *GetAsFloatPtr() const {
    return reinterpret_cast<const float *>(&mat[0][0]);
  }

  // Matrix multiplication (a * b)
  [[nodiscard]] friend Matrix4 operator*(const Matrix4 &a, const Matrix4 &b) {
    Matrix4 retVal;
    // row 0
    retVal.mat[0][0] = a.mat[0][0] * b.mat[0][0] + a.mat[0][1] * b.mat[1][0] +
                       a.mat[0][2] * b.mat[2][0] + a.mat[0][3] * b.mat[3][0];

    retVal.mat[0][1] = a.mat[0][0] * b.mat[0][1] + a.mat[0][1] * b.mat[1][1] +
                       a.mat[0][2] * b.mat[2][1] + a.mat[0][3] * b.mat[3][1];

    retVal.mat[0][2] = a.mat[0][0] * b.mat[0][2] + a.mat[0][1] * b.mat[1][2] +
                       a.mat[0][2] * b.mat[2][2] + a.mat[0][3] * b.mat[3][2];

    retVal.mat[0][3] = a.mat[0][0] * b.mat[0][3] + a.mat[0][1] * b.mat[1][3] +
                       a.mat[0][2] * b.mat[2][3] + a.mat[0][3] * b.mat[3][3];

    // row 1
    retVal.mat[1][0] = a.mat[1][0] * b.mat[0][0] + a.mat[1][1] * b.mat[1][0] +
                       a.mat[1][2] * b.mat[2][0] + a.mat[1][3] * b.mat[3][0];

    retVal.mat[1][1] = a.mat[1][0] * b.mat[0][1] + a.mat[1][1] * b.mat[1][1] +
                       a.mat[1][2] * b.mat[2][1] + a.mat[1][3] * b.mat[3][1];

    retVal.mat[1][2] = a.mat[1][0] * b.mat[0][2] + a.mat[1][1] * b.mat[1][2] +
                       a.mat[1][2] * b.mat[2][2] + a.mat[1][3] * b.mat[3][2];

    retVal.mat[1][3] = a.mat[1][0] * b.mat[0][3] + a.mat[1][1] * b.mat[1][3] +
                       a.mat[1][2] * b.mat[2][3] + a.mat[1][3] * b.mat[3][3];

    // row 2
    retVal.mat[2][0] = a.mat[2][0] * b.mat[0][0] + a.mat[2][1] * b.mat[1][0] +
                       a.mat[2][2] * b.mat[2][0] + a.mat[2][3] * b.mat[3][0];

    retVal.mat[2][1] = a.mat[2][0] * b.mat[0][1] + a.mat[2][1] * b.mat[1][1] +
                       a.mat[2][2] * b.mat[2][1] + a.mat[2][3] * b.mat[3][1];

    retVal.mat[2][2] = a.mat[2][0] * b.mat[0][2] + a.mat[2][1] * b.mat[1][2] +
                       a.mat[2][2] * b.mat[2][2] + a.mat[2][3] * b.mat[3][2];

    retVal.mat[2][3] = a.mat[2][0] * b.mat[0][3] + a.mat[2][1] * b.mat[1][3] +
                       a.mat[2][2] * b.mat[2][3] + a.mat[2][3] * b.mat[3][3];

    // row 3
    retVal.mat[3][0] = a.mat[3][0] * b.mat[0][0] + a.mat[3][1] * b.mat[1][0] +
                       a.mat[3][2] * b.mat[2][0] + a.mat[3][3] * b.mat[3][0];

    retVal.mat[3][1] = a.mat[3][0] * b.mat[0][1] + a.mat[3][1] * b.mat[1][1] +
                       a.mat[3][2] * b.mat[2][1] + a.mat[3][3] * b.mat[3][1];

    retVal.mat[3][2] = a.mat[3][0] * b.mat[0][2] + a.mat[3][1] * b.mat[1][2] +
                       a.mat[3][2] * b.mat[2][2] + a.mat[3][3] * b.mat[3][2];

    retVal.mat[3][3] = a.mat[3][0] * b.mat[0][3] + a.mat[3][1] * b.mat[1][3] +
                       a.mat[3][2] * b.mat[2][3] + a.mat[3][3] * b.mat[3][3];

    return retVal;
  }

  Matrix4 &operator*=(const Matrix4 &right) {
    *this = *this * right;
    return *this;
  }

  // Invert the matrix - super slow
  void Invert() {
    // Simple identity for now - implement full inversion if needed
    *this = Matrix4::Identity;
  }

  void Transpose() {
    float temp[4][4];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        temp[i][j] = mat[j][i];
      }
    }
    memcpy(mat, temp, 16 * sizeof(float));
  }

  // Get the translation component of the matrix
  [[nodiscard]] Vector3 GetTranslation() const {
    return Vector3(mat[3][0], mat[3][1], mat[3][2]);
  }

  // Get the X axis of the matrix (forward)
  [[nodiscard]] Vector3 GetXAxis() const {
    return Vector3::Normalize(Vector3(mat[0][0], mat[0][1], mat[0][2]));
  }

  // Get the Y axis of the matrix (left)
  [[nodiscard]] Vector3 GetYAxis() const {
    return Vector3::Normalize(Vector3(mat[1][0], mat[1][1], mat[1][2]));
  }

  // Get the Z axis of the matrix (up)
  [[nodiscard]] Vector3 GetZAxis() const {
    return Vector3::Normalize(Vector3(mat[2][0], mat[2][1], mat[2][2]));
  }

  // Extract the scale component from the matrix
  [[nodiscard]] Vector3 GetScale() const {
    Vector3 retVal;
    retVal.x = Vector3(mat[0][0], mat[0][1], mat[0][2]).Length();
    retVal.y = Vector3(mat[1][0], mat[1][1], mat[1][2]).Length();
    retVal.z = Vector3(mat[2][0], mat[2][1], mat[2][2]).Length();
    return retVal;
  }

  // Create a scale matrix with x, y, and z scales
  [[nodiscard]] static Matrix4 CreateScale(float xScale, float yScale,
                                           float zScale) {
    float temp[4][4] = {{xScale, 0.0f, 0.0f, 0.0f},
                        {0.0f, yScale, 0.0f, 0.0f},
                        {0.0f, 0.0f, zScale, 0.0f},
                        {0.0f, 0.0f, 0.0f, 1.0f}};
    return Matrix4(temp);
  }

  [[nodiscard]] static Matrix4 CreateScale(const Vector3 &scaleVector) {
    return CreateScale(scaleVector.x, scaleVector.y, scaleVector.z);
  }

  // Create a scale matrix with a uniform factor
  [[nodiscard]] static Matrix4 CreateScale(float scale) {
    return CreateScale(scale, scale, scale);
  }

  // Rotation about x-axis
  [[nodiscard]] static Matrix4 CreateRotationX(float theta) {
    float temp[4][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, Math::Cos(theta), Math::Sin(theta), 0.0f},
        {0.0f, -Math::Sin(theta), Math::Cos(theta), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };
    return Matrix4(temp);
  }

  // Rotation about y-axis
  [[nodiscard]] static Matrix4 CreateRotationY(float theta) {
    float temp[4][4] = {
        {Math::Cos(theta), 0.0f, -Math::Sin(theta), 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {Math::Sin(theta), 0.0f, Math::Cos(theta), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };
    return Matrix4(temp);
  }

  // Rotation about z-axis
  [[nodiscard]] static Matrix4 CreateRotationZ(float theta) {
    float temp[4][4] = {
        {Math::Cos(theta), Math::Sin(theta), 0.0f, 0.0f},
        {-Math::Sin(theta), Math::Cos(theta), 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };
    return Matrix4(temp);
  }

  // Create a rotation matrix from a quaternion
  [[nodiscard]] static Matrix4 CreateFromQuaternion(const class Quaternion &q);

  [[nodiscard]] static Matrix4 CreateTranslation(const Vector3 &trans) {
    float temp[4][4] = {{1.0f, 0.0f, 0.0f, 0.0f},
                        {0.0f, 1.0f, 0.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f, 0.0f},
                        {trans.x, trans.y, trans.z, 1.0f}};
    return Matrix4(temp);
  }

  [[nodiscard]] static Matrix4
  CreateLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up) {
    Vector3 zaxis = Vector3::Normalize(target - eye);
    Vector3 xaxis = Vector3::Normalize(Vector3::Cross(up, zaxis));
    Vector3 yaxis = Vector3::Normalize(Vector3::Cross(zaxis, xaxis));
    Vector3 trans;
    trans.x = -Vector3::Dot(xaxis, eye);
    trans.y = -Vector3::Dot(yaxis, eye);
    trans.z = -Vector3::Dot(zaxis, eye);

    float temp[4][4] = {{xaxis.x, yaxis.x, zaxis.x, 0.0f},
                        {xaxis.y, yaxis.y, zaxis.y, 0.0f},
                        {xaxis.z, yaxis.z, zaxis.z, 0.0f},
                        {trans.x, trans.y, trans.z, 1.0f}};
    return Matrix4(temp);
  }

  [[nodiscard]] static Matrix4 CreateOrtho(float width, float height,
                                           float near, float far) {
    float temp[4][4] = {{2.0f / width, 0.0f, 0.0f, 0.0f},
                        {0.0f, 2.0f / height, 0.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f / (far - near), 0.0f},
                        {0.0f, 0.0f, near / (near - far), 1.0f}};
    return Matrix4(temp);
  }

  [[nodiscard]] static Matrix4 CreateOrtho(float left, float right,
                                           float bottom, float top, float near,
                                           float far) {
    float temp[4][4] = {{2.0f / (right - left), 0.0f, 0.0f, 0.0f},
                        {0.0f, 2.0f / (top - bottom), 0.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f / (far - near), 0.0f},
                        {(left + right) / (left - right),
                         (top + bottom) / (bottom - top), near / (near - far),
                         1.0f}};
    return Matrix4(temp);
  }

  [[nodiscard]] static Matrix4 CreatePerspectiveFOV(float fovY, float width,
                                                    float height, float near,
                                                    float far) {
    float yScale = Math::Cot(fovY / 2.0f);
    float xScale = yScale * height / width;
    float temp[4][4] = {{xScale, 0.0f, 0.0f, 0.0f},
                        {0.0f, yScale, 0.0f, 0.0f},
                        {0.0f, 0.0f, far / (far - near), 1.0f},
                        {0.0f, 0.0f, -near * far / (far - near), 0.0f}};
    return Matrix4(temp);
  }

  // Create "Simple" View-Projection Matrix from Chapter 6
  [[nodiscard]] static Matrix4 CreateSimpleViewProj(float width, float height) {
    float temp[4][4] = {{2.0f / width, 0.0f, 0.0f, 0.0f},
                        {0.0f, 2.0f / height, 0.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f, 1.0f}};
    return Matrix4(temp);
  }

  static const Matrix4 Identity; // NOLINT
};

// Matrix4 Identity definition
inline constexpr float g_Matrix4Identity[4][4] = {{1.0f, 0.0f, 0.0f, 0.0f},
                                                  {0.0f, 1.0f, 0.0f, 0.0f},
                                                  {0.0f, 0.0f, 1.0f, 0.0f},
                                                  {0.0f, 0.0f, 0.0f, 1.0f}};
inline const Matrix4 Matrix4::Identity =
    Matrix4(const_cast<float (*)[4]>(g_Matrix4Identity));

// (Unit) Quaternion
class Quaternion {
public:
  // NOLINTBEGIN
  float x;
  float y;
  float z;
  float w;

  Quaternion() { *this = Quaternion::Identity; }

  // This directly sets the quaternion components --
  // don't use for axis/angle
  explicit Quaternion(float inX, float inY, float inZ, float inW) {
    Set(inX, inY, inZ, inW);
  }
  // NOLINTEND

  // Construct the quaternion from an axis and angle
  // It is assumed that axis is already normalized,
  // and the angle is in radians
  explicit Quaternion(const Vector3 &axis, float angle) {
    float scalar = Math::Sin(angle / 2.0f);
    x = axis.x * scalar;
    y = axis.y * scalar;
    z = axis.z * scalar;
    w = Math::Cos(angle / 2.0f);
  }

  // Directly set the internal components
  void Set(float inX, float inY, float inZ, float inW) {
    x = inX;
    y = inY;
    z = inZ;
    w = inW;
  }

  void Conjugate() {
    x *= -1.0f;
    y *= -1.0f;
    z *= -1.0f;
  }

  [[nodiscard]] float LengthSq() const {
    return (x * x + y * y + z * z + w * w);
  }

  [[nodiscard]] float Length() const { return Math::Sqrt(LengthSq()); }

  void Normalize() {
    float length = Length();
    x /= length;
    y /= length;
    z /= length;
    w /= length;
  }

  // Normalize the provided quaternion
  [[nodiscard]] static Quaternion Normalize(const Quaternion &q) {
    Quaternion retVal = q;
    retVal.Normalize();
    return retVal;
  }

  // Linear interpolation
  [[nodiscard]] static Quaternion Lerp(const Quaternion &a, const Quaternion &b,
                                       float f) {
    Quaternion retVal;
    retVal.x = Math::Lerp(a.x, b.x, f);
    retVal.y = Math::Lerp(a.y, b.y, f);
    retVal.z = Math::Lerp(a.z, b.z, f);
    retVal.w = Math::Lerp(a.w, b.w, f);
    retVal.Normalize();
    return retVal;
  }

  // Dot product between two quaternions
  [[nodiscard]] static float Dot(const Quaternion &a, const Quaternion &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  }

  // Spherical Linear Interpolation
  [[nodiscard]] static Quaternion Slerp(const Quaternion &a,
                                        const Quaternion &b, float f) {
    float dot = (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);

    // If the dot product is negative, negate one quaternion to take the shorter
    // path
    Quaternion b_adjusted = b;
    if (dot < 0.0f) {
      b_adjusted.x = -b.x;
      b_adjusted.y = -b.y;
      b_adjusted.z = -b.z;
      b_adjusted.w = -b.w;
      dot = -dot;
    }

    // Can't compute this if the quaternions are collinear
    if (Math::Abs(dot) > 0.9995f) {
      return Lerp(a, b_adjusted, f);
    }

    dot = Math::Clamp(dot, -1.0f, 1.0f);
    float halfTheta = Math::Acos(dot);
    float sinHalfTheta = Math::Sqrt(1.0f - dot * dot);

    float ratioA = Math::Sin((1.0f - f) * halfTheta) / sinHalfTheta;
    float ratioB = Math::Sin(f * halfTheta) / sinHalfTheta;

    Quaternion retVal;
    retVal.x = a.x * ratioA + b_adjusted.x * ratioB;
    retVal.y = a.y * ratioA + b_adjusted.y * ratioB;
    retVal.z = a.z * ratioA + b_adjusted.z * ratioB;
    retVal.w = a.w * ratioA + b_adjusted.w * ratioB;
    return retVal;
  }

  // Concatenate
  // Rotate by q FOLLOWED BY p
  [[nodiscard]] static Quaternion Concatenate(const Quaternion &q,
                                              const Quaternion &p) {
    Quaternion retVal;

    // Vector component is:
    // ps * qv + qs * pv + pv x qv
    Vector3 qv(q.x, q.y, q.z);
    Vector3 pv(p.x, p.y, p.z);
    Vector3 newVec = p.w * qv + q.w * pv + Vector3::Cross(pv, qv);
    retVal.x = newVec.x;
    retVal.y = newVec.y;
    retVal.z = newVec.z;

    // Scalar component is:
    // ps * qs - pv . qv
    retVal.w = p.w * q.w - Vector3::Dot(pv, qv);

    return retVal;
  }

  static const Quaternion Identity; // NOLINT
};

// Quaternion Identity definition
inline const Quaternion Quaternion::Identity =
    Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

namespace Math {
[[nodiscard]] inline bool NearlyZero(float val, float epsilon = 0.001f) {
  return Abs(val) <= epsilon;
}

[[nodiscard]] inline bool NearlyEqual(float a, float b,
                                      float epsilon = 0.001f) {
  return Abs(a - b) <= epsilon;
}

[[nodiscard]] inline bool NearlyEqual(const Vector2 &a, const Vector2 &b,
                                      float epsilon = 0.001f) {
  return NearlyEqual(a.x, b.x, epsilon) && NearlyEqual(a.y, b.y, epsilon);
}

[[nodiscard]] inline bool NearlyEqual(const Vector3 &a, const Vector3 &b,
                                      float epsilon = 0.001f) {
  return NearlyEqual(a.x, b.x, epsilon) && NearlyEqual(a.y, b.y, epsilon) &&
         NearlyEqual(a.z, b.z, epsilon);
}

[[nodiscard]] inline bool NearlyEqual(const Quaternion &a, const Quaternion &b,
                                      float epsilon = 0.001f) {
  return NearlyEqual(a.x, b.x, epsilon) && NearlyEqual(a.y, b.y, epsilon) &&
         NearlyEqual(a.z, b.z, epsilon) && NearlyEqual(a.w, b.w, epsilon);
}

// Build a quaternion that rotates the object's local +Z to the given
// world-space forward direction while trying to align the object's up with
// upHint. forwardInput: desired forward (world space). upHint: preferred world
// up. localFront: the object's forward in local space (default +Z). If your
// model uses -Z, pass (0,0,-1).
[[nodiscard]] inline Quaternion
LookRotation(const Vector3 &forwardInput,
             const Vector3 &upHint = Vector3(0.0f, 1.0f, 0.0f),
             const Vector3 &localFront = Vector3(0.0f, 0.0f, 1.0f)) {
  Vector3 f = forwardInput;
  if (f.LengthSq() <= 1e-8f)
    return Quaternion::Identity;
  f.Normalize();

  Vector3 up = upHint;
  Vector3 right = Vector3::Cross(up, f);
  if (right.LengthSq() <= 1e-8f) {
    // forward nearly parallel to upHint — pick another up
    up = Math::Abs(f.y) < 0.999f ? Vector3(0.0f, 1.0f, 0.0f)
                                 : Vector3(1.0f, 0.0f, 0.0f);
    right = Vector3::Cross(up, f);
    if (right.LengthSq() <= 1e-8f)
      return Quaternion::Identity;
  }
  right.Normalize();
  Vector3 up2 = Vector3::Cross(f, right);

  float matArr[4][4] = {{0}};
  // Column 0 = right
  matArr[0][0] = right.x;
  matArr[0][1] = right.y;
  matArr[0][2] = right.z;
  matArr[0][3] = 0.0f;
  // Column 1 = up2
  matArr[1][0] = up2.x;
  matArr[1][1] = up2.y;
  matArr[1][2] = up2.z;
  matArr[1][3] = 0.0f;
  // Column 2 = forward
  matArr[2][0] = f.x;
  matArr[2][1] = f.y;
  matArr[2][2] = f.z;
  matArr[2][3] = 0.0f;
  // Column 3 = translation/identity
  matArr[3][0] = 0.0f;
  matArr[3][1] = 0.0f;
  matArr[3][2] = 0.0f;
  matArr[3][3] = 1.0f;
  Matrix4 rotMat(matArr);

  // Convert rotation matrix to quaternion. Note Matrix4 stores columns in
  // mat[col][row].
  float r00 = rotMat.mat[0][0], r01 = rotMat.mat[1][0], r02 = rotMat.mat[2][0];
  float r10 = rotMat.mat[0][1], r11 = rotMat.mat[1][1], r12 = rotMat.mat[2][1];
  float r20 = rotMat.mat[0][2], r21 = rotMat.mat[1][2], r22 = rotMat.mat[2][2];

  Quaternion q;
  float trace = r00 + r11 + r22;
  if (trace > 0.0f) {
    float s = 0.5f / Math::Sqrt(trace + 1.0f);
    q.w = 0.25f / s;
    q.x = (r21 - r12) * s;
    q.y = (r02 - r20) * s;
    q.z = (r10 - r01) * s;
  } else if (r00 > r11 && r00 > r22) {
    float s = 2.0f * Math::Sqrt(1.0f + r00 - r11 - r22);
    q.w = (r21 - r12) / s;
    q.x = 0.25f * s;
    q.y = (r01 + r10) / s;
    q.z = (r02 + r20) / s;
  } else if (r11 > r22) {
    float s = 2.0f * Math::Sqrt(1.0f + r11 - r00 - r22);
    q.w = (r02 - r20) / s;
    q.x = (r01 + r10) / s;
    q.y = 0.25f * s;
    q.z = (r12 + r21) / s;
  } else {
    float s = 2.0f * Math::Sqrt(1.0f + r22 - r00 - r11);
    q.w = (r10 - r01) / s;
    q.x = (r02 + r20) / s;
    q.y = (r12 + r21) / s;
    q.z = 0.25f * s;
  }

  q.Normalize();

  // If object's local front isn't +Z, rotate localFront to +Z first and
  // concatenate
  if (!(Math::NearlyEqual(localFront.x, 0.0f) &&
        Math::NearlyEqual(localFront.y, 0.0f) &&
        Math::NearlyEqual(localFront.z, 1.0f))) {
    Vector3 lf = localFront;
    lf.Normalize();
    Vector3 targetLocalFront = Vector3(0.0f, 0.0f, 1.0f);
    float dot = Math::Clamp(Vector3::Dot(lf, targetLocalFront), -1.0f, 1.0f);
    float angle = Math::Acos(dot);
    if (Math::Abs(angle) > 1e-4f) {
      Vector3 axis = Vector3::Cross(lf, targetLocalFront);
      if (axis.LengthSq() > 1e-8f) {
        axis.Normalize();
        Quaternion qLocal(axis, angle);
        // Rotate by qLocal THEN by q
        q = Quaternion::Concatenate(qLocal, q);
      }
    }
  }

  q.Normalize();
  return q;
}

} // namespace Math

namespace Color {
// NOLINTBEGIN
constexpr Vector3 Black(0.0f, 0.0f, 0.0f);
constexpr Vector3 White(1.0f, 1.0f, 1.0f);
constexpr Vector3 Red(1.0f, 0.0f, 0.0f);
constexpr Vector3 Green(0.0f, 1.0f, 0.0f);
constexpr Vector3 Blue(0.0f, 0.0f, 1.0f);
constexpr Vector3 Yellow(1.0f, 1.0f, 0.0f);
constexpr Vector3 Orange(1.0f, 0.4f, 0.0f);
constexpr Vector3 Magenta(1.0f, 0.0f, 0.5f);
constexpr Vector3 Cyan(0.0f, 1.0f, 1.0f);
constexpr Vector3 Purple(0.5f, 0.0f, 1.0f);
constexpr Vector3 LightYellow(1.0f, 1.0f, 0.88f);
constexpr Vector3 LightBlue(0.68f, 0.85f, 0.9f);
constexpr Vector3 LightPink(1.0f, 0.71f, 0.76f);
constexpr Vector3 LightGreen(0.56f, 0.93f, 0.56f);
// NOLINTEND
} // namespace Color

// Vector2 Transform implementation
inline Vector2 Vector2::Transform(const Vector2 &vec, const Matrix3 &mat,
                                  float w) {
  Vector2 retVal;
  retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] + w * mat.mat[2][0];
  retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] + w * mat.mat[2][1];
  return retVal;
}

// Vector3 Transform implementations
inline Vector3 Vector3::Transform(const Vector3 &vec, const Matrix4 &mat,
                                  float w) {
  Vector3 retVal;
  retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
             vec.z * mat.mat[2][0] + w * mat.mat[3][0];
  retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
             vec.z * mat.mat[2][1] + w * mat.mat[3][1];
  retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
             vec.z * mat.mat[2][2] + w * mat.mat[3][2];
  return retVal;
}

inline Vector3 Vector3::TransformWithPerspDiv(const Vector3 &vec,
                                              const Matrix4 &mat, float w) {
  Vector3 retVal;
  retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
             vec.z * mat.mat[2][0] + w * mat.mat[3][0];
  retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
             vec.z * mat.mat[2][1] + w * mat.mat[3][1];
  retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
             vec.z * mat.mat[2][2] + w * mat.mat[3][2];
  float transformedW = vec.x * mat.mat[0][3] + vec.y * mat.mat[1][3] +
                       vec.z * mat.mat[2][3] + w * mat.mat[3][3];
  if (!Math::NearlyZero(Math::Abs(transformedW))) {
    transformedW = 1.0f / transformedW;
    retVal *= transformedW;
  }
  return retVal;
}

inline Vector3 Vector3::Transform(const Vector3 &v, const Quaternion &q) {
  // v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
  Vector3 qv(q.x, q.y, q.z);
  Vector3 retVal = v;
  retVal = retVal + 2.0f * Vector3::Cross(qv, Vector3::Cross(qv, v) + q.w * v);
  return retVal;
}

// Vector4 Transform implementation
inline Vector4 Vector4::Transform(const Vector4 &vec, const Matrix4 &mat) {
  Vector4 retVal;
  retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
             vec.z * mat.mat[2][0] + vec.w * mat.mat[3][0];
  retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
             vec.z * mat.mat[2][1] + vec.w * mat.mat[3][1];
  retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
             vec.z * mat.mat[2][2] + vec.w * mat.mat[3][2];
  retVal.w = vec.x * mat.mat[0][3] + vec.y * mat.mat[1][3] +
             vec.z * mat.mat[2][3] + vec.w * mat.mat[3][3];
  return retVal;
}

// Matrix4 CreateFromQuaternion implementation
inline Matrix4 Matrix4::CreateFromQuaternion(const Quaternion &q) {
  float mat[4][4];

  mat[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
  mat[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
  mat[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
  mat[0][3] = 0.0f;

  mat[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
  mat[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
  mat[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
  mat[1][3] = 0.0f;

  mat[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
  mat[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
  mat[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
  mat[2][3] = 0.0f;

  mat[3][0] = 0.0f;
  mat[3][1] = 0.0f;
  mat[3][2] = 0.0f;
  mat[3][3] = 1.0f;

  return Matrix4(mat);
}

#endif // MATH_HPP