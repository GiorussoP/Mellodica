#include "components/SpriteComponent.hpp"
#include "actors/Actor.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"
#include "render/TextureAtlas.hpp"
#include <fstream>
#include <iostream>
#include <cmath>


SpriteComponent::SpriteComponent(Actor* owner, int textureIndex, TextureAtlas* atlas)
: DrawComponent(owner)
, mTextureIndex(textureIndex)
, mAnimTimer(0.0f)
, mAnimFPS(10.0f)
, mIsPaused(false)
, mAtlasColumns(atlas ? atlas->GetColumns() : 1)
, mAtlasTileSizeX(atlas ? atlas->GetUVTileSizeX() : 1.0f)
, mAtlasTileSizeY(atlas ? atlas->GetUVTileSizeY() : 1.0f)
, mSize(1.0f, 1.0f)  // Default to 1x1 world units
, mWidth(atlas ? atlas->GetTileWidth() : 32)
, mHeight(atlas ? atlas->GetTileHeight() : 32)
, mApplyLighting(true)
, mTextureAtlas(atlas)
{
}

SpriteComponent::~SpriteComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool SpriteComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // For now, we'll skip JSON parsing and just use the atlas directly
    // The user can add animations manually with AddAnimation
    (void)dataPath;
    return true;
}

void SpriteComponent::Draw(Renderer* renderer)
{
    if (!mIsVisible)
        return;
    
    Vector2 size = mSize * mOwner->GetScale();
    
    // Get the texture atlas to fetch actual UV offsets
    TextureAtlas* atlas = mTextureAtlas;
    
    // Determine which tile index to use
    int tileIndex = mTextureIndex;  // Default to mTextureIndex for non-animated sprites
    
    if (!mAnimations.empty() && !mAnimName.empty())
    {
        // For animated sprites, get the tile index from the current animation frame
        int frameIndex = static_cast<int>(std::floor(mAnimTimer));
        tileIndex = mAnimations[mAnimName][frameIndex];
    }
    
    // Get actual UV offset from atlas
    float offsetX = -1.0f, offsetY = -1.0f;
    if (atlas) {
        atlas->GetTileUVOffset(tileIndex, offsetX, offsetY);
    }
    
    renderer->DrawTexture(
        mOwner->GetPosition(),
        size,
        mColor,
        mTextureIndex,
        tileIndex,
        mAtlasColumns,
        mAtlasTileSizeX,
        mAtlasTileSizeY,
        offsetX,
        offsetY,
        mApplyLighting
    );
}

void SpriteComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.empty() || mAnimName.empty())
        return;
    
    mAnimTimer += mAnimFPS * deltaTime;
    
    // Loop animation
    while (mAnimTimer >= static_cast<float>(mAnimations[mAnimName].size()))
    {
        mAnimTimer -= static_cast<float>(mAnimations[mAnimName].size());
    }
}

void SpriteComponent::SetAnimation(const std::string& name)
{
    if (mAnimations.find(name) != mAnimations.end())
    {
        mAnimName = name;
        mAnimTimer = 0.0f;
    }
}

void SpriteComponent::AddAnimation(const std::string& name, const std::vector<int>& tileIndices)
{
    mAnimations.emplace(name, tileIndices);
}
