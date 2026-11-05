# Firmware

Binarios del build `xiaozhi.bin` con los cambios de `bread-compact-wifi`.

## Flasheo

```bash
./flash.sh /dev/ttyACM0
```

El script usa `python -m esptool` y escribe:

| Offset | Archivo |
|---|---|
| 0x0 | bootloader.bin |
| 0x8000 | partition-table.bin |
| 0xd000 | ota_data_initial.bin |
| 0x20000 | xiaozhi.bin |
| 0x800000 | generated_assets.bin |

Si no tienes `esptool` en el Python del sistema, usa el entorno de ESP-IDF:

```bash
source ~/esp/esp-idf-6.0/export.sh
./flash.sh /dev/ttyACM0
```

## Nota

El equipo tiene un problema de alimentacion: el 5V baja a 1.4V. El flasheo puede funcionar, pero la pantalla y el microfono no funcionaran de forma estable hasta corregir la alimentacion.
