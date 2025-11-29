#ifndef MESH_HPP
#define MESH_HPP

#include "Math.hpp"
#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>

// Vertex structure with geometric attributes only
struct Vertex {
  Vector3 position;
  Vector3 normal;
  Vector2 texCoord;

  Vertex(const Vector3 &pos, const Vector3 &norm, const Vector2 &tex)
      : position(pos), normal(norm), texCoord(tex) {}
};

// Triangle with texture index (per-face)
struct Triangle {
  unsigned int indices[3]; // Three vertex indices
  int textureIndex;        // Texture tile index for this triangle/face

  Triangle(unsigned int i0, unsigned int i1, unsigned int i2, int texIndex = 0)
      : indices{i0, i1, i2}, textureIndex(texIndex) {}
};

// Mesh data that can be cached and shared
struct MeshData {
  std::vector<Vertex> vertices;
  std::vector<Triangle> triangles;
};

class Mesh {
public:
  Mesh();
  virtual ~Mesh();

  // Build the mesh from vertices and triangles
  void Build(const MeshData meshdata);

  // Load mesh from file (to be implemented by derived classes)
  virtual bool LoadFromFile(const std::string &filename);

  // Activate this mesh for rendering
  void SetActive() const;

  // Setup instance buffer with transform data
  void SetupInstanceBuffer(size_t maxInstances);

  // Update instance buffer with new data
  // Each instance needs: mat4 model (16 floats), mat4 normal (16 floats), vec3
  // color (3 floats), float tileIndex (1 float) = 36 floats
  void UpdateInstanceBuffer(const std::vector<float> &instanceData,
                            size_t instanceCount);

  // Get rendering info
  unsigned int GetNumIndices() const { return mNumIndices; }
  unsigned int GetNumVerts() const { return mNumVerts; }
  size_t GetMaxInstances() const { return mMaxInstances; }

  // Get number of triangles
  size_t GetTriangleCount() const { return mTriangles.size(); }

protected:
  // OpenGL buffer objects
  unsigned int mVertexArray;
  unsigned int mVertexBuffer;
  unsigned int mIndexBuffer;
  unsigned int mInstanceBuffer;

  // Mesh info
  unsigned int mNumVerts;
  unsigned int mNumIndices;
  std::vector<Triangle> mTriangles;

  size_t mMaxInstances; // Maximum number of instances
};

// Cube mesh class
class CubeMesh : public Mesh {
public:
  CubeMesh();
  virtual ~CubeMesh() = default;

private:
  static MeshData GenerateCubeData();
};

// Plane mesh class
class PlaneMesh : public Mesh {
public:
  PlaneMesh();
  virtual ~PlaneMesh() = default;

private:
  static MeshData GeneratePlaneData();

private:
  static MeshData GenerateCubeData();
};

// Pyramid mesh class
class PyramidMesh : public Mesh {
public:
  PyramidMesh();
  virtual ~PyramidMesh() = default;

private:
  static MeshData GeneratePyramidData();
};

// Sphere mesh class (icosahedron)
class SphereMesh : public Mesh {
public:
  SphereMesh();
  virtual ~SphereMesh() = default;

private:
  static MeshData GenerateIcosahedronData();
};

// Wall mesh class (tall cube with vertical texture strips)
class WallMesh : public Mesh {
public:
  WallMesh();
  virtual ~WallMesh() = default;

private:
  static MeshData GenerateWallData();
};

#endif // MESH_HPP