#include "render/Mesh.hpp"
#include <iostream>
#include <fstream>


// ============== Base Mesh Class ==============

Mesh::Mesh()
    : mVertexArray(0)
    , mVertexBuffer(0)
    , mIndexBuffer(0)
    , mInstanceBuffer(0)
    , mNumVerts(0)
    , mNumIndices(0)
    , mMaxInstances(0)
{
}

Mesh::~Mesh()
{
    if (mVertexBuffer != 0) {
        glDeleteBuffers(1, &mVertexBuffer);
        mVertexBuffer = 0;
    }
    if (mIndexBuffer != 0) {
        glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
    }
    if (mInstanceBuffer != 0) {
        glDeleteBuffers(1, &mInstanceBuffer);
        mInstanceBuffer = 0;
    }
    if (mVertexArray != 0) {
        glDeleteVertexArrays(1, &mVertexArray);
        mVertexArray = 0;
    }
}

void Mesh::Build(MeshData meshdata)
{
    // Clean up existing buffers if any
    if (mVertexBuffer != 0) {
        glDeleteBuffers(1, &mVertexBuffer);
        mVertexBuffer = 0;
    }
    if (mIndexBuffer != 0) {
        glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
    }
    if (mVertexArray != 0) {
        glDeleteVertexArrays(1, &mVertexArray);
        mVertexArray = 0;
    }
    
    // Store triangles (for texture mapping)
    mTriangles = meshdata.triangles;

    // Convert vertices to flat array format: [x, y, z, nx, ny, nz, u, v, texIndex]
    // We'll set texture index per-vertex based on the first triangle that uses it
    std::vector<float> vertexData;
    vertexData.reserve(meshdata.vertices.size() * 9);

    // Create a map to track which texture index each vertex should use
    // For shared vertices, we'll use the texture from the first triangle that references it
    std::vector<int> vertexTextureIndices(meshdata.vertices.size(), -1);

    for (const auto& triangle : meshdata.triangles) {
        for (int i = 0; i < 3; ++i) {
            unsigned int vIdx = triangle.indices[i];
            if (vertexTextureIndices[vIdx] == -1) {
                vertexTextureIndices[vIdx] = triangle.textureIndex;
            }
        }
    }
    
    // Now build the vertex data
    for (size_t i = 0; i < meshdata.vertices.size(); ++i) {
        const Vertex& vertex = meshdata.vertices[i];

        // Position
        vertexData.push_back(vertex.position.x);
        vertexData.push_back(vertex.position.y);
        vertexData.push_back(vertex.position.z);
        
        // Normal
        vertexData.push_back(vertex.normal.x);
        vertexData.push_back(vertex.normal.y);
        vertexData.push_back(vertex.normal.z);
        
        // Texture coordinates
        vertexData.push_back(vertex.texCoord.x);
        vertexData.push_back(vertex.texCoord.y);
        
        // Texture index (from the triangle that uses this vertex)
        vertexData.push_back(static_cast<float>(vertexTextureIndices[i]));
    }
    
    // Convert triangles to flat index array
    std::vector<unsigned int> indexData;
    indexData.reserve(meshdata.triangles.size() * 3);
    
    for (const auto& triangle : meshdata.triangles) {
        indexData.push_back(triangle.indices[0]);
        indexData.push_back(triangle.indices[1]);
        indexData.push_back(triangle.indices[2]);
    }
    
    mNumVerts = static_cast<unsigned int>(meshdata.vertices.size());
    mNumIndices = static_cast<unsigned int>(indexData.size());
    
    // Create vertex array object
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);
    
    // Create and populate vertex buffer
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    
    // Create and populate index buffer
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);
    
    // Set up vertex attributes (9 floats per vertex with texture index)
    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    
    // Normal attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Texture coordinate attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    
    // Texture index attribute (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    
    // Unbind VAO
    glBindVertexArray(0);

    std::cout << "Mesh built with " << meshdata.vertices.size() << " vertices and "
              << meshdata.triangles.size() << " triangles" << std::endl;
}

void Mesh::SetActive() const
{
    glBindVertexArray(mVertexArray);
}

bool Mesh::LoadFromFile(const std::string& filename)
{
    // Base implementation - to be overridden by derived classes
    std::cerr << "LoadFromFile not implemented for base Mesh class: " << filename << std::endl;
    return false;
}

// ============== CubeMesh Class ==============

CubeMesh::CubeMesh()
{
    MeshData data;
    
    // Tile indices for each face
    int tile0 = 0;  // Front
    int tile1 = 0;  // Back
    int tile2 = 0;  // Right
    int tile3 = 0;  // Left
    int tile4 = 1;  // Top
    int tile5 = 1;  // Bottom
    
    // Front face (Z+)
    {
        Vector3 normal(0.0f, 0.0f, 1.0f);
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, 0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, -0.5f, 0.5f), normal, Vector2(0.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, 0.5f, 0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, 0.5f, 0.5f), normal, Vector2(1.0f, 0.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, tile0));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, tile0));  // CCW
    }
    
    // Back face (Z-)
    {
        Vector3 normal(0.0f, 0.0f, -1.0f);
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(0.5f, -0.5f, -0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, -0.5f), normal, Vector2(0.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, 0.5f, -0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, 0.5f, -0.5f), normal, Vector2(1.0f, 0.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, tile1));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, tile1));  // CCW
    }
    
    // Right face (X+)
    {
        Vector3 normal(1.0f, 0.0f, 0.0f);
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(0.5f, -0.5f, 0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, -0.5f, -0.5f), normal, Vector2(0.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, 0.5f, -0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, 0.5f, 0.5f), normal, Vector2(1.0f, 0.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, tile2));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, tile2));  // CCW
    }
    
    // Left face (X-)
    {
        Vector3 normal(-1.0f, 0.0f, 0.0f);
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, -0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, 0.5f), normal, Vector2(0.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, 0.5f, 0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, 0.5f, -0.5f), normal, Vector2(1.0f, 0.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, tile3));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, tile3));  // CCW
    }
    
    // Top face (Y+)
    {
        Vector3 normal(0.0f, 1.0f, 0.0f);
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(-0.5f, 0.5f, 0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, 0.5f, 0.5f), normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, 0.5f, -0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, 0.5f, -0.5f), normal, Vector2(0.0f, 1.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, tile4));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, tile4));  // CCW
    }
    
    // Bottom face (Y-)
    {
        Vector3 normal(0.0f, -1.0f, 0.0f);
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, -0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, -0.5f, -0.5f), normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, -0.5f, 0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, 0.5f), normal, Vector2(0.0f, 1.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, tile5));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, tile5));  // CCW
    }

    Build(data);
}


// ============== PyramidMesh Class ==============

PyramidMesh::PyramidMesh()
{
    MeshData data;
    
    int brickTileIndex = 0;
    int checkerTileIndex = 1;
    
    Vector3 apex(0.0f, 0.5f, 0.0f);
    float baseHeight = -0.5f;
    
    // Front face (Z+)
    {
        Vector3 v0(-0.5f, baseHeight, 0.5f);
        Vector3 v1(0.5f, baseHeight, 0.5f);
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = apex - v0;
        Vector3 normal = Vector3::Cross(edge1, edge2);
        normal.Normalize();
        
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(v0, normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(v1, normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(apex, normal, Vector2(0.5f, 1.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, brickTileIndex));  // CCW
    }
    
    // Right face (X+)
    {
        Vector3 v0(0.5f, baseHeight, 0.5f);
        Vector3 v1(0.5f, baseHeight, -0.5f);
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = apex - v0;
        Vector3 normal = Vector3::Cross(edge1, edge2);
        normal.Normalize();
        
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(v0, normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(v1, normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(apex, normal, Vector2(0.5f, 1.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, brickTileIndex));  // CCW
    }
    
    // Back face (Z-)
    {
        Vector3 v0(0.5f, baseHeight, -0.5f);
        Vector3 v1(-0.5f, baseHeight, -0.5f);
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = apex - v0;
        Vector3 normal = Vector3::Cross(edge1, edge2);
        normal.Normalize();
        
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(v0, normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(v1, normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(apex, normal, Vector2(0.5f, 1.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, brickTileIndex));  // CCW
    }
    
    // Left face (X-)
    {
        Vector3 v0(-0.5f, baseHeight, -0.5f);
        Vector3 v1(-0.5f, baseHeight, 0.5f);
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = apex - v0;
        Vector3 normal = Vector3::Cross(edge1, edge2);
        normal.Normalize();
        
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(v0, normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(v1, normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(apex, normal, Vector2(0.5f, 1.0f)));
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, brickTileIndex));  // CCW
    }
    
    // Bottom face (Y-)
    {
        Vector3 normal(0.0f, -1.0f, 0.0f);
        
        unsigned int baseIdx = data.vertices.size();
        data.vertices.push_back(Vertex(Vector3(-0.5f, baseHeight, -0.5f), normal, Vector2(0.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, baseHeight, -0.5f), normal, Vector2(1.0f, 0.0f)));
        data.vertices.push_back(Vertex(Vector3(0.5f, baseHeight, 0.5f), normal, Vector2(1.0f, 1.0f)));
        data.vertices.push_back(Vertex(Vector3(-0.5f, baseHeight, 0.5f), normal, Vector2(0.0f, 1.0f)));
        
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+2, baseIdx+1, checkerTileIndex));  // CCW
        data.triangles.push_back(Triangle(baseIdx+0, baseIdx+3, baseIdx+2, checkerTileIndex));  // CCW
    }
    Build(data);
}

void Mesh::SetupInstanceBuffer(size_t maxInstances)
{
    mMaxInstances = maxInstances;
    
    if (mInstanceBuffer != 0) {
        glDeleteBuffers(1, &mInstanceBuffer);
    }
    
    // Bind VAO to modify it
    glBindVertexArray(mVertexArray);
    
    // Create instance buffer
    glGenBuffers(1, &mInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
    
    // Allocate buffer (36 floats per instance: 16 + 16 + 3 + 1)
    glBufferData(GL_ARRAY_BUFFER, maxInstances * 36 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    
    // Setup instance attribute pointers
    // Model matrix (mat4) - locations 4, 5, 6, 7
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, 36 * sizeof(float), 
                              (void*)(i * 4 * sizeof(float)));
        glVertexAttribDivisor(4 + i, 1);  // Advance once per instance
    }
    
    // Normal matrix (mat4) - locations 8, 9, 10, 11
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(8 + i);
        glVertexAttribPointer(8 + i, 4, GL_FLOAT, GL_FALSE, 36 * sizeof(float), 
                              (void*)((16 + i * 4) * sizeof(float)));
        glVertexAttribDivisor(8 + i, 1);  // Advance once per instance
    }
    
    // Color (vec3) - location 12
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, 36 * sizeof(float), 
                          (void*)(32 * sizeof(float)));
    glVertexAttribDivisor(12, 1);  // Advance once per instance
    
    // Tile index (float) - location 13
    glEnableVertexAttribArray(13);
    glVertexAttribPointer(13, 1, GL_FLOAT, GL_FALSE, 36 * sizeof(float), 
                          (void*)(35 * sizeof(float)));
    glVertexAttribDivisor(13, 1);  // Advance once per instance
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::UpdateInstanceBuffer(const std::vector<float>& instanceData, size_t instanceCount)
{
    if (mInstanceBuffer == 0) {
        std::cerr << "Instance buffer not set up. Call SetupInstanceBuffer first." << std::endl;
        return;
    }
    
    if (instanceCount > mMaxInstances) {
        std::cerr << "Instance count (" << instanceCount << ") exceeds max instances (" << mMaxInstances << ")" << std::endl;
        return;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(float), instanceData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
