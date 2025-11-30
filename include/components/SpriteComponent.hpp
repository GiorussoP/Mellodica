#pragma once
#include "DrawComponent.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class SpriteComponent : public DrawComponent {
public:
  // Constructor for sprite with atlas (animated or static)
  // isHUD: if true, sprite is drawn in screen space after framebuffer rendering
  SpriteComponent(class Actor *owner, int textureIndex,
                  class TextureAtlas *atlas = nullptr, bool isHUD = false);

  ~SpriteComponent() override;

  void Update(float deltaTime) override;

  // Animation controls
  void SetAnimFPS(float fps) { mAnimFPS = fps; }
  void SetAnimation(const std::string &name);
  void SetIsPaused(bool pause) { mIsPaused = pause; }
  void AddAnimation(const std::string &name,
                    const std::vector<std::string> &frameNames,
                    bool repeat = true);

  void SetAnimationTimer(float time) { mAnimTimer = time; }
  float GetAnimationTimer() const { return mAnimTimer; }

  // Atlas controls
  void SetTextureAtlas(class TextureAtlas *atlas) { mTextureAtlas = atlas; }
  class TextureAtlas *GetTextureAtlas() const { return mTextureAtlas; }

  // Get texture index (in renderer's texture array)
  int GetTextureIndex() const { return mTextureIndex; }
  void SetTextureIndex(int idx) { mTextureIndex = idx; }

  // Get current tile index (accounting for animations)
  int GetCurrentTileIndex() const;

  // HUD sprite flag
  bool IsHUD() const { return mIsHUD; }

private:
  // Texture index in renderer (for animated sprites) or tile index (for static
  // sprites)
  int mTextureIndex;

  // Sprite sheet data (UV rectangles for each frame)
  std::vector<Vector4> mSpriteSheetData;

  struct AnimationData {
    bool repeat;
    std::vector<int> frameIndices;
  };

  // Animations map (name -> list of frame indices)
  std::unordered_map<std::string, AnimationData> mAnimations;

  // Current animation state
  std::string mAnimName;
  float mAnimTimer;
  float mAnimFPS;
  bool mIsPaused;

  // Texture atlas (not owned, just a reference)
  class TextureAtlas *mTextureAtlas;

  // HUD sprite flag (if true, rendered in screen space after framebuffer)
  bool mIsHUD;
};
