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

    // Drawing with texture atlas (legacy - single mesh)
    void DrawMesh(MeshComponent& mesh, RendererMode mode);
    
    // Instanced drawing - draw multiple instances of the same mesh
    void DrawMeshesInstanced(const std::vector<MeshComponent*>& meshes, RendererMode mode);
    
    // Drawing sprites (legacy - single sprite)
    void DrawSprite(SpriteComponent& sprite, RendererMode mode);
    
    // Instanced sprite drawing - draw multiple sprites
    void DrawSpritesInstanced(const std::vector<SpriteComponent*>& sprites, RendererMode mode);
    
    // HUD sprite drawing - draw sprites in screen space (after framebuffer)
    void DrawHUDSprites(const std::vector<SpriteComponent*>& hudSprites);

    // Batch rendering - set frame-level uniforms once before drawing multiple objects
    void ActivateMeshShader();
    void ActivateSpriteShader();
    void ActivateMeshShaderForBloom();    // Activate mesh shader for bloom pass
    void ActivateSpriteShaderForBloom();  // Activate sprite shader for bloom pass

    
    // Draw a single mesh without instancing (for debug drawing)
    void DrawSingleMesh(Mesh* mesh, const Vector3& position, const Vector3& scale, const Quaternion& rotation = Quaternion::Identity);
                     
    void SetViewMatrix(const Matrix4 &view);
    void SetProjectionMatrix(const Matrix4 &projection);

    void Clear();
    void Present();
    
    // Framebuffer rendering
    void BeginFramebuffer();  // Start rendering to framebuffer
    void EndFramebuffer();    // Render framebuffer to screen
    
    // Bloom rendering
    void BeginBloomPass();    // Start rendering bloomed objects to bloom framebuffer
    void EndBloomPass();      // Finish rendering to bloom framebuffer
    void ApplyBloomBlur();    // Apply Gaussian blur to bloom texture
    
    // Get framebuffer dimensions
    int GetFramebufferWidth() const { return mFramebufferWidth; }
    int GetFramebufferHeight() const { return mFramebufferHeight; }

    bool IsDark() const { return mIsDark; }
    void SetIsDark(bool isDark) { mIsDark = isDark; }

private:
	bool LoadShaders();
    void CreateSpriteQuad();  // Create a simple quad for sprite rendering
    void CreateFramebuffer(); // Create framebuffer for render-to-texture
    void CreateScreenQuad();  // Create fullscreen quad for framebuffer display
    void CreateBloomFramebuffer(); // Create bloom framebuffer for bright objects
    void CreateBlurTextures();     // Create textures for ping-pong blur

	// Projection and view matrices
	Matrix4 mViewMatrix;
	Matrix4 mProjectionMatrix;
	
	// Shaders
	Shader* mMeshShader;
	Shader* mSpriteShader;
	Shader* mFramebufferShader;
	Shader* mHUDShader;
	Shader* mBloomBlurShader;
    
    // Textures
    std::vector<Texture*> mTextures;
    std::unordered_map<std::string, Texture*> mTextureCache;
    
    // Meshes
    std::unordered_map<std::string, Mesh*> mMeshCache;
    
    // Sprite quad mesh for simple sprite rendering
    Mesh* mSpriteQuad;
    
    // Screen quad for framebuffer rendering
    Mesh* mScreenQuad;
    
    // Framebuffer objects
    GLuint mFramebuffer;
    GLuint mFramebufferTexture;
    GLuint mFramebufferDepthStencil;
    int mFramebufferWidth;
    int mFramebufferHeight;
    
    // Bloom framebuffer objects
    GLuint mBloomFramebuffer;
    GLuint mBloomTexture;
    GLuint mBloomDepthStencil;
    
    // Blur textures for ping-pong blur
    GLuint mBlurTexture1;
    GLuint mBlurTexture2;
    GLuint mBlurFramebuffer1;
    GLuint mBlurFramebuffer2;
    

    bool mIsDark;

    // Atlases
    std::unordered_map<std::string, TextureAtlas*> mAtlasCache;
};