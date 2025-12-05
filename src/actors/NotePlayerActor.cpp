#include "actors/NotePlayerActor.hpp"
#include "BattleSystem.hpp"
#include "Game.hpp"
#include "SynthEngine.hpp"
#include "actors/NoteActor.hpp"

float NotePlayerActor::noteSpacing = 0.5f;

bool NotePlayerActor::PlayNote(unsigned int note, unsigned int channel,
                               bool in_battle, float speed) {

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

  if (in_battle) {
    mActiveNotes[noteIndex]->SetPosition(
        position + right * (-offset + (noteIndex)*noteSpacing + noteSpacing));
  } else {
    mActiveNotes[noteIndex]->SetPosition(position);
  }

  mActiveNotes[noteIndex]->Start();
  // std::cout << "Playing note " << noteIndex << " on channel " << channel
  //          << std::endl;

  if (channel == 12 && mMirrored == false) {
    SynthEngine::startNote(channel, mActiveNotes[noteIndex]->GetNote());
  }

  return true;
}

bool NotePlayerActor::EndNote(unsigned int note) {
  int noteIndex =
      mMirrored ? (MAX_NOTES - 1) - (note % MAX_NOTES) : note % MAX_NOTES;

  if (mActiveNotes[noteIndex] == nullptr)
    return false;

  if (mActiveNotes[noteIndex]->GetMidiChannel() == 12 && mMirrored == false) {
    SynthEngine::stopNote(12, mActiveNotes[noteIndex]->GetNote());
  }

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

int NotePlayerActor::GetNoteFromPosition(Vector3 position) {
  Vector3 localPos = position - mPosition;
  Vector3 front = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), mRotation);
  Vector3 right = Vector3::Cross(Vector3::UnitY, front);

  float projection = Vector3::Dot(localPos, right);
  float offset = 0.5f * MAX_NOTES * noteSpacing + noteSpacing / 2.0f;

  float noteIndexFloat = (projection + offset - noteSpacing) / noteSpacing;
  int noteIndex = std::round(noteIndexFloat);

  if (noteIndex < 0)
    noteIndex = 0;
  if (noteIndex >= MAX_NOTES)
    noteIndex = MAX_NOTES - 1;

  return mMirrored ? (MAX_NOTES - 1) - noteIndex : noteIndex;
}
