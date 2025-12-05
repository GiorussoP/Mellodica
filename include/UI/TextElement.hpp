#ifndef TEXT_ELEMENT_HPP
#define TEXT_ELEMENT_HPP

#include "../actors/Actor.hpp"
#include "Math.hpp"
#include "SpriteComponent.hpp"
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class TextElement : public Actor {
public:
  TextElement(Game *game, const std::string &text, const Vector3 &color,
              const Vector3 &bgColor, float bgAlpha = 1.0f);
  ~TextElement();

  void SetText(const std::string &text);
  std::string GetText() const { return mText; }

  void SetTextColor(const Vector3 &color);
  void SetBackgroundColor(const Vector3 &bgColor);
  void SetBackgroundAlpha(float alpha);

  // SetScale to remember user scale
  void SetScale(const Vector3 &scale);

private:
  void UpdateTextDisplay();
  void RenderTextToTexture();

  std::string mText;
  Vector3 mTextColor;
  Vector3 mBackgroundColor;
  float mBackgroundAlpha;
  Vector3 mUserScale; // User-defined scale override
  bool mHasUserScale; // Whether user has set a custom scale

  // Text sprite (renders the text texture)
  SpriteComponent *mTextSprite;

  // Text texture
  class Texture *mTextTexture;

  static TTF_Font *sFont;
  static int sFontRefCount;
};

#endif
