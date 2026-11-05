# XiaoZhi ESP32-S3 N16R8 - bread-compact-wifi

Proyecto de prueba de XiaoZhi en una placa **ESP32-S3-N16R8** con OLED SSD1306, micrófono INMP441 y parlante MAX98357A.

## Estado actual

- Firmware XiaoZhi compilado y flasheado correctamente.
- Parlante: funciona, reproduce los pitidos de prueba.
- Pantalla: **el módulo OLED está bien** (probado en Arduino Uno, responde en `0x3C`), pero en el ESP32 no responde.
- Micrófono: no entrega datos (`RAW0=0x00000000`, `PEAK=0`).
- Causa probable: **el riel de 5V del ESP32 está inestable**. Se midió **1.4V** en 5V; el 3.3V arranca en 3.3V y cae a los pocos segundos.
- No usar el equipo con ese problema de alimentación hasta revisar cable/fuente/cortos.

## Conexiones

### OLED SSD1306 128x64

| OLED | ESP32-S3 |
|---|---|
| VDD | 3V3 |
| GND | GND |
| SDA | GPIO4 |
| SCL | GPIO15 |

### Micrófono INMP441

| INMP441 | ESP32-S3 |
|---|---|
| VDD | 3V3 |
| GND | GND |
| L/R | GND |
| WS | GPIO8 |
| SCK | GPIO5 |
| SD | GPIO6 |

### Parlante MAX98357A

| MAX98357A | ESP32-S3 |
|---|---|
| VIN | 5V |
| GND | GND |
| DIN | GPIO7 |
| BCLK | GPIO9 |
| LRC | GPIO16 |
| SD | 3V3 (si existe) |
| GAIN | al aire (si existe) |

Todos los GND deben estar unidos. El OLED y el micrófono van a **3.3V**, no a 5V.

## Flashear

Con ESP-IDF:

```bash
source ~/esp/esp-idf-6.0/export.sh
cd xiaozhi-esp32
idf.py -p /dev/ttyACM0 flash
```

Con los binarios incluidos:

```bash
cd firmware
./flash.sh /dev/ttyACM0
```

## Cambios principales

- Pines OLED: `SDA=4`, `SCL=15`.
- Pines micrófono: `WS=8`, `SCK=5`, `DIN=6`.
- Pines parlante: `DOUT=7`, `BCLK=9`, `LRCK=16`.
- DNS de respaldo `8.8.8.8` / `1.1.1.1`.
- LED integrado desactivado (`NoLed`).
- I2C del OLED a 50 kHz, timeout 1000 ms, sin pull-up interno.
- 25 alias de emociones en `oled_display.cc`.
- Demo de 21 expresiones con pulsación larga de BOOT.
- Log del código de activación en `application.cc`.

## Wake word

La palabra de activación del firmware es **"Hi ESP"**. "wi" es el nombre del asistente en la página de Xiaozhi, no la wake word del ESP32.

## Problema de alimentación

Mediciones:

- 5V del ESP32: **1.4V** (debería ser ~5V).
- 3.3V: arranca en 3.3V y cae a los pocos segundos.
- OLED en Arduino Uno: funciona en `0x3C`.

Pasos recomendados:

1. Desconectar OLED, micrófono y parlante.
2. Medir 5V y 3.3V solo con USB.
3. Probar otro cable USB, otro puerto o un hub con alimentación.
4. Conectar un módulo a la vez y medir después de cada uno.
5. Si el 5V sigue en 1.4V sin módulos, revisar la placa/fuente.

## Contenido

- `CONEXIONES.md`: conexiones detalladas.
- `INFORME.md`: informe técnico y resultados de pruebas.
- `CAMBIOS.patch`: diff de los archivos modificados.
- `source/`: copia de los archivos modificados y `sdkconfig`.
- `firmware/`: binarios y script de flasheo.
- `sketches/`: pruebas de OLED, I2C, micrófono y audio.
- `logs/`: logs de las pruebas.
- `GITHUB.md`: cómo subir este repositorio a GitHub.

## Créditos

Basado en [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32). Respeta la licencia del proyecto original.
