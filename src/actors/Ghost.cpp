#include "Ghost.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"

Ghost::Ghost(Game *game, int channel, int health)
    : Combatant(game, channel, health, CombatantType::Phantasm) {
  InitializeSprite();
}

Ghost::~Ghost() {}

void Ghost::InitializeSprite() {
  const std::string texture_name = "./assets/sprites/robots/ghost";
  TextureAtlas *atlas = mGame->GetRenderer()->LoadAtlas(texture_name + ".json");
  Texture *texture = mGame->GetRenderer()->LoadTexture(texture_name + ".png");
  int textureIndex = mGame->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
  mSpriteComponent->SetScale(Vector3(1.5f, 1.5f, 1.0f));
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.25f, 0.0f));
  mSpriteComponent->AddAnimation(
      "idle", {"ghost-1.png", "ghost-2.png", "ghost-3.png", "ghost-4.png"});
  mSpriteComponent->AddAnimation("dead", {"ghost-dead.png"});
  mSpriteComponent->SetAnimation("idle");
  mSpriteComponent->SetAnimFPS(4.0f);
  SetSpriteColorByChannel();
}
