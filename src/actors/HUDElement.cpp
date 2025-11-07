#include "HUDElement.hpp"
#include "TextureAtlas.hpp"
#include "Texture.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

HUDElement::HUDElement(Game* game, const std::string& hudTexturePath, const std::string& hudAtlasPath)
    : Actor(game) {
    game->AddAlwaysActive(this);

    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas(hudAtlasPath);
    // Get texture index from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture(hudTexturePath);
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    
    // Create sprite component with atlas
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas, true);
}