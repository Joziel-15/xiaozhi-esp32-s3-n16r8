#include <Arduino.h>
#include <Wire.h>

#define OLED_SDA 4
#define OLED_SCL 15

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== I2C SCAN LOOP ===");
  Serial.printf("SDA=%d SCL=%d\n", OLED_SDA, OLED_SCL);
  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(50000);
}

void loop() {
  int total = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("FOUND 0x%02X\n", addr);
      total++;
    }
  }
  Serial.printf("TOTAL=%d\n", total);
  delay(2000);
}
