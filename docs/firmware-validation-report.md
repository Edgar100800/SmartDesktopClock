# Informe de validación inicial de firmware

Estado: WAITING_FOR_USER — reconfiguración LAN tras prueba del AP protegido

## Hardware detectado

Adaptador USB-serie QinHeng Electronics CH340, VID:PID `1a86:7523`.
Microcontrolador confirmado mediante `esptool chip-id`:

- Chip: ESP8266EX
- Funciones: Wi-Fi, 160 MHz
- Cristal: 26 MHz
- MAC: `48:3f:da:41:5e:30`
- Chip ID: `0x00415e30`

## Puerto

`/dev/ttyUSB0`

Enlace persistente observado:
`/dev/serial/by-id/usb-1a86_USB2.0-Ser_-if00-port0`

## PixelIt

Versión estable oficial más reciente verificada: `2.5.6`.

Archivo original: `firmware_v2.5.6_ESP8266_d1_mini.bin`

Archivo local: `firmware/pixelit_d1_mini.bin`

## SHA256

`dbd119743e237732546541981fd038fe57c4531037c469021f95002f9b601284`

## Resultados

- Detección USB: PASS
- Identificación ESP8266: PASS — ESP8266EX
- Firmware oficial: PASS — PixelIt 2.5.6 para ESP8266 D1 Mini
- Backup de flash: PASS — 4194304 bytes
- Flash: PASS — borrado y escritura a 1000000 baud, hash verificado
- Boot: PASS — servidor HTTP iniciado, sin boot loop observado
- PIXELIT AP: PASS — SSID visible con señal 100%, IP 192.168.4.1
- Portal de configuración: PASS — confirmado manualmente por el usuario
- AP protegido personalizado: PASS — `PIXELIT` anuncia WPA2
- Wi-Fi: PASS — conectado a LAN, RSSI -79 dBm, calidad 42%
- WebUI: PASS — HTTP 200 en 192.168.1.58
- Matrix: NOT RUN
- Orientación: pendiente

## Problemas

La sesión original no había cargado todavía el nuevo grupo `uucp`. La prueba se
ejecutó dentro de una sesión con grupo primario `uucp` y terminó correctamente.

## Conclusión provisional

Las capas USB y ESP8266 están validadas. Firmware oficial descargado, validado
y flasheado. El arranque llegó al hotspot PIXELIT y al servidor HTTP. Pendiente
conexión y portal confirmados manualmente. `FIRMWARE BASIC BOOT = PASS`.
Conexión LAN, WebUI y API validadas. PixelIt reporta versión 2.5.6,
`ESP8266_d1_mini`, IP `192.168.1.58` y matriz configurada como 32x8. Pendiente
validación física completa. El 2026-08-11, una matriz CJMCU 8x8 respondió a los
patrones de API, pero mostró píxeles y colores no solicitados; incluso un bitmap
de 64 ceros dejó LED encendidos. Resultado: FAIL controlado, probable problema
de señal/cableado (DIN, tierra común o nivel lógico). Evidencia en `evidence/` y
procedimiento reproducible en `tools/panel_test.py`.

El montaje fue reajustado posteriormente y la repetición pasó: `off` apagó los
256 LED. Se configuró persistentemente `matrixType: 3` (Tiled 4x 8x8 CJMCU,
Column major), brillo 20/255, brillo automático desactivado y pantallas de
arranque desactivadas. El patrón de cuatro esquinas coincidió con los paneles y
el texto `HOLA` apareció legible, continuo y correctamente orientado. Validación
final 32x8: PASS. Evidencia: `evidence/full-matrix-orientation-type3.jpg` y
`evidence/full-matrix-text-hola.jpg`.

Se instaló después `2.5.6-smart-ui`, que agrega `/simple`: WebUI móvil local sin
dependencias externas, con reloj, texto, Fogata y apagado. Compilación y flash
verificados; configuración 32x8, brillo seguro y zona horaria Perú preservadas.
reconectar PixelIt a la LAN después de la prueba WPA2; después conectar
físicamente una sola matriz CJMCU 8x8 con brillo inicial máximo de 10%.

## Variante protegida

Firmware personalizado: `2.5.6-secure-ap`

SHA256:
`17f46e6cb0c49e9b18210f878ef25b6aa198ab84d62bb545b78b7bf3e413b06e`

El AP temporal `PIXELIT` exige la contraseña privada compilada localmente. La
contraseña no está registrada en este informe.
