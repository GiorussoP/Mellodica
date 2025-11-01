#include <GL/glew.h>
#include <iostream>
#include "render/Renderer.hpp"
#include "render/Shader.hpp"
#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"
#include "components/MeshComponent.hpp"

Renderer::Renderer()
: mViewMatrix(Matrix4::Identity)
, mProjectionMatrix(Matrix4::Identity)
, mMeshShader(nullptr)
, mSpriteShader(nullptr)
, mSpriteQuad(nullptr)
, mCameraPosition(Vector3::Zero)
{

}

Renderer::~Renderer()
{
    // Delete shaders
    if (mMeshShader) {
        delete mMeshShader;
        mMeshShader = nullptr;
    }
    if (mSpriteShader) {
        delete mSpriteShader;
        mSpriteShader = nullptr;
    }
    
    // Delete sprite quad
    if (mSpriteQuad) {
        delete mSpriteQuad;
        mSpriteQuad = nullptr;
    }
    
    // Delete all textures
    for (auto* texture : mTextures) {
        delete texture;
    }
    mTextures.clear();
}

bool Renderer::Initialize(float width, float height)
{
	// Make sure we can create/compile shaders
	if (!LoadShaders()) {
		std::cerr << "Failed to load shaders." << std::endl;
		return false;
	}
	
	// Create sprite quad for simple sprite rendering
	CreateSpriteQuad();

    // Set the clear color to light grey
    glClearColor(0.419f, 0.549f, 1.0f, 1.0f);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);           // Cull back faces
    glFrontFace(GL_CCW);           // Counter-clockwise winding is front-facing

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set default projection matrix (orthographic)
    float orthoSize = 5.0f;  // Size of the orthographic view (half-height)
    float aspectRatio = width / height;
    mProjectionMatrix = Matrix4::CreateOrtho(-orthoSize * aspectRatio, orthoSize * aspectRatio,
                                              -orthoSize, orthoSize, 0.1f, 1000.0f);
    
    std::cout << "Orthographic projection created: size=" << orthoSize 
              << ", width=" << (orthoSize * aspectRatio * 2.0f)
              << ", height=" << (orthoSize * 2.0f) << std::endl;

    // Activate mesh shader
    if (mMeshShader) {
        mMeshShader->SetActive();
    }

    return true;
}

void Renderer::Shutdown()
{
    // Unload shaders
    if (mMeshShader) {
        mMeshShader->Unload();
    }
    if (mSpriteShader) {
        mSpriteShader->Unload();
    }
    
    // Unload all textures
    for (auto* texture : mTextures) {
        if (texture) {
            texture->Unload();
        }
    }
    
    // Clear the texture cache
    mTextureCache.clear();
    
    // Delete all cached meshes
    for (auto& pair : mMeshCache) {
        delete pair.second;
    }
    mMeshCache.clear();
    
    // Delete all cached atlases
    for (auto& pair : mAtlasCache) {
        delete pair.second;
    }
    mAtlasCache.clear();
    
    // Delete sprite quad
    if (mSpriteQuad) {
        delete mSpriteQuad;
        mSpriteQuad = nullptr;
    }
}

void Renderer::Clear()
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawMesh(MeshComponent& mesh, const Vector3 &position, const Vector3 &size, const Quaternion &rotation,RendererMode mode)
{
    
    
    
    // Frustum culling - skip drawing if object is outside view frustum
    if (!IsInFrustum(position, size)) {
        return;
    }
    
    // Create model matrix: Scale * Rotation * Translation
    Matrix4 model = Matrix4::CreateScale(size) *
                    Matrix4::CreateFromQuaternion(rotation) *
                    Matrix4::CreateTranslation(position);
    

    if (!mMeshShader) {
        std::cerr << "Mesh shader not loaded" << std::endl;
        return;
    }
    mMeshShader->SetActive();
    
    // Apply view matrix
    Matrix4 modelView = model * mViewMatrix;
    
    // Combine with projection matrix
    Matrix4 mvp = modelView * mProjectionMatrix;
    
    // Normal matrix uses object rotation
    Matrix4 normalMatrix = Matrix4::CreateFromQuaternion(rotation);
    
    mMeshShader->SetMatrixUniform("uWorldTransform", mvp);
    mMeshShader->SetMatrixUniform("uModelMatrix", model);
    mMeshShader->SetMatrixUniform("uNormalMatrix", normalMatrix);

    mMeshShader->SetVectorUniform("uColor", mesh.GetColor());
    

    Vector3 lightdir = Vector3(1.0f, -1.0f, 1.0f);
    lightdir.Normalize();
    mMeshShader->SetVectorUniform("uDirectionalLightDir", lightdir);

    mMeshShader->SetVectorUniform("uDirectionalLightColor", Vector3(1.0f, 1.0f, 1.0f));
    mMeshShader->SetVectorUniform("uAmbientLightColor", Vector3(0.2f, 0.2f, 0.2f));
 

    
    // Bind atlas texture and set atlas-specific uniforms
    if (mesh.GetStartingIndex() >= 0 && mesh.GetTextureAtlas()->GetTextureIndex() < static_cast<int>(mTextures.size())) {

        
        mTextures[mesh.GetTextureAtlas()->GetTextureIndex()]->Bind(0);

        mMeshShader->SetIntegerUniform("uTileIndex", mesh.GetStartingIndex());
        mMeshShader->SetIntegerUniform("uTextureAtlas", 0);
        mMeshShader->SetIntegerUniform("uAtlasColumns", mesh.GetTextureAtlas()->GetColumns());
        mMeshShader->SetVectorUniform("uAtlasTileSize", Vector2(mesh.GetTextureAtlas()->GetUVTileSizeX(), mesh.GetTextureAtlas()->GetUVTileSizeY()));
    }

    mMeshShader->SetIntegerUniform("uIsSprite", 0); // Indicate this is not a sprite
    


    
    mesh.GetMesh().SetActive();
    
    if (mode == RendererMode::LINES) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mesh.GetMesh().GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (mode == RendererMode::TRIANGLES) {
        glDrawElements(GL_TRIANGLES, mesh.GetMesh().GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

void Renderer::SetViewMatrix(const Matrix4 &view)
{
    mViewMatrix = view;
}

void Renderer::SetProjectionMatrix(const Matrix4 &projection)
{
    mProjectionMatrix = projection;
}

void Renderer::Present()
{
	// Buffer swapping is now handled by SDL in main.cpp
	// This function kept for API compatibility but does nothing
}

bool Renderer::LoadShaders()
{
	// Create mesh shader
	mMeshShader = new Shader();
	if (!mMeshShader->Load("./assets/shaders/Base.vert", "./assets/shaders/Base.frag")) {
		delete mMeshShader;
		mMeshShader = nullptr;
		return false;
	}

	// Create sprite shader (using the same shaders for now, can be changed later)
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("./assets/shaders/Base.vert", "./assets/shaders/Base.frag")) {
		delete mSpriteShader;
		mSpriteShader = nullptr;
		return false;
	}

    return true;
}

Texture* Renderer::LoadTexture(const std::string& fileName)
{
    // Check if texture is already cached
    auto it = mTextureCache.find(fileName);
    if (it != mTextureCache.end()) {
        return it->second;
    }
    
    // Texture not found, load it
    Texture* texture = new Texture();
    if (!texture->Load(fileName)) {
        delete texture;
        std::cerr << "Failed to load texture: " << fileName << std::endl;
        return nullptr;
    }
    
    // Add to cache and vector
    mTextureCache[fileName] = texture;
    mTextures.push_back(texture);
    return texture;
}


int Renderer::GetTextureIndex(Texture* texture) const
{
    for (size_t i = 0; i < mTextures.size(); i++) {
        if (mTextures[i] == texture) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

Mesh* Renderer::LoadMesh(const std::string& meshName)
{
    // Check if mesh is already cached
    auto it = mMeshCache.find(meshName);
    if (it != mMeshCache.end()) {
        return it->second;
    }
    
    // Create new mesh based on name
    Mesh* mesh = nullptr;
    if (meshName == "cube") {
        mesh = new CubeMesh();
    } else if (meshName == "pyramid") {
        mesh = new PyramidMesh();
    }
    
    if (mesh) {
        mMeshCache[meshName] = mesh;
        std::cout << "Cached mesh: " << meshName << std::endl;
    }
    
    return mesh;
}

TextureAtlas* Renderer::LoadAtlas(const std::string& atlasPath)
{
    // Check if atlas is already cached
    auto it = mAtlasCache.find(atlasPath);
    if (it != mAtlasCache.end()) {
        return it->second;
    }
    
    // Load new atlas
    TextureAtlas* atlas = new TextureAtlas(mAtlasCache.size());
    if (atlas->Load(atlasPath)) {
        mAtlasCache[atlasPath] = atlas;
        std::cout << "Cached atlas: " << atlasPath << std::endl;
        return atlas;
    }

    
    
    // Failed to load
    delete atlas;
    return nullptr;
}

bool Renderer::IsInFrustum(const Vector3 &position, const Vector3 &size)
{
    // Transform position to view space
    Vector3 viewPos = Vector3::Transform(position, mViewMatrix, 1.0f);
    
    // Calculate bounding sphere radius (conservative estimate)
    float radius = Math::Max(Math::Max(size.x, size.y), size.z) * 0.866f; // sqrt(3)/2 for cube diagonal
    
    // Extract orthographic frustum bounds from projection matrix
    // For orthographic projection: CreateOrtho(left, right, bottom, top, near, far)
    // Projection matrix structure for ortho:
    // [2/(r-l)    0         0        -(r+l)/(r-l)]
    // [0        2/(t-b)     0        -(t+b)/(t-b)]
    // [0          0      -2/(f-n)    -(f+n)/(f-n)]
    // [0          0         0             1      ]
    
    float right = 1.0f / mProjectionMatrix.mat[0][0];
    float top = 1.0f / mProjectionMatrix.mat[1][1];
    float far = -(mProjectionMatrix.mat[2][3] - 1.0f) / mProjectionMatrix.mat[2][2];
    float near = -(mProjectionMatrix.mat[2][3] + 1.0f) / mProjectionMatrix.mat[2][2];
    
        // Check if object (with radius) is within orthographic bounds in view space
    if (viewPos.x + radius < -right || viewPos.x - radius > right) return false;
    if (viewPos.y + radius < -top || viewPos.y - radius > top) return false;
    if (viewPos.z + radius < near || viewPos.z - radius > far) return false;
    
    return true;
}

void Renderer::DrawTexture(const Vector3 &position, const Vector2 &size, const Vector3 &color,
                           int textureIndex, int tileIndex, int atlasColumns,
                           float atlasTileSizeX, float atlasTileSizeY,
                           float tileOffsetX, float tileOffsetY, bool applyLighting)
{
    if (!mSpriteQuad) {
        std::cerr << "Sprite quad not created" << std::endl;
        return;
    }
    
    if (!mSpriteShader) {
        std::cerr << "Sprite shader not loaded" << std::endl;
        return;
    }
    
    // Create model matrix for billboard sprite (no rotation, scale + translation)
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateTranslation(position);
    
    mSpriteShader->SetActive();
    
    // Apply view matrix
    Matrix4 modelView = model * mViewMatrix;
    
    // Strip rotation from modelView matrix for billboard effect
    modelView.mat[0][0] = size.x;
    modelView.mat[0][1] = 0.0f;
    modelView.mat[0][2] = 0.0f;
    
    modelView.mat[1][0] = 0.0f;
    modelView.mat[1][1] = size.y;
    modelView.mat[1][2] = 0.0f;
    
    modelView.mat[2][0] = 0.0f;
    modelView.mat[2][1] = 0.0f;
    modelView.mat[2][2] = 1.0f;
    
    // Combine with projection matrix
    Matrix4 mvp = modelView * mProjectionMatrix;
    
    // Extract camera rotation from view matrix for sprite normals
    Matrix4 normalMatrix = Matrix4::Identity;
    normalMatrix.mat[0][0] = mViewMatrix.mat[0][0];
    normalMatrix.mat[0][1] = mViewMatrix.mat[1][0];
    normalMatrix.mat[0][2] = mViewMatrix.mat[2][0];
    
    normalMatrix.mat[1][0] = mViewMatrix.mat[0][1];
    normalMatrix.mat[1][1] = mViewMatrix.mat[1][1];
    normalMatrix.mat[1][2] = mViewMatrix.mat[2][1];
    
    normalMatrix.mat[2][0] = mViewMatrix.mat[0][2];
    normalMatrix.mat[2][1] = mViewMatrix.mat[1][2];
    normalMatrix.mat[2][2] = mViewMatrix.mat[2][2];
    
    mSpriteShader->SetMatrixUniform("uWorldTransform", mvp);
    
    // Set uniforms that the shader uses
    if (mSpriteShader->HasUniform("uModelMatrix")) {
        mSpriteShader->SetMatrixUniform("uModelMatrix", model);
    }
    if (mSpriteShader->HasUniform("uNormalMatrix")) {
        mSpriteShader->SetMatrixUniform("uNormalMatrix", normalMatrix);
    }
    if (mSpriteShader->HasUniform("uColor")) {
        mSpriteShader->SetVectorUniform("uColor", color);
    }
    
    // Set lighting parameters
    if (applyLighting) {
        // Apply directional lighting based on camera angle
        if (mSpriteShader->HasUniform("uDirectionalLightDir")) {
            // For sprites (billboards), the normal always points toward the camera
            // So we use the inverse of the camera's forward direction as the light direction
            // to check how much the camera is looking from the light direction
            Vector3 cameraForward = Vector3(mViewMatrix.mat[0][2], mViewMatrix.mat[1][2], mViewMatrix.mat[2][2]);
            cameraForward.Normalize();
            
            
            // The scene light direction in world space
            Vector3 sceneLightDir = Vector3(1.0f, -1.0f, 1.0f);
            sceneLightDir = Vector3::Transform(sceneLightDir, modelView, 0.0f);
            
            // Calculate how much the camera direction aligns with the light
            // When camera looks from light direction, sprite should be bright
            // Use camera forward as the "normal" for billboard lighting
            mSpriteShader->SetVectorUniform("uDirectionalLightDir", sceneLightDir);
        }
        if (mSpriteShader->HasUniform("uDirectionalLightColor")) {
            mSpriteShader->SetVectorUniform("uDirectionalLightColor", Vector3(1.0f, 1.0f, 1.0f));
        }
        if (mSpriteShader->HasUniform("uAmbientLightColor")) {
            mSpriteShader->SetVectorUniform("uAmbientLightColor", Vector3(0.3f, 0.3f, 0.3f));  // Ambient for sprites
        }
    } else {
        // No lighting - fully lit with directional light color
        if (mSpriteShader->HasUniform("uDirectionalLightDir")) {
            mSpriteShader->SetVectorUniform("uDirectionalLightDir", Vector3(0.0f, 0.0f, -1.0f));
        }
        if (mSpriteShader->HasUniform("uDirectionalLightColor")) {
            mSpriteShader->SetVectorUniform("uDirectionalLightColor", Vector3(0.0f, 0.0f, 0.0f));  // No directional
        }
        if (mSpriteShader->HasUniform("uAmbientLightColor")) {
            mSpriteShader->SetVectorUniform("uAmbientLightColor", Vector3(1.0f, 1.0f, 1.0f));  // Full brightness
        }
    }
    
    // Bind texture and set atlas uniforms
    if (textureIndex >= 0 && textureIndex < static_cast<int>(mTextures.size())) {

        mTextures[textureIndex]->Bind(0);
        if (mSpriteShader->HasUniform("uTextureAtlas")) {
            mSpriteShader->SetIntegerUniform("uTextureAtlas", 0);
        }
        if (mSpriteShader->HasUniform("uAtlasColumns")) {
            mSpriteShader->SetIntegerUniform("uAtlasColumns", atlasColumns);
        }
        if (mSpriteShader->HasUniform("uAtlasTileSize")) {
            mSpriteShader->SetVectorUniform("uAtlasTileSize", Vector2(atlasTileSizeX, atlasTileSizeY));
        }
    }
    
    // Set the tile index as a uniform for the sprite shader
    // This overrides the per-vertex texture index
    if (mSpriteShader->HasUniform("uTileIndex")) {
        mSpriteShader->SetIntegerUniform("uTileIndex", tileIndex);
    }

    mSpriteShader->SetIntegerUniform("uIsSprite", 1); // Indicate this is a sprite
    
    // Set the tile UV offset if provided (for non-grid-based atlases)
    if (mSpriteShader->HasUniform("uTileOffset")) {
        mSpriteShader->SetVectorUniform("uTileOffset", Vector2(tileOffsetX, tileOffsetY));
    }
    
    mSpriteQuad->SetActive();
    glDrawElements(GL_TRIANGLES, mSpriteQuad->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::CreateSpriteQuad()
{
    // Create a simple quad mesh centered at origin with UVs
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    
    // Vertices for a centered quad (-0.5 to 0.5)
    // Normal points away from camera (0, 0, -1) so it points in the same direction as camera looks
    // UVs: bottom-left is (0,0), top-right is (1,1) - flipped V for correct orientation
    vertices.push_back(Vertex(Vector3(-0.5f,  0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f)));  // Top-left
    vertices.push_back(Vertex(Vector3(-0.5f, -0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f)));  // Bottom-left
    vertices.push_back(Vertex(Vector3( 0.5f, -0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f)));  // Bottom-right
    vertices.push_back(Vertex(Vector3( 0.5f,  0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f)));  // Top-right
    

    
    // Two triangles to form a quad
    triangles.push_back(Triangle(0, 1, 2, 0));  // First triangle
    triangles.push_back(Triangle(0, 2, 3, 0));  // Second triangle

    
    
    mSpriteQuad = new Mesh();
    mSpriteQuad->Build({vertices, triangles});
}
