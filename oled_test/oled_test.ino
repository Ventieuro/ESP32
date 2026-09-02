// Test display OLED 0,96" 128x64 (driver SSD1306, I2C).
// Librerie richieste: "Adafruit SSD1306" + "Adafruit GFX Library"
//   (Gestore librerie Arduino, oppure: arduino-cli lib install "Adafruit SSD1306")
//
// Collegamenti: VCC->3V3  GND->GND  SDA->GPIO21  SCL->GPIO22
// Monitor seriale a 115200.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64      // <-- 64, non 32
#define OLED_ADDR    0x3C     // se l'I2C scanner mostra 0x3D, cambia qui

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);          // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display non trovato: controlla cablaggio e indirizzo I2C");
    for (;;) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("ESP32 OK");
  display.setTextSize(1);
  display.println();
  display.println("Display SSD1306 128x64");
  display.display();
  Serial.println("Display inizializzato");
}

void loop() {
  static uint32_t n = 0;
  display.fillRect(0, 48, SCREEN_WIDTH, 16, SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(0, 48);
  display.printf("count %lu", n++);
  display.display();
  delay(500);
}
