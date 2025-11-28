#include "../../include/UI/HUDElement.hpp"
#include "Game.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "TextureAtlas.hpp"

HUDElement::HUDElement(Game *game, const std::string &hudTexturePath,
                       const std::string &hudAtlasPath)
    : Actor(game) {
  game->AddAlwaysActive(this);

  // Get atlas from renderer cache
  TextureAtlas *atlas = game->GetRenderer()->LoadAtlas(hudAtlasPath);
  // Get texture index from renderer cache
  Texture *texture = game->GetRenderer()->LoadTexture(hudTexturePath);
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
  if (atlas) {
    atlas->SetTextureIndex(textureIndex);
  }

  // Create sprite component with atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas, true);
}

HUDElement::HUDElement(Game *game, const std::string &singleImagePath)
    : Actor(game) {
  game->AddAlwaysActive(this);

  // Get texture from renderer cache
  Texture *texture = game->GetRenderer()->LoadTexture(singleImagePath);
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);

  // Create sprite component without atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, nullptr, true);
}