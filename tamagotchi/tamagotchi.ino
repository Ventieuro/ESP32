// Tamagotchi su ESP32 + OLED SSD1306 128x64.
// Bottoni: GPIO4 = nutri, GPIO16 = gioca, GPIO17 = dormi/sveglia.
// Ogni bottone: una gambetta al pin, l'altra a GND (uso INPUT_PULLUP, niente resistenze).

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pet.h"
#include "ui.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR    0x3C

#define BTN_FEED  4
#define BTN_PLAY  16
#define BTN_SLEEP 17

#define SAVE_INTERVAL_MS 30000UL
#define DEBOUNCE_MS 30UL

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
PetState pet;

unsigned long lastLoopMs = 0;
unsigned long lastSaveMs = 0;

struct Button {
  uint8_t pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChangeMs;
};

Button btnFeed  = { BTN_FEED,  true, true, 0 };
Button btnPlay  = { BTN_PLAY,  true, true, 0 };
Button btnSleep = { BTN_SLEEP, true, true, 0 };

// Ritorna true un solo istante, quando il bottone passa da rilasciato a premuto.
bool buttonPressed(Button &b, unsigned long now) {
  bool reading = digitalRead(b.pin);
  if (reading != b.lastReading) {
    b.lastChangeMs = now;
    b.lastReading = reading;
  }
  bool justPressed = false;
  if (now - b.lastChangeMs > DEBOUNCE_MS && reading != b.stableState) {
    b.stableState = reading;
    justPressed = (b.stableState == LOW); // LOW = premuto (pull-up)
  }
  return justPressed;
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_FEED, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_SLEEP, INPUT_PULLUP);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display non trovato: controlla cablaggio e indirizzo I2C");
    for (;;) delay(1000);
  }

  pet_load(pet);
  Serial.println("Tamagotchi avviato");

  lastLoopMs = millis();
  lastSaveMs = millis();
  ui_draw(display, pet);
}

void loop() {
  unsigned long now = millis();
  unsigned long deltaMs = now - lastLoopMs;
  lastLoopMs = now;

  if (buttonPressed(btnFeed, now)) {
    pet_feed(pet);
    Serial.println("Nutrito");
  }
  if (buttonPressed(btnPlay, now)) {
    pet_play(pet);
    Serial.println("Giocato");
  }
  if (buttonPressed(btnSleep, now)) {
    pet_toggleSleep(pet);
    Serial.println(pet.sleeping ? "Va a dormire" : "Si sveglia");
  }

  pet_tick(pet, deltaMs);

  if (now - lastSaveMs > SAVE_INTERVAL_MS) {
    pet_save(pet);
    lastSaveMs = now;
  }

  ui_draw(display, pet);
  delay(100);
}
