#ifndef MELLODICA_CREDITSSCREEN_H
#define MELLODICA_CREDITSSCREEN_H

#include "./UIScreen.hpp"
#include "actors/NotePlayerActor.hpp"

class CreditsScreen : public UIScreen {
public:
  CreditsScreen(class Game *game, const std::string &fontName);
  ~CreditsScreen();

  void HandleKeyPress(int key) override;
  void Update(float deltaTime) override;

private:
  float mTimer;
  TextElement *mCreditsText;
  NotePlayerActor *mNotePlayer;
};

#endif // MELLODICA_CREDITSSCREEN_H
