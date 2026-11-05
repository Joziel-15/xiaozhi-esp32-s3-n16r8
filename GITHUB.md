# Subir a GitHub

Este repositorio local ya esta listo. Falta autenticarte en GitHub y crear el repositorio remoto.

## Opcion 1: con GitHub CLI

Instala `gh` y autenticate:

```bash
sudo pacman -S github-cli
gh auth login
```

Luego, desde esta carpeta:

```bash
gh repo create xiaozhi-esp32-s3-n16r8 --public --source=. --remote=origin --push
```

## Opcion 2: manual

1. Crea un repositorio vacio en <https://github.com/new> llamado `xiaozhi-esp32-s3-n16r8`.
2. Ejecuta:

```bash
git remote add origin https://github.com/TU_USUARIO/xiaozhi-esp32-s3-n16r8.git
git branch -M main
git push -u origin main
```

## Nota

No se subieron credenciales WiFi, NVS ni tokens. Los logs fueron sanitizados (SSID, BSSID, IP, MAC y UUID reemplazados).
