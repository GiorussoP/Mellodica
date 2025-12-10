//
// Created by rapha on 5/12/2025.
// OpeningScreen.cpp

#include "../../../include/UI/Screen/OpeningScreen.hpp"

#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "scenes/MainMenu.hpp"

OpeningScreen::OpeningScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName), mTimer(0.0f), mStoryPosition(1.0f),
      mStory0(nullptr), mStory1(nullptr), mStory2(nullptr), mStory3(nullptr),
      mStory4(nullptr), mStory5(nullptr), mStory6(nullptr), mStoryText(nullptr),
      mImageIndex(0) {

  // Add black rectangle background
  auto background = AddImageOrElement(Vector3(0.0f, 0.0f, 0.0f));
  background->SetPosition(Vector3(0.0f, 0.0f, -1.0f));
  background->SetScale(Vector3(3.0f, 3.0f, 1.0f));

  // Load story images and make them invisible initially
  mStory0 = AddImageOrElement(getAssetPath("sprites/scenes/scene0.png"));
  mStory1 = AddImageOrElement(getAssetPath("sprites/scenes/scene1.png"));
  mStory2 = AddImageOrElement(getAssetPath("sprites/scenes/scene2.png"));
  mStory3 = AddImageOrElement(getAssetPath("sprites/scenes/scene3.png"));
  mStory4 = AddImageOrElement(getAssetPath("sprites/scenes/scene4.png"));
  mStory5 = AddImageOrElement(getAssetPath("sprites/scenes/scene5.png"));
  mStory6 = AddImageOrElement(getAssetPath("sprites/scenes/scene6.png"));

  mStory0->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mStory1->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mStory2->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mStory3->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mStory4->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mStory5->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mStory6->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

  mStory0->SetScale(Vector3(2.0f, 2.0f, 1.0f));
  mStory1->SetScale(Vector3(2.0f, 2.0f, 1.0f));
  mStory2->SetScale(Vector3(2.0f, 2.0f, 1.0f));
  mStory3->SetScale(Vector3(2.0f, 2.0f, 1.0f));
  mStory4->SetScale(Vector3(2.0f, 2.0f, 1.0f));
  mStory5->SetScale(Vector3(2.0f, 2.0f, 1.0f));
  mStory6->SetScale(Vector3(2.0f, 2.0f, 1.0f));

  mStory0->GetSpriteComponent().SetVisible(true);
  mStory1->GetSpriteComponent().SetVisible(false);
  mStory2->GetSpriteComponent().SetVisible(false);
  mStory3->GetSpriteComponent().SetVisible(false);
  mStory4->GetSpriteComponent().SetVisible(false);
  mStory5->GetSpriteComponent().SetVisible(false);
  mStory6->GetSpriteComponent().SetVisible(false);

  mStoryText = AddText("Sempre fui uma criança\napaixonada por música...",
                       Color::White, Vector3(0.0f, 0.0f, 0.0f), 0.0f);
  mStoryText->SetPosition(Vector3(0.0f, -0.8f, 0.0f));
}

OpeningScreen::~OpeningScreen() { UIScreen::~UIScreen(); }

void OpeningScreen::HandleKeyPress(int key) {
  // On specific key presses, jump to main menu
  if (key == SDLK_RETURN || key == SDLK_SPACE ||
      (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_MINUS ||
      key == SDLK_EQUALS) {
    mGame->LoadScene(new MainMenu(mGame));
  }
}

void OpeningScreen::Update(float deltaTime) {
  UIScreen::Update(deltaTime);
  mTimer += deltaTime;
  if (mTimer >= 7.0f) {
    // Change story scene
    mTimer = 0.0f;
    mImageIndex++;

    if (mImageIndex > 6) {
      // End of story, go to main menu
      mGame->LoadScene(new MainMenu(mGame));
      return;
    }

    switch (mImageIndex) {
    case 1:
      mStory0->GetSpriteComponent().SetVisible(false);
      mStory1->GetSpriteComponent().SetVisible(true);
      mStoryText->SetText(
          "E cresci assim, sempre me\ndesenvolvendo mais e mais...");
      break;
    case 2:
      mStory1->GetSpriteComponent().SetVisible(false);
      mStory2->GetSpriteComponent().SetVisible(true);
      mStoryText->SetText(
          "Quanto mais a melodia aumentava\nmais o apoio diminuía...");
      break;
    case 3:
      mStory2->GetSpriteComponent().SetVisible(false);
      mStory3->GetSpriteComponent().SetVisible(true);
      mStoryText->SetText(
          "E certa noite, em meio à \nfrustração, algo aconteceu...");
      break;
    case 4:
      mStory3->GetSpriteComponent().SetVisible(false);
      mStory4->GetSpriteComponent().SetVisible(true);
      mStoryText->SetText("O piano ficou estranho...");
      break;
    case 5:
      mStory4->GetSpriteComponent().SetVisible(false);
      mStory5->GetSpriteComponent().SetVisible(true);
      mStoryText->SetText("Acredite se quiser, o piano\n ME ENGOLIU!");
      break;
    case 6:
      mStory5->GetSpriteComponent().SetVisible(false);
      mStory6->GetSpriteComponent().SetVisible(true);
      mStoryText->SetText("E agora estou aqui...");
      break;

    default:
      break;
    }
  }
}