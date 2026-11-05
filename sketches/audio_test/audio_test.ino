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

static bool spkOk = false;
static bool micOk = false;

static void playTone(float freq, int ms) {
  if (!spkOk) return;
  int total = 16000 * ms / 1000;
  int16_t buf[512];
  for (int done = 0; done < total;) {
    int chunk = total - done;
    if (chunk > 256) chunk = 256;
    for (int j = 0; j < chunk; j++) {
      int16_t s = (int16_t)(10000.0f * sinf(2.0f * PI * freq * (done + j) / 16000.0f));
      buf[2 * j] = s;
      buf[2 * j + 1] = s;
    }
    spk.write((char *)buf, chunk * 2 * sizeof(int16_t));
    done += chunk;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== TEST AUDIO + BOCINA ===");
  Serial.printf("MIC SCK=%d WS=%d SD=%d\n", MIC_SCK, MIC_WS, MIC_SD);
  Serial.printf("SPK BCLK=%d LRC=%d DIN=%d\n", SPK_BCLK, SPK_LRC, SPK_DIN);

  spk.setPort(I2S_NUM_1);
  spk.setPins(SPK_BCLK, SPK_LRC, SPK_DIN);
  spkOk = spk.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
  Serial.printf("SPK BEGIN=%d\n", spkOk ? 1 : 0);

  if (spkOk) {
    Serial.println("SPK: 3 pitidos de 1 kHz");
    playTone(1000, 200);
    delay(250);
    playTone(1000, 200);
    delay(250);
    playTone(1000, 200);
    delay(300);
  }

  mic.setPort(I2S_NUM_0);
  mic.setPins(MIC_SCK, MIC_WS, -1, MIC_SD);
  micOk = mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO);
  Serial.printf("MIC BEGIN=%d\n", micOk ? 1 : 0);
  Serial.println("TOCA O HABLA AL MICROFONO");
}

void loop() {
  if (!micOk) {
    delay(1000);
    return;
  }

  static int32_t s32[256];
  static int16_t out[512];

  size_t bytes = mic.readBytes((char *)s32, sizeof(s32));
  int n = bytes / sizeof(int32_t);
  if (n <= 0) {
    Serial.println("MIC SIN DATOS");
    delay(250);
    return;
  }

  int32_t peak = 0;
  for (int i = 0; i < n; i++) {
    int32_t v = s32[i] >> 16;
    if (v < 0) v = -v;
    if (v > peak) peak = v;
  }

  if (peak > 2000 && spkOk) {
    for (int i = 0; i < n; i++) {
      int16_t v = (int16_t)(s32[i] >> 16);
      out[2 * i] = v;
      out[2 * i + 1] = v;
    }
    spk.write((char *)out, n * 2 * sizeof(int16_t));
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 250) {
    lastPrint = millis();
    Serial.printf("MIC BYTES=%u N=%d RAW0=0x%08X RAW1=0x%08X PEAK=%ld\n",
                  (unsigned)bytes, n,
                  n > 0 ? (unsigned)s32[0] : 0,
                  n > 1 ? (unsigned)s32[1] : 0,
                  (long)peak);
  }
}
