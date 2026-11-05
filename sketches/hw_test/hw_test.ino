#include <Arduino.h>
#include <Wire.h>
#include <ESP_I2S.h>

#define OLED_SDA 4
#define OLED_SCL 15

#define MIC_SCK 5
#define MIC_WS  8
#define MIC_SD  6

#define SPK_BCLK 9
#define SPK_LRC  16
#define SPK_DIN  7

I2SClass mic;
I2SClass spk;

void scanI2c() {
  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);
  Serial.println("I2C scan SDA=4 SCL=15");
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C FOUND 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      found++;
    }
  }
  Serial.print("I2C TOTAL: ");
  Serial.println(found);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== TEST PANTALLA / MIC / PARLANTE ===");

  scanI2c();

  mic.setPort(I2S_NUM_0);
  mic.setPins(MIC_SCK, MIC_WS, -1, MIC_SD);
  if (!mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT)) {
    Serial.println("MIC: fallo al iniciar I2S");
  } else {
    Serial.println("MIC: I2S listo");
  }

  spk.setPort(I2S_NUM_1);
  spk.setPins(SPK_BCLK, SPK_LRC, SPK_DIN);
  if (!spk.begin(I2S_MODE_STD, 24000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("SPK: fallo al iniciar I2S");
  } else {
    Serial.println("SPK: I2S listo");
  }
}

void playBeep(uint16_t freq, uint16_t ms) {
  const uint32_t rate = 24000;
  const uint32_t total = rate * ms / 1000;
  int16_t frame[2];
  for (uint32_t i = 0; i < total; i++) {
    int16_t v = (int16_t)(sinf(2.0f * PI * freq * i / rate) * 7000);
    frame[0] = v;
    frame[1] = v;
    spk.write(frame, sizeof(frame));
  }
}

void loop() {
  static unsigned long lastBeep = 0;
  static unsigned long lastPrint = 0;

  if (millis() - lastBeep > 5000) {
    lastBeep = millis();
    Serial.println("SPK: BEEP 1 kHz");
    playBeep(1000, 500);
  }

  int32_t samples[128];
  size_t bytes = mic.readBytes((char *)samples, sizeof(samples));
  if (bytes > 0 && millis() - lastPrint > 200) {
    lastPrint = millis();
    int32_t peak = 0;
    int n = bytes / sizeof(int32_t);
    for (int i = 0; i < n; i++) {
      int32_t v = samples[i] >> 14;
      if (v < 0) v = -v;
      if (v > peak) peak = v;
    }
    Serial.print("MIC peak: ");
    Serial.println(peak);
  }
}
