#ifndef NotePlayerComponent_HPP
#define NotePlayerComponent_HPP

#include "actors/NoteActor.hpp"
#include <iostream>

class Game;
#include <array>

class NotePlayerComponent : public Component {
public:
  NotePlayerComponent(Actor *owner, bool mirrored = true,
                      Vector3 offset = Vector3::Zero)
      : Component(owner), mOffset(offset), mMirrored(mirrored) {
    mActiveNotes.fill(nullptr);
  };
  ~NotePlayerComponent() {};
  bool PlayNote(unsigned int note, unsigned int channel, float speed = 10.0f);
  bool EndNote(unsigned int note);

  void MarkNoteDead(NoteActor *note) {
    if (note != nullptr && mActiveNotes[note->GetNote() % 12] == note) {
      mActiveNotes[note->GetNote() % 12] = nullptr;

      std::cout << "Marked note " << note->GetNote() << " on channel "
                << note->GetMidiChannel() << " as dead." << std::endl;
    }
  };

  const std::array<NoteActor *, 12> &GetActiveNotes() const {
    return mActiveNotes;
  }

private:
  Vector3 mOffset;

  bool mMirrored;

  std::array<NoteActor *, 12> mActiveNotes;
};

#endif