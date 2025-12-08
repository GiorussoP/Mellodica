#include "actors/RobotA.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"

RobotA::RobotA(Game *game, int channel, int health)
    : Combatant(game, channel, health, CombatantType::Robot) {
  InitializeSprite();
}

RobotA::~RobotA() {}

void RobotA::InitializeSprite() {
  const std::string texture_name = "./assets/sprites/robots/robot";
  TextureAtlas *atlas = mGame->GetRenderer()->LoadAtlas(texture_name + ".json");
  Texture *texture = mGame->GetRenderer()->LoadTexture(texture_name + ".png");
  int textureIndex = mGame->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
  mSpriteComponent->SetScale(Vector3(1.5f, 1.5f, 1.0f));
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
  mSpriteComponent->AddAnimation("idle", {"robot-A-1.png", "robot-A-2.png",
                                          "robot-A-3.png", "robot-A-4.png",
                                          "robot-A-5.png"});
  mSpriteComponent->AddAnimation("dead", {"robot-A-dead.png"});
  mSpriteComponent->SetAnimation("idle");
  mSpriteComponent->SetAnimFPS(4.0f);
  SetSpriteColorByChannel();
}
