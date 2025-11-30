#ifndef HUD_ELEMENT_HPP
#define HUD_ELEMENT_HPP

#include "../actors/Actor.hpp"
#include "SpriteComponent.hpp"

class HUDElement : public Actor {
public:
    HUDElement(Game *game, const std::string &hudTexturePath,
             const std::string &hudAtlasPath);
    HUDElement(Game *game, const std::string &singleImagePath);
    SpriteComponent &GetSpriteComponent() { return *mSpriteComponent; }

private:
    SpriteComponent *mSpriteComponent;
};

#endif