# Conexiones completas

## ESP32-S3-N16R8

```
OLED SSD1306 128x64
  VDD  -> 3V3
  GND  -> GND
  SDA  -> GPIO4
  SCL  -> GPIO15

INMP441
  VDD  -> 3V3
  GND  -> GND
  L/R  -> GND
  WS   -> GPIO8
  SCK  -> GPIO5
  SD   -> GPIO6

MAX98357A
  VIN  -> 5V
  GND  -> GND
  DIN  -> GPIO7
  BCLK -> GPIO9
  LRC  -> GPIO16
  SD   -> 3V3   (si el modulo tiene pin SD)
  GAIN -> al aire

Parlante fisico
  +    -> + del MAX98357A
  -    -> - del MAX98357A
```

## Rieles

- 3V3: OLED VDD, INMP441 VDD, MAX98357A SD.
- 5V: solo MAX98357A VIN.
- GND comun: OLED, INMP441, INMP441 L/R, MAX98357A y ESP32.

## Notas

- Usa las etiquetas de cada modulo, no el orden fisico.
- El OLED y el INMP441 no van a 5V.
- El boton BOOT es el de la placa (GPIO0); no necesita cable.
- Si el modulo OLED tiene pines `VCC/GND/SCL/SDA`, respeta las etiquetas.
- Si el MAX98357A no tiene pin SD/GAIN, ignoralos.

## Prueba en Arduino Uno/Nano

Para probar solo el OLED:

```
OLED VDD -> 3.3V
OLED GND -> GND
OLED SDA -> A4
OLED SCL -> A5
```

En Arduino Mega: SDA -> 20, SCL -> 21.

Precaucion: el Uno/Nano usa I2C de 5V. Alimenta el OLED a 3.3V y usa el modulo con pull-ups a VCC; no alimentes el OLED a 5V salvo que el modulo indique 3.3V-5V.
