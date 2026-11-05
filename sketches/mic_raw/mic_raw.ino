#include <Arduino.h>
#include <ESP_I2S.h>

#define MIC_SCK 5
#define MIC_WS  8
#define MIC_SD  6

I2SClass mic;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== MIC RAW 32BIT STEREO ===");
  Serial.printf("PINS SCK=%d WS=%d SD=%d\n", MIC_SCK, MIC_WS, MIC_SD);

  mic.setPort(I2S_NUM_0);
  mic.setPins(MIC_SCK, MIC_WS, -1, MIC_SD);
  bool ok = mic.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO);
  Serial.printf("BEGIN=%d\n", ok ? 1 : 0);
  Serial.println("TOCA O SOPLA EL MICROFONO");
}

void loop() {
  static int32_t s[256];
  size_t bytes = mic.readBytes((char *)s, sizeof(s));
  int n = bytes / sizeof(int32_t);

  int32_t minv = 0;
  int32_t maxv = 0;
  int32_t peak = 0;
  for (int i = 0; i < n; i++) {
    int32_t v = s[i];
    if (v < minv) minv = v;
    if (v > maxv) maxv = v;
    int32_t a = v < 0 ? -v : v;
    if (a > peak) peak = a;
  }

  Serial.printf("BYTES=%u N=%d RAW0=0x%08X RAW1=0x%08X MIN=%ld MAX=%ld PEAK=%ld\n",
                (unsigned)bytes, n,
                n > 0 ? (unsigned)s[0] : 0,
                n > 1 ? (unsigned)s[1] : 0,
                (long)minv, (long)maxv, (long)peak);
  delay(250);
}
