// Animazione su OLED SSD1306 128x64.
// I frame sono in anim.h, generato da:  python tools/img2header.py <gif|cartella> -o oled_anim/anim.h -n anim
//
// Collegamenti: VDD->3V3  GND->GND  SDA->GPIO21  SCK->GPIO22
// Librerie: Adafruit SSD1306 + Adafruit GFX

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "anim.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR    0x3C     // 0x3D su alcuni moduli (usa l'i2c_scanner)
#define FRAME_DELAY  60       // ms tra un frame e l'altro (~16 fps)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);          // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display non trovato: controlla cablaggio e indirizzo I2C");
    for (;;) delay(1000);
  }
  display.clearDisplay();
  display.display();
  Serial.printf("Animazione: %d frame\n", ANIM_FRAME_COUNT);
}

void loop() {
  for (int i = 0; i < ANIM_FRAME_COUNT; i++) {
    // l'array di puntatori e' in PROGMEM: va letto con pgm_read_ptr
    const unsigned char* frame =
        (const unsigned char*)pgm_read_ptr(&anim_frames[i]);

    display.clearDisplay();
    display.drawBitmap(0, 0, frame, ANIM_WIDTH, ANIM_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(FRAME_DELAY);
  }
}
