#include "../../include/UI/TextElement.hpp"
#include "../../include/Game.hpp"
#include "../../include/render/Renderer.hpp"
#include "../../include/render/Texture.hpp"
#include <SDL2/SDL_ttf.h>

// Static font members
TTF_Font *TextElement::sFont = nullptr;
int TextElement::sFontRefCount = 0;

TextElement::TextElement(Game *game, const std::string &text,
                         const Vector3 &color, const Vector3 &bgColor,
                         float bgAlpha)
    : Actor(game), mText(text), mTextColor(color), mBackgroundColor(bgColor),
      mBackgroundAlpha(bgAlpha), mUserScale(Vector3::One), mHasUserScale(false),
      mTextSprite(nullptr), mTextTexture(nullptr) {

  // Add to always active list (like HUDElement does)
  game->AddAlwaysActive(this);

  // Initialize SDL_ttf on first use
  if (sFontRefCount == 0) {
    if (TTF_Init() == -1) {
      SDL_Log("TTF_Init failed: %s", TTF_GetError());
      // Create fallback sprite with no texture
      mTextSprite = new SpriteComponent(this, -1, nullptr, true);
      mTextSprite->SetColor(mBackgroundColor);
      return;
    }
  }
  sFontRefCount++;

  // Load font on first use
  if (!sFont) {
    sFont = TTF_OpenFont("./assets/fonts/MedodicaRegular.otf", 24);
    if (!sFont) {
      SDL_Log("Failed to load font: %s", TTF_GetError());
      // Create fallback sprite
      mTextSprite = new SpriteComponent(this, -1, nullptr, true);
      mTextSprite->SetColor(mBackgroundColor);
      return;
    }
  }

  // Create text texture
  mTextTexture = new Texture();

  // Render initial text and get texture index
  RenderTextToTexture();

  // Get the texture index (should be registered by now)
  int textureIndex = mGame->GetRenderer()->GetTextureIndex(mTextTexture);

  // Create sprite component with the texture index (like HUDElement does)
  mTextSprite = new SpriteComponent(this, textureIndex, nullptr, true);
  mTextSprite->SetColor(Vector3(1.0f, 1.0f, 1.0f)); // White multiplier
  mTextSprite->SetVisible(true);

  SDL_Log("TextElement created with text: '%s', texture index: %d",
          mText.c_str(), textureIndex);
}

TextElement::~TextElement() {
  // Don't delete mTextTexture - it's managed by the Renderer
  // The Renderer will clean up all textures in its destructor

  sFontRefCount--;
  if (sFontRefCount == 0 && sFont) {
    TTF_CloseFont(sFont);
    sFont = nullptr;
    TTF_Quit();
  }
}

void TextElement::SetText(const std::string &text) {
  if (mText != text) {
    mText = text;
    RenderTextToTexture();
    // Update sprite's texture index in case it changed
    if (mTextSprite && mTextTexture) {
      int textureIndex = mGame->GetRenderer()->GetTextureIndex(mTextTexture);
      mTextSprite->SetTextureIndex(textureIndex);
    }
    // RenderTextToTexture will reapply user scale if it was set
  }
}

void TextElement::SetTextColor(const Vector3 &color) {
  mTextColor = color;
  RenderTextToTexture();
  // Update sprite's texture index
  if (mTextSprite && mTextTexture) {
    int textureIndex = mGame->GetRenderer()->GetTextureIndex(mTextTexture);
    mTextSprite->SetTextureIndex(textureIndex);
  }
}

void TextElement::SetBackgroundColor(const Vector3 &bgColor) {
  mBackgroundColor = bgColor;
  RenderTextToTexture();
  // Update sprite's texture index
  if (mTextSprite && mTextTexture) {
    int textureIndex = mGame->GetRenderer()->GetTextureIndex(mTextTexture);
    mTextSprite->SetTextureIndex(textureIndex);
  }
}

void TextElement::SetBackgroundAlpha(float alpha) {
  mBackgroundAlpha = alpha;
  RenderTextToTexture();
  // Update sprite's texture index
  if (mTextSprite && mTextTexture) {
    int textureIndex = mGame->GetRenderer()->GetTextureIndex(mTextTexture);
    mTextSprite->SetTextureIndex(textureIndex);
  }
}

void TextElement::SetScale(const Vector3 &scale) {
  mUserScale = scale;
  mHasUserScale = true;
  Actor::SetScale(scale);
}

void TextElement::UpdateTextDisplay() {
  // Deprecated - use SetText instead
  RenderTextToTexture();
}

void TextElement::RenderTextToTexture() {
  if (!sFont || mText.empty()) {
    SDL_Log("RenderTextToTexture: Font not loaded or text empty");
    return;
  }

  if (!mTextTexture) {
    SDL_Log("RenderTextToTexture: mTextTexture is null!");
    return;
  }

  // Note: mTextSprite might be null during initial construction, that's OK

  // Convert Vector3 color to SDL_Color
  SDL_Color textColor = {static_cast<Uint8>(mTextColor.x * 255),
                         static_cast<Uint8>(mTextColor.y * 255),
                         static_cast<Uint8>(mTextColor.z * 255), 255};

  SDL_Surface *textSurface = nullptr;

  // Use different rendering modes based on background alpha
  if (mBackgroundAlpha > 0.0f) {
    // Render with background color
    SDL_Color bgColor = {static_cast<Uint8>(mBackgroundColor.x * 255),
                         static_cast<Uint8>(mBackgroundColor.y * 255),
                         static_cast<Uint8>(mBackgroundColor.z * 255),
                         static_cast<Uint8>(mBackgroundAlpha * 255)};
    textSurface =
        TTF_RenderText_Shaded(sFont, mText.c_str(), textColor, bgColor);
  } else {
    // Render with transparent background
    textSurface = TTF_RenderText_Blended(sFont, mText.c_str(), textColor);
  }

  if (!textSurface) {
    SDL_Log("Failed to render text: %s", TTF_GetError());
    return;
  }

  SDL_Log("TextElement: Rendered text '%s' to %dx%d surface", mText.c_str(),
          textSurface->w, textSurface->h);

  // Load texture from surface (this will handle unloading old texture
  // internally)
  if (!mTextTexture->LoadFromSurface(textSurface)) {
    SDL_Log("Failed to load texture from surface");
    SDL_FreeSurface(textSurface);
    return;
  }

  SDL_Log("TextElement: Loaded texture successfully (GL ID: %d)",
          mTextTexture->GetTextureID());

  // Register texture with renderer and get index
  int textureIndex = mGame->GetRenderer()->RegisterTexture(mTextTexture);
  SDL_Log("TextElement: Registered texture with index %d (GL ID: %d)",
          textureIndex, mTextTexture->GetTextureID());

  // Set sprite scale based on text dimensions
  // Keep aspect ratio and make text a reasonable size
  float pixelToWorld = 200.0f; // Adjust this to change text size
  float width = textSurface->w / pixelToWorld;
  float height = textSurface->h / pixelToWorld;

  // Apply user scale if set, otherwise use auto-calculated scale
  if (mHasUserScale) {
    Actor::SetScale(mUserScale); // Use Actor::SetScale to avoid recursion
  } else {
    Actor::SetScale(Vector3(width, height, 1.0f));
  }

  SDL_Log("TextElement: Text size %dx%d pixels, scale %fx%f at position (%f, "
          "%f, %f)",
          textSurface->w, textSurface->h, GetScale().x, GetScale().y,
          GetPosition().x, GetPosition().y, GetPosition().z);

  SDL_FreeSurface(textSurface);
}
