#include "actors/Human.hpp"
#include "AssetLoader.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"
#include "scenes/Scene.hpp"

Human::Human(class Game *game, int channel, int health)
    : Combatant(game, channel, health, CombatantType::Human) {
  InitializeSprite();
}

Human::~Human() {}

void Human::InitializeSprite() {
  const std::string texture_name =
      getAssetPath("sprites/main-character/player");
  TextureAtlas *atlas = mGame->GetRenderer()->LoadAtlas(texture_name + ".json");
  Texture *texture = mGame->GetRenderer()->LoadTexture(texture_name + ".png");
  int textureIndex = mGame->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
  mSpriteComponent->SetScale(Vector3(1.0f, 1.0f, 1.0f));
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
  mSpriteComponent->AddAnimation(
      "idle", {"main-front-run1.png", "main-front-run2.png"});
  mSpriteComponent->AddAnimation("dead", {"main-back-idle.png"});
  mSpriteComponent->SetAnimation("idle");
  mSpriteComponent->SetAnimFPS(3.0f);

  if (mGame->GetCurrentScene()->GetSceneID() == Scene::SceneEnum::scene3 &&
      GetChannel() == 7) {
    mSpriteComponent->SetColor(Color::White);
  } else {
    SetSpriteColorByChannel();
    mSpriteComponent->SetColor(2.0f * mSpriteComponent->GetColor());
  }
}
