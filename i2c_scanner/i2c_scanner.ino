// I2C scanner - elenca gli indirizzi dei dispositivi I2C collegati.
// Utile per verificare che l'OLED sia cablato bene prima di usarlo.
// SDA = GPIO 21, SCL = GPIO 22 (default ESP32). Monitor seriale a 115200.

#include <Wire.h>

void setup() {
  Wire.begin(21, 22);          // SDA, SCL
  Serial.begin(115200);
  delay(300);
  Serial.println("\nI2C scanner - avvio");
}

void loop() {
  int trovati = 0;
  Serial.println("Scansione...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  dispositivo trovato a 0x%02X\n", addr);
      trovati++;
    }
  }
  if (trovati == 0)
    Serial.println("  nessun dispositivo (controlla cablaggio / alimentazione)");
  Serial.println();
  delay(2000);
}
