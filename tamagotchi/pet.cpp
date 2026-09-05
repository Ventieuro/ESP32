#include "pet.h"
#include <Preferences.h>

static Preferences prefs;
static const char *NS = "tamagotchi";

// Ogni quanti ms passa un "tick" di invecchiamento delle statistiche.
static const unsigned long STAT_TICK_MS = 5000;
static unsigned long statAccumMs = 0;

void pet_load(PetState &pet) {
  prefs.begin(NS, false);
  bool hasSave = prefs.getBool("saved", false);
  if (hasSave) {
    pet.hunger = prefs.getInt("hunger", 30);
    pet.happiness = prefs.getInt("happiness", 70);
    pet.energy = prefs.getInt("energy", 100);
    pet.sleeping = prefs.getBool("sleeping", false);
    pet.alive = prefs.getBool("alive", true);
    pet.ageSeconds = prefs.getULong("age", 0);
  } else {
    pet_reset(pet);
  }
  prefs.end();
}

void pet_save(const PetState &pet) {
  prefs.begin(NS, false);
  prefs.putBool("saved", true);
  prefs.putInt("hunger", pet.hunger);
  prefs.putInt("happiness", pet.happiness);
  prefs.putInt("energy", pet.energy);
  prefs.putBool("sleeping", pet.sleeping);
  prefs.putBool("alive", pet.alive);
  prefs.putULong("age", pet.ageSeconds);
  prefs.end();
}

void pet_reset(PetState &pet) {
  pet.hunger = 30;
  pet.happiness = 70;
  pet.energy = 100;
  pet.sleeping = false;
  pet.alive = true;
  pet.ageSeconds = 0;
}

static int clamp100(int v) {
  if (v < 0) return 0;
  if (v > 100) return 100;
  return v;
}

void pet_tick(PetState &pet, unsigned long deltaMs) {
  if (!pet.alive) return;

  pet.ageSeconds += deltaMs / 1000;
  statAccumMs += deltaMs;
  if (statAccumMs < STAT_TICK_MS) return;
  statAccumMs = 0;

  if (pet.sleeping) {
    // Dormendo recupera energia lentamente, la fame sale piano.
    pet.energy = clamp100(pet.energy + 3);
    pet.hunger = clamp100(pet.hunger + 1);
  } else {
    pet.hunger = clamp100(pet.hunger + 2);
    pet.energy = clamp100(pet.energy - 1);
    // La felicita' scende piu' in fretta se ha fame o e' stanco.
    int drop = 1;
    if (pet.hunger > 70) drop += 1;
    if (pet.energy < 20) drop += 1;
    pet.happiness = clamp100(pet.happiness - drop);
  }

  if (pet.hunger >= 100 && pet.happiness <= 0) {
    pet.alive = false;
  }
}

void pet_feed(PetState &pet) {
  if (!pet.alive) return;
  pet.hunger = clamp100(pet.hunger - 25);
  pet.happiness = clamp100(pet.happiness + 5);
}

void pet_play(PetState &pet) {
  if (!pet.alive || pet.sleeping) return;
  pet.happiness = clamp100(pet.happiness + 15);
  pet.energy = clamp100(pet.energy - 10);
  pet.hunger = clamp100(pet.hunger + 5);
}

void pet_toggleSleep(PetState &pet) {
  if (!pet.alive) return;
  pet.sleeping = !pet.sleeping;
}

Mood pet_mood(const PetState &pet) {
  if (!pet.alive) return Mood::Dead;
  if (pet.sleeping) return Mood::Sleeping;
  if (pet.hunger > 70) return Mood::Hungry;
  if (pet.happiness < 30) return Mood::Sad;
  if (pet.happiness > 60) return Mood::Happy;
  return Mood::Neutral;
}
