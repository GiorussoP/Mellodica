#include "components/MelodyComponent.hpp"

// Standard Knuth–Morris–Pratt algorithm for string matching
std::vector<int> kmp(std::vector<int> seq) {
  const int n = seq.size();
  std::vector<int> pi(n, 0);
  for (int i = 1, j = 0; i < n; i++) {
    while (j > 0 and !MelodyComponent::CompareNotes(seq[i], seq[j])) j = pi[j-1];
    if (MelodyComponent::CompareNotes(seq[i], seq[j])) j++;
    pi[i] = j;
  }
  return pi;
}

// Standard KMP-Automaton algorithm for online (on text) pattern matching
void computeAutomaton(std::vector<std::vector<int>> &aut,
                      std::vector<int> sequence) {
  const auto pi = kmp(sequence);
  aut.resize(sequence.size());

  for (unsigned i = 0; i < sequence.size(); i++) {
    aut[i].resize(MelodyComponent::SIGMA);
    for (unsigned c = 0; c < MelodyComponent::SIGMA; c++) {
      if (i > 0 and !MelodyComponent::CompareNotes(sequence[i], c))
        aut[i][c] = aut[pi[i-1]][c];
      else
        aut[i][c] = i + (MelodyComponent::CompareNotes(sequence[i], c) == 1);
    }
  }
  
}

MelodyComponent::MelodyComponent(class Actor *owner, std::vector<int> melody,
                                 float timer)
    : Component(owner), sequence(melody)
    , timer(timer) {
}

void MelodyComponent::Update(float deltaTime) {
  if (FullMatch()) return;
  if (currentTimer > 0) {
    currentTimer -= deltaTime;
    currentTimer = Math::Max(0.0f, currentTimer);
    if (currentTimer == 0) {
      // Reset state:
      state = 0;
    }
  } else {
    currentTimer = 0;
    state = 0;
  }

  if (delayTimer > 0) {
    delayTimer -= deltaTime;
    delayTimer = Math::Max(0.0f, delayTimer);
  }
}

bool MelodyComponent::OnNoteCollision(const NoteActor *note) {
  if (FullMatch()) return true;
  // if (delayTimer > 0) return false;
  SDL_Log("Melody Component: received note %d", note->GetNote());
  SDL_Log("Melody Component: current state %d, next note is %d", state, sequence[state]);
  // delayTimer = MelodyComponent::DELAY;
  if (CompareNotes(sequence[state], note->GetNote())) {
    // match!
    state++;
    currentTimer = timer;
    return true;
  } else if (state > 0 and CompareNotes(sequence[state-1], note->GetNote())) {
    currentTimer = timer;
    return true;
  } else {
    // reset!
    currentTimer = 0;
    return false;
  }
}
