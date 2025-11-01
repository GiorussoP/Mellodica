#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <map>
#include <GL/glew.h>
#include "Math.hpp"

// Vertex structure with geometric attributes only
struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
    
    Vertex(const Vector3& pos, const Vector3& norm, const Vector2& tex)
        : position(pos), normal(norm), texCoord(tex) {}
};

// Triangle with texture index (per-face)
struct Triangle {
    unsigned int indices[3];  // Three vertex indices
    int textureIndex;         // Texture tile index for this triangle/face
    
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
    virtual bool LoadFromFile(const std::string& filename);
    
    // Activate this mesh for rendering
    void SetActive() const;
    
    // Get rendering info
    unsigned int GetNumIndices() const { return mNumIndices; }
    unsigned int GetNumVerts() const { return mNumVerts; }
    
    // Get number of triangles
    size_t GetTriangleCount() const { return mTriangles.size(); }

protected:

    // OpenGL buffer objects
    unsigned int mVertexArray;
    unsigned int mVertexBuffer;
    unsigned int mIndexBuffer;
    
    // Mesh info
    unsigned int mNumVerts;
    unsigned int mNumIndices;
    std::vector<Triangle> mTriangles;
};

// Cube mesh class
class CubeMesh : public Mesh {
public:
    CubeMesh();
    virtual ~CubeMesh() = default;

private:
    static const std::string CACHE_KEY;
    static bool sDataGenerated;
};

// Pyramid mesh class
class PyramidMesh : public Mesh {
public:
    PyramidMesh();
    virtual ~PyramidMesh() = default;

private:
    static const std::string CACHE_KEY;
    static bool sDataGenerated;
    
    void GeneratePyramidData();
};

#endif // MESH_HPP
