#include <Arduino.h>
#include <Wire.h>

#define OLED_SDA 4
#define OLED_SCL 15

static void printLevels(const char *tag) {
  pinMode(OLED_SDA, INPUT_PULLUP);
  pinMode(OLED_SCL, INPUT_PULLUP);
  delayMicroseconds(50);
  Serial.printf("%s SDA=%d SCL=%d\n", tag, digitalRead(OLED_SDA), digitalRead(OLED_SCL));
}

static void busRecover() {
  printLevels("BEFORE");

  pinMode(OLED_SDA, OUTPUT_OPEN_DRAIN);
  pinMode(OLED_SCL, OUTPUT_OPEN_DRAIN);
  digitalWrite(OLED_SDA, HIGH);
  digitalWrite(OLED_SCL, HIGH);
  delayMicroseconds(20);

  for (int i = 0; i < 9; i++) {
    digitalWrite(OLED_SCL, LOW);
    delayMicroseconds(20);
    digitalWrite(OLED_SCL, HIGH);
    delayMicroseconds(20);
  }

  digitalWrite(OLED_SDA, LOW);
  delayMicroseconds(20);
  digitalWrite(OLED_SCL, HIGH);
  delayMicroseconds(20);
  digitalWrite(OLED_SDA, HIGH);
  delayMicroseconds(20);

  pinMode(OLED_SDA, INPUT_PULLUP);
  pinMode(OLED_SCL, INPUT_PULLUP);
  delayMicroseconds(50);
  printLevels("AFTER");
}

static void scan() {
  int total = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("FOUND 0x%02X\n", addr);
      total++;
    }
  }
  Serial.printf("TOTAL=%d\n", total);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== I2C RECOVER + SCAN ===");

  busRecover();

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(50000);
  Serial.println("WIRE_READY");
}

void loop() {
  scan();
  delay(2000);
}
