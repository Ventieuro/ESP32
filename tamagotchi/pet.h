#pragma once

// Stato del tamagotchi. Tutti i valori 0-100.
struct PetState {
  int hunger;       // 0 = pieno, 100 = affamatissimo
  int happiness;    // 0 = triste, 100 = felicissimo
  int energy;       // 0 = esausto, 100 = carico
  bool sleeping;
  bool alive;
  unsigned long ageSeconds;
};

enum class Mood {
  Happy,
  Neutral,
  Sad,
  Hungry,
  Sleeping,
  Dead
};

void pet_load(PetState &pet);
void pet_save(const PetState &pet);

// Aggiorna lo stato in base al tempo trascorso (chiamata ad ogni loop()).
void pet_tick(PetState &pet, unsigned long deltaMs);

void pet_feed(PetState &pet);
void pet_play(PetState &pet);
void pet_toggleSleep(PetState &pet);
void pet_reset(PetState &pet);

Mood pet_mood(const PetState &pet);
