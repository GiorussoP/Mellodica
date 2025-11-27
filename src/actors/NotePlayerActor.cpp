#include "actors/NotePlayerActor.hpp"
#include "actors/NoteActor.hpp"

float NotePlayerActor::noteSpacing = 0.5f;

bool NotePlayerActor::PlayNote(unsigned int note, unsigned int channel,
                               float speed) {

  int noteIndex =
      mMirrored ? (MAX_NOTES - 1) - (note % MAX_NOTES) : note % MAX_NOTES;

  // std::cout << "tRYING TO PLAY note " << noteIndex << std::endl;
  if (mActiveNotes[noteIndex] != nullptr)
    return false;

  Vector3 position = mPosition;

  Vector3 front = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), mRotation);

  Vector3 right = Vector3::Cross(Vector3::UnitY, front);

  mActiveNotes[noteIndex] = new NoteActor(this, mGame, channel, note, front,
                                          NOTE_COLORS[channel], speed);

  float offset = 0.5f * MAX_NOTES * noteSpacing + noteSpacing / 2.0f;

  mActiveNotes[noteIndex]->SetPosition(
      position + right * (-offset + (noteIndex)*noteSpacing + noteSpacing));

  mActiveNotes[noteIndex]->Start();
  // std::cout << "Playing note " << noteIndex << " on channel " << channel
  //          << std::endl;

  return true;
}

bool NotePlayerActor::EndNote(unsigned int note) {
  int noteIndex =
      mMirrored ? (MAX_NOTES - 1) - (note % MAX_NOTES) : note % MAX_NOTES;

  if (mActiveNotes[noteIndex] == nullptr)
    return false;

  mActiveNotes[noteIndex]->End();
  mActiveNotes[noteIndex] = nullptr;
  return true;
}

Vector3 NotePlayerActor::GetNotePosition(unsigned int note) {

  int noteIndex =
      mMirrored ? (MAX_NOTES - 1) - (note % MAX_NOTES) : note % MAX_NOTES;

  Vector3 position = mPosition;

  Vector3 front = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), mRotation);

  Vector3 right = Vector3::Cross(Vector3::UnitY, front);

  float offset = 0.5f * MAX_NOTES * noteSpacing + noteSpacing / 2.0f;

  return position +
         right * (-offset + ((float)noteIndex) * noteSpacing + noteSpacing);
}
