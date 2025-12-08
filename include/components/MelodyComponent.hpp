#pragma once
#include "Component.hpp"
#include "NoteActor.hpp"
#include <vector>



class MelodyComponent : public Component {
public:

  // TODO: review these constants
  static constexpr int SIGMA = 12; // Alphabet size for melodies
  static constexpr float DEFAULT_TIMER = 1.0f; // Default timer for matching

  /*
    Adds melody matching capabilities to actor. The tracking resets if
    a wrong note collides or enough time ellapses since last
    collision. After a full match, the component becomes idle.
    Additional Parameters:
    - melody: which melody to track
    - timer: max time between right matches, if timer segunds ellapses since last right note colliding, the tracking resets. Smaller timers result in higher difficulty
   */
  MelodyComponent(class Actor *owner, std::vector<int> melody, float timer = DEFAULT_TIMER);
  ~MelodyComponent() = default;


  // Returns if notes are equal modulo SIGMA
  static bool CompareNotes(const int a, const int b) {
    // NOTE: not using Math::Abs because it returns a float
    return (abs(a - b) % SIGMA) == 0;
  }

  // Returns if NoteActors are equal modulo SIGMA
  static bool CompareNoteActors(const NoteActor &a, const NoteActor &b) {
    return CompareNotes(a.GetNote(), b.GetNote());
  }

  void Update(float deltaTime) override;

  // Returns whether Note matched, should be used for visual feedback
  // Becomes a noop after full match
  // ALERT: destroys Note after collision
  bool OnNoteCollision(NoteActor* note);

  // Returns wheter full melody matched
  bool FullMatch() const { return state == sequence.size(); }

  // Reset tracking
  void Reset() {
    currentTimer = 0.0f;
    state = 0;
  }

  // Change melody, resets tracking afterwards
  void SetMelody(std::vector<int> melody) {
    sequence = melody;
    Reset();
  }
  const std::vector<int>& GetMelody() const {return sequence; }

  // Get current matching percentage
  float GetPercentage() const { return ((float)state) / sequence.size(); }

private:
  std::vector<int> sequence;

  unsigned state = 0; // Tracks where on sequence we are
  float timer;
  float currentTimer = 0.0f; // Tracks how much time left to get next note right
};
