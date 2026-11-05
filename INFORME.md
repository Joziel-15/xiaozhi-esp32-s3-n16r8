# Informe tecnico

## Equipo

- Placa: ESP32-S3-N16R8, 16 MB flash, 8 MB PSRAM.
- MAC: `<MAC>`
- UUID: `<UUID>`
- SKU: `bread-compact-wifi`
- XiaoZhi: 2.5.3
- Puerto: `/dev/ttyACM0` (CH343, `1a86:55d3`)

## Firmware

- ESP-IDF 6.0
- Arduino core 3.3.12 para las pruebas
- Board: `CONFIG_BOARD_TYPE_BREAD_COMPACT_WIFI=y`
- Pantalla: `CONFIG_OLED_SSD1306_128X64=y`
- Particiones: `partitions/v2/16m.csv`
- Idioma: `es-ES`
- Wake word: `CONFIG_SR_WN_WN9_HIESP=y` ("Hi ESP")

## Pines

| Funcion | GPIO |
|---|---|
| OLED SDA | 4 |
| OLED SCL | 15 |
| MIC WS | 8 |
| MIC SCK | 5 |
| MIC DIN/SD | 6 |
| SPK DOUT | 7 |
| SPK BCLK | 9 |
| SPK LRCK | 16 |
| BOOT | 0 |
| TOUCH | 47 |
| VOL+ | 40 |
| VOL- | 39 |
| LAMP | 18 |
| LED | 48 (desactivado) |

## Cambios realizados

- OLED movido a SDA=4 / SCL=15.
- Microfono movido a WS=8 / SCK=5 / DIN=6.
- Parlante movido a BCLK=9 / LRCK=16 / DOUT=7.
- I2C del OLED a 50 kHz, timeout de 1000 ms, sin pull-up interno.
- DNS de respaldo 8.8.8.8 / 1.1.1.1 al conectar WiFi.
- LED integrado reemplazado por `NoLed`.
- 25 alias de emociones en `oled_display.cc`.
- Demo de 21 expresiones con pulsacion larga de BOOT.
- Log del codigo de activacion.

## Resultados de pruebas

### Pantalla

- Con el firmware XiaoZhi arranco correctamente en un momento:
  - `Install SSD1306 driver`
  - `SSD1306 driver installed`
  - `Turning display on`
  - `OledDisplay: Initialize LVGL`
  - `Adding OLED display`
- Despues de mover cables dejo de responder:
  - `panel_ssd1306_init(151): io tx param SSD1306_CMD_SET_MULTIPLEX failed`
  - `Failed to initialize display: ESP_ERR_INVALID_RESPONSE`
- Escaneo I2C en el ESP32:
  - `I2C TOTAL=0` en 4/15, 15/4, 13/14 y 14/13.
- Prueba en Arduino Uno:
  - `I2C FOUND 0x3C`
  - `BEGIN 0x3C=1`
  - `OLED INICIALIZADO`
  - Conclusion: el modulo OLED esta bien; el problema esta en el lado del ESP32.

### Audio

- Parlante:
  - `SPK BEGIN=1`
  - Se reprodujeron 3 pitidos de 1 kHz.
- Microfono:
  - `MIC BEGIN=1`
  - `BYTES=1024 N=256 RAW0=0x00000000 RAW1=0x00000000 PEAK=0`
  - Probado en 16/32 bits, mono/estereo y en GPIO 8/5/6 y 10/11/12.
  - Conclusion: el INMP441 no entrega datos.

### Alimentacion

- 5V del ESP32: **1.4V**.
- 3.3V: arranca en 3.3V y cae a los pocos segundos.
- El OLED funciona en el Arduino, por lo que no es el modulo.
- Hipotesis principal: fuente/cable USB insuficiente, corto en algun modulo o regulador de la placa dañado.

## Diagnostico

El sintoma principal es la caida del riel de 5V a 1.4V. Sin 5V estables, el regulador de 3.3V no puede mantener la pantalla ni el microfono. El modulo OLED esta sano; el microfono no entrega datos probablemente por falta de alimentacion o por un corto.

## Siguientes pasos

1. Desconectar todos los modulos y medir 5V/3.3V solo con USB.
2. Probar otro cable USB, otro puerto y un hub con alimentacion.
3. Conectar un modulo a la vez y medir despues de cada uno.
4. Si el 5V sigue en 1.4V sin modulos, revisar la placa o la fuente.
5. Cuando el riel este estable, volver a probar pantalla y microfono.
6. Si el microfono sigue en cero con 3.3V estables, probar otro INMP441.

## Archivos de prueba

- `sketches/oled_simple`: prueba de pantalla en ESP32.
- `sketches/oled_uno`: prueba de pantalla en Arduino Uno/Nano.
- `sketches/audio_test`: pitidos + lectura de microfono.
- `sketches/i2c_scan_loop`: escaneo I2C continuo.
- `sketches/i2c_recover`: recuperacion de bus I2C.
- `sketches/mic_raw`: lectura cruda del microfono.
- `sketches/mic_probe`: prueba de 16/32 bits.
- `sketches/hw_test`: prueba combinada I2C/mic/parlante.

## Logs

- `logs/idf-flash-final.log`: flasheo final.
- `logs/xiaozhi-after-mic-test.log`: arranque de XiaoZhi.
- `logs/audio-test-serial.log`: pitidos y microfono.
- `logs/oled-uno-serial.log`: prueba exitosa en Arduino.
- `logs/i2c-recover-serial.log`: escaneo y recuperacion I2C.
- `logs/mic-raw-serial.log`: lectura cruda del microfono.
