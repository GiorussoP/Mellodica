#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Math.hpp"
#include "Texture.hpp"
#include "MeshComponent.hpp"


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
    class Texture* LoadTexture(const std::string& fileName);

    int GetTextureIndex(class Texture* texture) const;
    
    // Mesh management
    class Mesh* LoadMesh(const std::string& meshName);
    
    // Atlas management
    class TextureAtlas* LoadAtlas(const std::string& atlasPath);

    // Drawing with texture atlas
    void DrawMesh(MeshComponent& mesh, const Vector3 &position, const Vector3 &size, const Quaternion &rotation, RendererMode mode);
    
    // Drawing 2D texture sprite - simpler sprite rendering
    void DrawTexture(const Vector3 &position, const Vector2 &size, const Vector3 &color,
                     int textureIndex, int tileIndex = 0, int atlasColumns = 1,
                     float atlasTileSizeX = 1.0f, float atlasTileSizeY = 1.0f,
                     float tileOffsetX = -1.0f, float tileOffsetY = -1.0f,
                     bool applyLighting = true);


    
                     
    void SetViewMatrix(const Matrix4 &view);
    void SetProjectionMatrix(const Matrix4 &projection);
    void SetCameraPosition(const Vector3 &position) { mCameraPosition = position; }

    void Clear();
    void Present();

private:
	bool LoadShaders();
    bool IsInFrustum(const Vector3 &position, const Vector3 &size);
    void CreateSpriteQuad();  // Create a simple quad for sprite rendering

	// Projection and view matrices
	Matrix4 mViewMatrix;
	Matrix4 mProjectionMatrix;
	
	// Shaders
	class Shader* mMeshShader;
	class Shader* mSpriteShader;
    
    // Textures
    std::vector<class Texture*> mTextures;
    std::unordered_map<std::string, Texture*> mTextureCache;
    
    // Meshes
    std::unordered_map<std::string, class Mesh*> mMeshCache;
    
    // Sprite quad mesh for simple sprite rendering
    class Mesh* mSpriteQuad;
    
    // Atlases
    std::unordered_map<std::string, TextureAtlas*> mAtlasCache;
	
    Vector3 mCameraPosition;
};