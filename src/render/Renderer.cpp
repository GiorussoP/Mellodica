#include <GL/glew.h>
#include <iostream>
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "TextureAtlas.hpp"
#include "MeshComponent.hpp"
#include "SpriteComponent.hpp"
#include "Actor.hpp"

Renderer::Renderer()
: mViewMatrix(Matrix4::Identity)
, mProjectionMatrix(Matrix4::Identity)
, mMeshShader(nullptr)
, mSpriteShader(nullptr)
, mSpriteQuad(nullptr)

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
    float orthoSize = 4.0f;  // Size of the orthographic view (half-height)
    float aspectRatio = width / height;
    mProjectionMatrix = Matrix4::CreateOrtho(-orthoSize * aspectRatio, orthoSize * aspectRatio,
                                              -orthoSize, orthoSize, 0.1f, 100.0f);
    
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
/*
void Renderer::DrawMesh(MeshComponent& mesh,RendererMode mode)
{

    Vector3 position = mesh.GetOwner()->GetPosition();
    Vector3 size = mesh.GetOwner()->GetScale();
    Quaternion rotation = mesh.GetOwner()->GetRotation();

    // Visibility is now handled by spatial grid
    if (!mesh.IsVisible()) {
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
    
    // Apply view matrix
    Matrix4 modelView = model * mViewMatrix;
    
    // Combine with projection matrix
    Matrix4 mvp = modelView * mProjectionMatrix;
    
    // Normal matrix uses object rotation
    Matrix4 normalMatrix = Matrix4::CreateFromQuaternion(rotation);
    
    // Set mesh-specific uniforms
    mMeshShader->SetMatrixUniform("uWorldTransform", mvp);
    mMeshShader->SetMatrixUniform("uModelMatrix", model);
    mMeshShader->SetMatrixUniform("uNormalMatrix", normalMatrix);

    mMeshShader->SetVectorUniform("uColor", mesh.GetColor());

    
    // Bind atlas texture and set atlas-specific uniforms
    if (mesh.GetTextureAtlas() && mesh.GetTextureAtlas()->GetTextureIndex() < mTextures.size()) {

        
        mTextures[mesh.GetTextureAtlas()->GetTextureIndex()]->Bind(0);

        mMeshShader->SetIntegerUniform("uTileIndex", mode == RendererMode::TRIANGLES ? static_cast<int>(mesh.GetStartingIndex()) : -1);
        mMeshShader->SetIntegerUniform("uTextureAtlas", 0);
        mMeshShader->SetIntegerUniform("uAtlasColumns", mesh.GetTextureAtlas()->GetColumns());
        mMeshShader->SetVectorUniform("uAtlasTileSize", Vector2(mesh.GetTextureAtlas()->GetUVTileSizeX(), mesh.GetTextureAtlas()->GetUVTileSizeY()));
    }
    
    mesh.GetMesh().SetActive();
    
    if (mode == RendererMode::LINES) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mesh.GetMesh().GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (mode == RendererMode::TRIANGLES) {
        glDrawElements(GL_TRIANGLES, mesh.GetMesh().GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}
    */

void Renderer::DrawMeshesInstanced(const std::vector<MeshComponent*>& meshes, RendererMode mode)
{
    if (meshes.empty() || !mMeshShader) {
        return;
    }
    
    // Group meshes by their mesh pointer and texture atlas
    struct MeshGroup {
        Mesh* mesh;
        TextureAtlas* atlas;
        int textureIndex;
        std::vector<MeshComponent*> components;
    };
    
    std::vector<MeshGroup> groups;
    
    // Group meshes
    for (auto* meshComp : meshes) {
        if (!meshComp->IsVisible()) continue;
        
        Mesh* mesh = &meshComp->GetMesh();
        TextureAtlas* atlas = meshComp->GetTextureAtlas();
        int texIndex = atlas ? atlas->GetTextureIndex() : -1;
        
        // Find or create group
        bool found = false;
        for (auto& group : groups) {
            if (group.mesh == mesh && group.atlas == atlas && group.textureIndex == texIndex) {
                group.components.push_back(meshComp);
                found = true;
                break;
            }
        }
        
        if (!found) {
            groups.push_back({mesh, atlas, texIndex, {meshComp}});
        }
    }
    
    // Draw each group with instancing
    for (auto& group : groups) {
        if (group.components.empty()) continue;
        
        // Setup instance buffer if not already done
        if (group.mesh->GetMaxInstances() == 0) {
            group.mesh->SetupInstanceBuffer(10000);  // Max 10k instances per mesh type
        }
        
        // Prepare instance data: model matrix (16) + normal matrix (16) + color (3) + tileIndex (1) = 36 floats per instance
        std::vector<float> instanceData;
        instanceData.reserve(group.components.size() * 36);
        
        for (auto* meshComp : group.components) {
            Vector3 position = meshComp->GetOwner()->GetPosition();
            Vector3 size = meshComp->GetOwner()->GetScale();
            Quaternion rotation = meshComp->GetOwner()->GetRotation();
            
            // Model matrix (just transform, not MVP)
            Matrix4 model = Matrix4::CreateScale(size) *
                           Matrix4::CreateFromQuaternion(rotation) *
                           Matrix4::CreateTranslation(position);
            
            // Add model matrix (16 floats)
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    instanceData.push_back(model.mat[row][col]);
                }
            }
            
            // Normal matrix (just rotation)
            Matrix4 normalMatrix = Matrix4::CreateFromQuaternion(rotation);
            
            // Add normal matrix (16 floats)
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    instanceData.push_back(normalMatrix.mat[row][col]);
                }
            }
            
            // Add color (3 floats)
            Vector3 color = meshComp->GetColor();
            instanceData.push_back(color.x);
            instanceData.push_back(color.y);
            instanceData.push_back(color.z);
            
            // Add tile index (1 float)
            instanceData.push_back(static_cast<float>(meshComp->GetStartingIndex()));
        }
        
        // Upload instance data
        group.mesh->UpdateInstanceBuffer(instanceData, group.components.size());
        
        // Set view-projection matrix uniform (same for all instances)
        Matrix4 viewProj = mViewMatrix * mProjectionMatrix;
        mMeshShader->SetMatrixUniform("uViewProjection", viewProj);
        
        // Bind texture atlas
        if (group.atlas && group.textureIndex >= 0 && group.textureIndex < static_cast<int>(mTextures.size())) {
            mTextures[group.textureIndex]->Bind(0);
            mMeshShader->SetIntegerUniform("uTextureAtlas", 0);
            mMeshShader->SetIntegerUniform("uAtlasColumns", group.atlas->GetColumns());
            mMeshShader->SetVectorUniform("uAtlasTileSize", 
                Vector2(group.atlas->GetUVTileSizeX(), group.atlas->GetUVTileSizeY()));
        }
        
        // Activate mesh VAO
        group.mesh->SetActive();
        
        // Draw all instances
        if (mode == RendererMode::LINES) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElementsInstanced(GL_TRIANGLES, group.mesh->GetNumIndices(), GL_UNSIGNED_INT, nullptr, group.components.size());
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glDrawElementsInstanced(GL_TRIANGLES, group.mesh->GetNumIndices(), GL_UNSIGNED_INT, nullptr, group.components.size());
        }
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
	// Create mesh shader (Base.vert -> Mesh.frag)
	mMeshShader = new Shader();
	if (!mMeshShader->Load("./assets/shaders/Base.vert", "./assets/shaders/Mesh.frag")) {
		delete mMeshShader;
		mMeshShader = nullptr;
		return false;
	}

	// Create sprite shader (Base.vert -> Sprite.frag)
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("./assets/shaders/Base.vert", "./assets/shaders/Sprite.frag")) {
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

/*
void Renderer::DrawSprite(SpriteComponent& sprite, RendererMode mode)
{

    Vector3 position = sprite.GetOwner()->GetPosition();
    Vector3 size = sprite.GetOwner()->GetScale();
    Quaternion rotation = sprite.GetOwner()->GetRotation();

    // Visibility is now handled by spatial grid - no need for frustum check
    if (!sprite.IsVisible()) {
        return;
    }

    if (!mSpriteQuad) {
        std::cerr << "Sprite quad not created" << std::endl;
        return;
    }
    
    if (!mSpriteShader) {
        std::cerr << "Sprite shader not loaded" << std::endl;
        return;
    }
    
    // Create model matrix: Scale * Rotation * Translation
    // For sprites, we use a billboard effect by default (rotation around Y-axis only)
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateFromQuaternion(rotation) *
                    Matrix4::CreateTranslation(position);
    
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
    
    // Set sprite-specific uniforms
    mSpriteShader->SetMatrixUniform("uWorldTransform", mvp);
    mSpriteShader->SetMatrixUniform("uModelMatrix", model);
    mSpriteShader->SetMatrixUniform("uNormalMatrix", normalMatrix);
    mSpriteShader->SetVectorUniform("uColor", sprite.GetColor());
    
    // Bind texture atlas and set atlas-specific uniforms
    TextureAtlas* atlas = sprite.GetTextureAtlas();
    int texIndex = sprite.GetTextureIndex();
    
    if (!atlas) {
        std::cerr << "DrawSprite: sprite has no atlas!" << std::endl;
        return;
    }
    
    if (texIndex < 0 || texIndex >= static_cast<int>(mTextures.size())) {
        std::cerr << "DrawSprite: invalid texture index " << texIndex << " (size=" << mTextures.size() << ")" << std::endl;
        return;
    }
    
    mTextures[texIndex]->Bind(0);
    mSpriteShader->SetIntegerUniform("uTextureAtlas", 0);
    mSpriteShader->SetVectorUniform("uAtlasTileSize", Vector2(atlas->GetUVTileSizeX(), atlas->GetUVTileSizeY()));
    
    // Get the current tile index (handles animation)
    int tileIndex = sprite.GetCurrentTileIndex();
    
    // Get actual UV offset from atlas
    float offsetX = 0.0f, offsetY = 0.0f;
    atlas->GetTileUVOffset(tileIndex, offsetX, offsetY);
    mSpriteShader->SetVectorUniform("uTileOffset", Vector2(offsetX, offsetY));
    
    mSpriteQuad->SetActive();
    
    if (mode == RendererMode::LINES) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mSpriteQuad->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (mode == RendererMode::TRIANGLES) {
        glDrawElements(GL_TRIANGLES, mSpriteQuad->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}
    */

void Renderer::DrawSpritesInstanced(const std::vector<SpriteComponent*>& sprites, RendererMode mode)
{
    if (sprites.empty() || !mSpriteShader || !mSpriteQuad) {
        return;
    }
    
    // Group sprites by texture atlas
    struct SpriteGroup {
        TextureAtlas* atlas;
        int textureIndex;
        std::vector<SpriteComponent*> components;
    };
    
    std::vector<SpriteGroup> groups;
    
    // Group sprites by atlas
    for (auto* spriteComp : sprites) {
        if (!spriteComp->IsVisible()) continue;
        
        TextureAtlas* atlas = spriteComp->GetTextureAtlas();
        int texIndex = mode == RendererMode::TRIANGLES ? spriteComp->GetTextureIndex() : -1;
        
        // Find or create group
        bool found = false;
        for (auto& group : groups) {
            if (group.atlas == atlas && group.textureIndex == texIndex) {
                group.components.push_back(spriteComp);
                found = true;
                break;
            }
        }
        
        if (!found) {
            groups.push_back({atlas, texIndex, {spriteComp}});
        }
    }
    
    // Setup sprite quad instance buffer if not already done
    if (mSpriteQuad->GetMaxInstances() == 0) {
        mSpriteQuad->SetupInstanceBuffer(100000);  // Max 100k sprite instances
    }
    
    // Draw each group with instancing
    for (auto& group : groups) {
        if (group.components.empty()) continue;
        
        // Prepare instance data: model matrix (16) + normal matrix (16) + color (3) + tileIndex (1) = 36 floats per instance
        std::vector<float> instanceData;
        instanceData.reserve(group.components.size() * 36);
        
        for (auto* spriteComp : group.components) {
            Vector3 position = spriteComp->GetOwner()->GetPosition();
            Vector3 size = spriteComp->GetOwner()->GetScale();
            Quaternion rotation = spriteComp->GetOwner()->GetRotation();
            
            // Create initial model matrix
            Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                           Matrix4::CreateFromQuaternion(rotation) *
                           Matrix4::CreateTranslation(position);
            
            // Transform to view space
            Matrix4 modelView = model * mViewMatrix;
            
            // Billboard effect: strip rotation from modelView, keep only translation and scale
            // In our row-major matrix: mat[row][col]
            // Row 0 is X-axis, Row 1 is Y-axis, Row 2 is Z-axis, Row 3 is homogeneous
            Matrix4 billboard = Matrix4::Identity;
            
            // Row 0: X-axis (scaled, no rotation)
            billboard.mat[0][0] = size.x;
            billboard.mat[0][1] = 0.0f;
            billboard.mat[0][2] = 0.0f;
            billboard.mat[0][3] = 0.0f;
            
            // Row 1: Y-axis (scaled, no rotation)
            billboard.mat[1][0] = 0.0f;
            billboard.mat[1][1] = size.y;
            billboard.mat[1][2] = 0.0f;
            billboard.mat[1][3] = 0.0f;
            
            // Row 2: Z-axis (no rotation)
            billboard.mat[2][0] = 0.0f;
            billboard.mat[2][1] = 0.0f;
            billboard.mat[2][2] = 1.0f;
            billboard.mat[2][3] = 0.0f;
            
            // Row 3: Translation (from view-transformed position)
            billboard.mat[3][0] = modelView.mat[3][0];
            billboard.mat[3][1] = modelView.mat[3][1];
            billboard.mat[3][2] = modelView.mat[3][2];
            billboard.mat[3][3] = 1.0f;
            
            // Add billboard modelView matrix (16 floats)
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    instanceData.push_back(billboard.mat[row][col]);
                }
            }
            
            // Normal matrix for sprites (camera-facing)
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
            
            // Add normal matrix (16 floats)
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    instanceData.push_back(normalMatrix.mat[row][col]);
                }
            }
            
            // Add color (3 floats)
            Vector3 color = spriteComp->GetColor();
            instanceData.push_back(color.x);
            instanceData.push_back(color.y);
            instanceData.push_back(color.z);
            
            // Add current tile index (handles animation) (1 float)
            instanceData.push_back(static_cast<float>(spriteComp->GetCurrentTileIndex()));
        }
        
        // Upload instance data
        mSpriteQuad->UpdateInstanceBuffer(instanceData, group.components.size());
        
        // Set view-projection (just projection since billboard is already in view space)
        mSpriteShader->SetMatrixUniform("uViewProjection", mProjectionMatrix);
        
        // Bind texture atlas
        if (group.atlas && group.textureIndex >= 0 && group.textureIndex < static_cast<int>(mTextures.size())) {
            mTextures[group.textureIndex]->Bind(0);
            mSpriteShader->SetIntegerUniform("uTextureAtlas", 0);
            mSpriteShader->SetIntegerUniform("uAtlasColumns", group.atlas->GetColumns());
            mSpriteShader->SetVectorUniform("uAtlasTileSize", 
                Vector2(group.atlas->GetUVTileSizeX(), group.atlas->GetUVTileSizeY()));
        }
        
        // Activate sprite quad VAO
        mSpriteQuad->SetActive();
        
        // Draw all sprite instances
        if (mode == RendererMode::LINES) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElementsInstanced(GL_TRIANGLES, mSpriteQuad->GetNumIndices(), GL_UNSIGNED_INT, nullptr, group.components.size());
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glDrawElementsInstanced(GL_TRIANGLES, mSpriteQuad->GetNumIndices(), GL_UNSIGNED_INT, nullptr, group.components.size());
        }
    }
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

void Renderer::ActivateMeshShader()
{
    if (!mMeshShader) {
        std::cerr << "Mesh shader not loaded" << std::endl;
        return;
    }
    
    // Activate mesh shader program
    mMeshShader->SetActive();
    
    // Set frame-level uniforms (uniforms that don't change per mesh)
    Vector3 lightdir = Vector3(1.0f, -1.0f, 1.0f);
    lightdir.Normalize();
    mMeshShader->SetVectorUniform("uDirectionalLightDir", lightdir);
    mMeshShader->SetVectorUniform("uDirectionalLightColor", Vector3(1.0f, 1.0f, 1.0f));
    mMeshShader->SetVectorUniform("uAmbientLightColor", Vector3(0.2f, 0.2f, 0.2f));
}

void Renderer::ActivateSpriteShader()
{
    if (!mSpriteShader) {
        std::cerr << "Sprite shader not loaded" << std::endl;
        return;
    }
    
    // Activate sprite shader program
    mSpriteShader->SetActive();
    
    // Set frame-level uniforms (uniforms that don't change per sprite)
    // For sprites, lighting is typically simpler
    // These can be configured based on your needs
}