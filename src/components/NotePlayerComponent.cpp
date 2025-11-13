#include "components/NotePlayerComponent.hpp"

const Vector3 NOTE_COLORS[16] = {
    Color::Red,     Color::Green, Color::Yellow, Color::Blue,
    Color::Magenta, Color::Blue,  Color::Purple, Color::Orange,
    Color::White,   Color::White, Color::White,  Color::White,
    Color::White,   Color::White, Color::White,  Color::White};

bool NotePlayerComponent::PlayNote(unsigned int note, unsigned int channel,
                                   float speed) {

  int noteIndex = mMirrored ? 11 - (note % 12) : note % 12;

  // std::cout << "tRYING TO PLAY note " << noteIndex << std::endl;
  if (mActiveNotes[noteIndex] != nullptr)
    return false;

  Vector3 position = mOwner->GetPosition() +
                     Vector3::Transform(mOffset, mOwner->GetRotation());

  Vector3 front =
      Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), mOwner->GetRotation());
  Vector3 right = Vector3::Cross(Vector3::UnitY, front);

  mActiveNotes[noteIndex] =
      new NoteActor(this, mOwner->GetGame(), channel, note, front,
                    NOTE_COLORS[channel], speed);

  mActiveNotes[noteIndex]->SetPosition(
      position + right * (-2.75f + (noteIndex) * 0.5f) + front * 0.5f);

  mActiveNotes[noteIndex]->Start();
  //std::cout << "Playing note " << noteIndex << " on channel " << channel
   //         << std::endl;

  return true;
}

bool NotePlayerComponent::EndNote(unsigned int note) {
  int noteIndex = mMirrored ? 11 - (note % 12) : note % 12;

  if (mActiveNotes[noteIndex] == nullptr)
    return false;

  mActiveNotes[noteIndex]->End();
  mActiveNotes[noteIndex] = nullptr;
  return true;
}
