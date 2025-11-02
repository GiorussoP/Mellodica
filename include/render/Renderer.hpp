#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Math.hpp"
#include "Texture.hpp"
#include "MeshComponent.hpp"
#include "SpriteComponent.hpp"
#include "Shader.hpp"


enum class RendererMode
{
    TRIANGLES,
    LINES
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initialize(float width, float height);
	void Shutdown();

    // Texture management
    Texture* LoadTexture(const std::string& fileName);

    int GetTextureIndex(Texture* texture) const;
    
    // Mesh management
    Mesh* LoadMesh(const std::string& meshName);
    
    // Atlas management
    TextureAtlas* LoadAtlas(const std::string& atlasPath);

    // Drawing with texture atlas
    void DrawMesh(MeshComponent& mesh, RendererMode mode);
    
    // Drawing sprites
    void DrawSprite(SpriteComponent& sprite, RendererMode mode);

    // Batch rendering - set frame-level uniforms once before drawing multiple objects
    void ActivateMeshShader();
    void ActivateSpriteShader();


    
                     
    void SetViewMatrix(const Matrix4 &view);
    void SetProjectionMatrix(const Matrix4 &projection);
    void SetCameraPosition(const Vector3 &position) { mCameraPosition = position; }

    void Clear();
    void Present();

private:
	bool LoadShaders();
    bool IsInFrustum(const Vector3 &position, const float radius);
    void CreateSpriteQuad();  // Create a simple quad for sprite rendering

	// Projection and view matrices
	Matrix4 mViewMatrix;
	Matrix4 mProjectionMatrix;
	
	// Shaders
	Shader* mMeshShader;
	Shader* mSpriteShader;
    
    // Textures
    std::vector<Texture*> mTextures;
    std::unordered_map<std::string, Texture*> mTextureCache;
    
    // Meshes
    std::unordered_map<std::string, Mesh*> mMeshCache;
    
    // Sprite quad mesh for simple sprite rendering
    Mesh* mSpriteQuad;
    
    // Atlases
    std::unordered_map<std::string, TextureAtlas*> mAtlasCache;
	
    Vector3 mCameraPosition;
};