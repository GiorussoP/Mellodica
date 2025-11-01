#pragma once
#include "DrawComponent.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class SpriteComponent : public DrawComponent
{
public:
    // Constructor for sprite with atlas (animated or static)
    SpriteComponent(class Actor* owner, int textureIndex, class TextureAtlas* atlas);
    
    ~SpriteComponent() override;
    
    void Draw(class Renderer* renderer) override;
    void Update(float deltaTime) override;
    
    // Animation controls
    void SetAnimFPS(float fps) { mAnimFPS = fps; }
    void SetAnimation(const std::string& name);
    void SetIsPaused(bool pause) { mIsPaused = pause; }
    void AddAnimation(const std::string& name, const std::vector<int>& tileIndices);
    
    // Tile/texture index (for non-animated sprites, set the tile to display)
    void SetTileIndex(int index) { mTextureIndex = index; }
    
    // Size controls
    void SetSize(const Vector2& size) { mSize = size; }
    Vector2 GetSize() const { return mSize; }
    
    // Lighting controls
    void SetApplyLighting(bool apply) { mApplyLighting = apply; }
    bool GetApplyLighting() const { return mApplyLighting; }
    
    // Atlas controls
    void SetTextureAtlas(class TextureAtlas* atlas) { mTextureAtlas = atlas; }

private:
    bool LoadSpriteSheetData(const std::string& dataPath);
    
    // Texture index in renderer (for animated sprites) or tile index (for static sprites)
    int mTextureIndex;
    
    // Sprite sheet data (UV rectangles for each frame)
    std::vector<Vector4> mSpriteSheetData;
    
    // Animations map (name -> list of frame indices)
    std::unordered_map<std::string, std::vector<int>> mAnimations;
    
    // Current animation state
    std::string mAnimName;
    float mAnimTimer;
    float mAnimFPS;
    bool mIsPaused;
    
    // Atlas parameters
    int mAtlasColumns;
    float mAtlasTileSizeX;
    float mAtlasTileSizeY;
    
    // Sprite size
    Vector2 mSize;
    int mWidth;
    int mHeight;
    
    // Lighting
    bool mApplyLighting;
    
    // Texture atlas (not owned, just a reference)
    class TextureAtlas* mTextureAtlas;
};
