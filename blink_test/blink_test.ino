// Test di base per ESP32 DevKit V1
// - Fa lampeggiare il LED integrato (GPIO 2)
// - Stampa lo stato sul monitor seriale (115200 baud)

#define LED_PIN 2

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("ESP32 avviato - test blink");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(500);
}
