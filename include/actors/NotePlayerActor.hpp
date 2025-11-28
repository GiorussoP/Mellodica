#ifndef NotePlayerActor_HPP
#define NotePlayerActor_HPP

#include "actors/NoteActor.hpp"
#include <iostream>

class Game;
#include "SynthEngine.hpp"
#include <array>

static const unsigned int MAX_NOTES = 12;

class NotePlayerActor : public Actor {
public:
  NotePlayerActor(Game *game, bool mirrored = true)
      : Actor(game), mMirrored(mirrored) {
    mActiveNotes.fill(nullptr);
  };
  ~NotePlayerActor() {};
  bool PlayNote(unsigned int note, unsigned int channel, float speed = 1.0f);
  bool EndNote(unsigned int note);

  Vector3 GetNotePosition(unsigned int noteIndex);

  void MarkNoteDead(NoteActor *note) {
    if (note != nullptr && mActiveNotes[note->GetNote() % MAX_NOTES] == note) {
      mActiveNotes[note->GetNote() % MAX_NOTES] = nullptr;

      if (note->GetMidiChannel() == 12 && mMirrored == false) {
        SynthEngine::stopNote(12, note->GetNote());
      }

      std::cout << "Marked note " << note->GetNote() << " on channel "
                << note->GetMidiChannel() << " as dead." << std::endl;
    }
  };

  const std::array<NoteActor *, MAX_NOTES> &GetActiveNotes() const {
    return mActiveNotes;
  }

private:
  bool mMirrored;

  std::array<NoteActor *, MAX_NOTES> mActiveNotes;

  static float noteSpacing;
};

#endif