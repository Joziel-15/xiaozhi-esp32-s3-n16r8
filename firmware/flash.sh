#!/usr/bin/env bash
set -euo pipefail

PORT="${1:-/dev/ttyACM0}"
DIR="$(cd "$(dirname "$0")" && pwd)"

cd "$DIR"

python -m esptool --chip esp32s3 -p "$PORT" -b 460800 \
  --before default_reset --after hard_reset write_flash \
  --flash_mode dio --flash_freq 80m --flash_size 16MB \
  0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0xd000 ota_data_initial.bin \
  0x20000 xiaozhi.bin \
  0x800000 generated_assets.bin
