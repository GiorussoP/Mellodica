#include "components/SpriteComponent.hpp"
#include "Game.hpp"
#include "actors/Actor.hpp"
#include "render/Renderer.hpp"
#include "render/TextureAtlas.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

SpriteComponent::SpriteComponent(Actor *owner, int textureIndex,
                                 TextureAtlas *atlas, bool isHUD)
    : DrawComponent(owner), mTextureIndex(textureIndex), mAnimTimer(0.0f),
      mAnimFPS(10.0f), mIsPaused(false), mTextureAtlas(atlas), mIsHUD(isHUD) {}

SpriteComponent::~SpriteComponent() {
  mAnimations.clear();
  mSpriteSheetData.clear();
}

void SpriteComponent::Update(float deltaTime) {
  if (mIsPaused || mAnimations.empty() || mAnimName.empty())
    return;

  auto &anim = mAnimations[mAnimName];
  mAnimTimer += mAnimFPS * deltaTime;

  float frameCount = static_cast<float>(anim.frameIndices.size());
  if (frameCount > 0.0f) {
    if (!anim.repeat && mAnimTimer >= frameCount) {
      mIsPaused = true;
      // Do not clamp mAnimTimer; let it stay or increase, but clamp in
      // GetCurrentTileIndex
    } else {
      // Loop if repeating
      while (mAnimTimer >= frameCount) {
        mAnimTimer -= frameCount;
      }
    }
  }
}

void SpriteComponent::SetAnimation(const std::string &name) {
  if (mAnimations.find(name) != mAnimations.end()) {
    mAnimName = name;
    auto &anim = mAnimations[mAnimName];
    float frameCount = static_cast<float>(anim.frameIndices.size());
    while (mAnimTimer >= frameCount) {
      mAnimTimer -= frameCount;
    }
  }
}

void SpriteComponent::AddAnimation(const std::string &name,
                                   const std::vector<std::string> &frameNames,
                                   bool repeat) {
  if (!mTextureAtlas) {
    std::cerr << "Cannot add animation '" << name << "': no texture atlas set"
              << std::endl;
    return;
  }

  // Convert frame names to tile indices using the atlas
  std::vector<int> tileIndices;
  tileIndices.reserve(frameNames.size());

  for (const auto &frameName : frameNames) {
    int tileIndex = mTextureAtlas->GetTileIndex(frameName);
    if (tileIndex < 0) {
      std::cerr << "Warning: Frame '" << frameName
                << "' not found in atlas for animation '" << name << "'"
                << std::endl;
      tileIndex = 0; // Fallback to first tile
    }
    tileIndices.push_back(tileIndex);
  }

  mAnimations.emplace(name, AnimationData{repeat, tileIndices});
}

int SpriteComponent::GetCurrentTileIndex() const {
  // Determine which tile index to use
  int tileIndex = mTextureAtlas ? mTextureIndex : 0;

  if (!mAnimations.empty() && !mAnimName.empty()) {
    // For animated sprites, get the tile index from the current animation frame
    auto it = mAnimations.find(mAnimName);
    if (it != mAnimations.end()) {
      const auto &anim = it->second;
      int frameIndex = static_cast<int>(std::floor(mAnimTimer));
      // Clamp frameIndex to prevent out-of-bounds access
      frameIndex =
          std::min(frameIndex, static_cast<int>(anim.frameIndices.size()) - 1);
      if (frameIndex >= 0) {
        tileIndex = anim.frameIndices[frameIndex];
      }
    }
  }

  return tileIndex;
}
