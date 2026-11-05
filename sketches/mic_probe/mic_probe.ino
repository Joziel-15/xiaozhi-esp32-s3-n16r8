#include <Arduino.h>
#include <ESP_I2S.h>
#include <math.h>

#define MIC_SCK 5
#define MIC_WS  8
#define MIC_SD  6

#define SPK_BCLK 9
#define SPK_LRC  16
#define SPK_DIN  7

I2SClass mic;
I2SClass spk;

static int16_t beepBuf[640];
static int16_t toneBuf[512];
static unsigned long lastBeep = 0;
static unsigned long modeStart = 0;
static int mode = 0;

static const char *modeName(int m) {
  switch (m) {
    case 0: return "16BIT_MONO_LEFT";
    case 1: return "16BIT_STEREO";
    case 2: return "32BIT_MONO_LEFT";
    default: return "32BIT_STEREO";
  }
}

static bool startMode(int m) {
  mic.end();
  mic.setPort(I2S_NUM_0);
  mic.setPins(MIC_SCK, MIC_WS, -1, MIC_SD);
  if (m == 0) {
    return mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT);
  }
  if (m == 1) {
    return mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
  }
  if (m == 2) {
    return mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT);
  }
  return mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO);
}

static void beep() {
  unsigned long now = millis();
  if (now - lastBeep < 500) return;
  lastBeep = now;
  for (int i = 0; i < 640; i++) {
    int16_t s = (int16_t)(12000.0f * sinf(2.0f * PI * 1000.0f * i / 16000.0f));
    beepBuf[i] = s;
  }
  spk.write((char *)beepBuf, sizeof(beepBuf));
}

static int32_t readPeak16Mono() {
  size_t bytes = mic.readBytes((char *)toneBuf, sizeof(toneBuf));
  int n = bytes / sizeof(int16_t);
  int32_t peak = 0;
  for (int i = 0; i < n; i++) {
    int32_t v = toneBuf[i];
    if (v < 0) v = -v;
    if (v > peak) peak = v;
  }
  return peak;
}

static int32_t readPeak16Stereo() {
  size_t bytes = mic.readBytes((char *)toneBuf, sizeof(toneBuf));
  int n = bytes / sizeof(int16_t);
  int32_t peak = 0;
  for (int i = 0; i < n; i++) {
    int32_t v = toneBuf[i];
    if (v < 0) v = -v;
    if (v > peak) peak = v;
  }
  return peak;
}

static int32_t readPeak32Mono() {
  static int32_t s32[256];
  size_t bytes = mic.readBytes((char *)s32, sizeof(s32));
  int n = bytes / sizeof(int32_t);
  int32_t peak = 0;
  for (int i = 0; i < n; i++) {
    int32_t v = s32[i] >> 16;
    if (v < 0) v = -v;
    if (v > peak) peak = v;
  }
  return peak;
}

static int32_t readPeak32Stereo() {
  static int32_t s32[256];
  size_t bytes = mic.readBytes((char *)s32, sizeof(s32));
  int n = bytes / sizeof(int32_t);
  int32_t peak = 0;
  for (int i = 0; i < n; i++) {
    int32_t v = s32[i] >> 16;
    if (v < 0) v = -v;
    if (v > peak) peak = v;
  }
  return peak;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== MIC PROBE 16/32 BIT ===");

  spk.setPort(I2S_NUM_1);
  spk.setPins(SPK_BCLK, SPK_LRC, SPK_DIN);
  if (!spk.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("SPK: FAIL");
  } else {
    Serial.println("SPK: OK");
  }

  if (!startMode(mode)) {
    Serial.printf("MIC %s: FAIL\n", modeName(mode));
  } else {
    Serial.printf("MIC %s: OK\n", modeName(mode));
  }
  modeStart = millis();
  Serial.println("TOCA O SOPLA EL MICROFONO. Si funciona, oiras un pitido.");
}

void loop() {
  if (millis() - modeStart > 5000) {
    mode = (mode + 1) % 4;
    if (!startMode(mode)) {
      Serial.printf("MIC %s: FAIL\n", modeName(mode));
    } else {
      Serial.printf("MIC %s: OK\n", modeName(mode));
    }
    modeStart = millis();
    delay(300);
    return;
  }

  int32_t peak = 0;
  switch (mode) {
    case 0: peak = readPeak16Mono(); break;
    case 1: peak = readPeak16Stereo(); break;
    case 2: peak = readPeak32Mono(); break;
    default: peak = readPeak32Stereo(); break;
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 250) {
    lastPrint = millis();
    Serial.printf("MODE=%s PEAK=%d\n", modeName(mode), peak);
  }

  if (peak > 2000) {
    beep();
  }
}
