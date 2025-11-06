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
    }
}

void SpriteComponent::AddAnimation(const std::string& name, const std::vector<std::string>& frameNames)
{
    if (!mTextureAtlas) {
        std::cerr << "Cannot add animation '" << name << "': no texture atlas set" << std::endl;
        return;
    }
    
    // Convert frame names to tile indices using the atlas
    std::vector<int> tileIndices;
    tileIndices.reserve(frameNames.size());
    
    for (const auto& frameName : frameNames) {
        int tileIndex = mTextureAtlas->GetTileIndex(frameName);
        if (tileIndex < 0) {
            std::cerr << "Warning: Frame '" << frameName << "' not found in atlas for animation '" << name << "'" << std::endl;
            tileIndex = 0;  // Fallback to first tile
        }
        tileIndices.push_back(tileIndex);
    }
    
    mAnimations.emplace(name, tileIndices);
}

int SpriteComponent::GetCurrentTileIndex() const
{
    // Determine which tile index to use
    int tileIndex = mTextureIndex;  // Default to mTextureIndex for non-animated sprites
    
    if (!mAnimations.empty() && !mAnimName.empty())
    {
        // For animated sprites, get the tile index from the current animation frame
        int frameIndex = static_cast<int>(std::floor(mAnimTimer));
        tileIndex = mAnimations.at(mAnimName)[frameIndex];
    }
    
    return tileIndex;
}
