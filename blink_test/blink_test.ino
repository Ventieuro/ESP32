// Test di base per ESP32 DevKit V1
// - Fa lampeggiare il LED integrato (GPIO 2)
// - Stampa lo stato sul monitor seriale (115200 baud)

#define LED_PIN 2
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64      // <-- 64, non 32
#define OLED_ADDR    0x3C     // se l'I2C scanner mostra 0x3D, cambia qui

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Wire.begin(21, 22);          // SDA, SCL
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display non trovato: controlla cablaggio e indirizzo I2C");
    for (;;) delay(1000);
  }
  delay(200);
  Serial.println();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("ESP32 pronto");
  display.display();
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  display.println("LED ON");
  display.display();
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  display.println("LED OFF");
  display.display();
  delay(500);
}
