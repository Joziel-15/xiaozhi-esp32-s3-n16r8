#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define SCREEN_W 128
#define SCREEN_H 64

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
bool oledReady = false;

static int scanI2C() {
  int total = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C FOUND 0x%02X\n", addr);
      total++;
    }
  }
  Serial.printf("I2C TOTAL=%d\n", total);
  return total;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== OLED SIMPLE TEST ===");
  Serial.printf("PINS SDA=%d SCL=%d\n", OLED_SDA, OLED_SCL);

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(50000);

  scanI2C();

  oledReady = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.printf("BEGIN 0x3C = %d\n", oledReady ? 1 : 0);
  if (!oledReady) {
    oledReady = display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    Serial.printf("BEGIN 0x3D = %d\n", oledReady ? 1 : 0);
  }

  if (!oledReady) {
    Serial.println("OLED NO RESPONDE");
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED OK");
  display.setCursor(0, 16);
  display.printf("SDA=%d SCL=%d", OLED_SDA, OLED_SCL);
  display.setCursor(0, 32);
  display.println("TEST SIMPLE");
  display.display();
  Serial.println("OLED INICIALIZADO");
  delay(1500);
}

void loop() {
  if (!oledReady) {
    scanI2C();
    delay(2000);
    return;
  }

  display.clearDisplay();
  display.drawRect(0, 0, SCREEN_W, SCREEN_H, SSD1306_WHITE);
  display.drawLine(0, 0, SCREEN_W - 1, SCREEN_H - 1, SSD1306_WHITE);
  display.drawLine(0, SCREEN_H - 1, SCREEN_W - 1, 0, SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(24, 24);
  display.print("TEST");
  display.display();
  Serial.println("DRAW ON");
  delay(1000);

  display.clearDisplay();
  display.display();
  Serial.println("DRAW OFF");
  delay(1000);
}
