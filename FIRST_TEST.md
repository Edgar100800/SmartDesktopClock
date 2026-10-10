# PRD — Validación inicial del firmware del reloj LED 32×8

**Proyecto:** Reloj LED IoT 32×8
**Fase:** 01 — Firmware Bring-Up
**Versión PRD:** 0.1
**Objetivo:** validar el Wemos D1 Mini y PixelIt antes de construir el resto del sistema.

## 1. Contexto

El hardware disponible es:

```text
Microcontrolador:
Wemos D1 Mini
ESP8266

Pantalla final:
4 × CJMCU WS2812B64 8×8

Resolución:
32 × 8

Total:
256 LEDs RGB
```

Para esta primera prueba **NO utilizar las cuatro matrices**.

Empezar con:

```text
PC
 │
 │ USB
 ▼
Wemos D1 Mini
 │
 │ D2 / GPIO4
 ▼
330 Ω
 │
 ▼
DIN
 │
 ▼
1 × CJMCU WS2812B 8×8
```

El firmware base será **PixelIt**.

El proyecto oficial soporta ESP8266/ESP32, WS2812B y una interfaz web/API JSON. La versión publicada actualmente en el repositorio es 2.5.6, pero el agente debe verificar la versión disponible en el momento de ejecutar la tarea y no asumirla ciegamente. ([GitHub][2])

---

# 2. Objetivo de esta fase

Determinar si podemos conseguir la siguiente cadena funcional:

```text
Linux
  │
  │ USB
  ▼
Wemos detectado
  │
  ▼
PixelIt flasheado
  │
  ▼
Firmware inicia
  │
  ▼
Wi-Fi PIXELIT
  │
  ▼
Configuración Wi-Fi
  │
  ▼
WebUI accesible
  │
  ▼
D2 / GPIO4
  │
  ▼
WS2812B 8×8
  │
  ▼
Imagen/hora/patrón visible
```

Al terminar esta fase debemos poder responder:

> ¿El Wemos D1 Mini puede ejecutar correctamente PixelIt y controlar nuestra matriz CJMCU WS2812B?

---

# 3. Restricción principal

**No desarrollar firmware propio todavía.**

No modificar PixelIt salvo que sea necesario para solucionar un problema demostrado.

Primero utilizar:

```text
firmware oficial
        ↓
hardware original
        ↓
configuración oficial
        ↓
prueba
```

Solo si falla, investigar.

---

# 4. Fuentes permitidas

Utilizar únicamente documentación/repositorio oficial:

```text
GitHub:
pixelit-project/PixelIt

Documentation:
pixelit-project.github.io
```

La documentación oficial indica que para un Wemos D1 Mini debe utilizarse el firmware correspondiente a:

```text
*_d1_mini.bin
```

También documenta el procedimiento Linux con `esptool`. ([PixelIt][3])

No descargar `.bin` desde blogs, Google Drive, foros o repositorios de terceros.

---

# 5. Fase A — Inspección del entorno

El agente debe identificar el sistema antes de instalar nada.

Ejecutar y almacenar los resultados de:

```bash
uname -a
```

```bash
python --version
```

```bash
python3 --version
```

```bash
lsusb
```

Y comprobar dispositivos serie:

```bash
ls -la /dev/ttyUSB* 2>/dev/null
ls -la /dev/ttyACM* 2>/dev/null
```

Preferentemente comprobar también:

```bash
ls -la /dev/serial/by-id/ 2>/dev/null
```

### Resultado esperado

Al conectar el Wemos debería aparecer un dispositivo serie parecido a:

```text
/dev/ttyUSB0
```

o equivalente.

**No asumir `/dev/ttyUSB0`.**

Detectarlo dinámicamente.

---

# 6. Fase B — Verificar que realmente sea un ESP8266

Antes de borrar o escribir memoria:

1. identificar el puerto;
2. ejecutar `esptool` contra el dispositivo;
3. obtener información del chip.

El resultado debe identificar un dispositivo de la familia:

```text
ESP8266
```

Si el dispositivo detectado no es ESP8266:

```text
STOP
```

No escribir firmware.

---

# 7. Fase C — Preparar esptool

No contaminar innecesariamente Python global.

Preferencia:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install esptool
```

Verificar instalación:

```bash
esptool version
```

o la sintaxis equivalente soportada por la versión instalada.

El agente debe ejecutar:

```bash
esptool --help
```

antes de construir los comandos definitivos.

---

# 8. Fase D — Obtener firmware

Obtener desde el proyecto oficial la última versión estable compatible con:

```text
Wemos D1 Mini
ESP8266
```

El archivo debe corresponder a:

```text
*_d1_mini.bin
```

La documentación oficial especifica esa variante para el D1 Mini. ([PixelIt][3])

Guardar el firmware dentro del proyecto:

```text
firmware/
└── pixelit_d1_mini.bin
```

Pero conservar también en un archivo de metadata:

```text
URL de origen
versión
fecha de descarga
nombre original
SHA256
```

Calcular:

```bash
sha256sum firmware/*.bin
```

Guardar como:

```text
firmware/SHA256SUMS.txt
```

---

# 9. Fase E — Backup previo

Si técnicamente es posible y el ESP contiene firmware previo, realizar primero un backup de la flash.

Crear:

```text
backup/
└── wemos-original-flash.bin
```

No es necesario interpretar su contenido.

Su objetivo es simplemente disponer de una copia antes del borrado.

Si no puede hacerse de manera fiable, documentarlo y continuar solo después de confirmar que se está trabajando sobre el Wemos correcto.

---

# 10. Fase F — Borrar flash

Una vez confirmado:

```text
ESP8266
+
puerto correcto
+
firmware correcto
```

borrar la flash.

La documentación oficial muestra para Linux un flujo equivalente a:

```bash
esptool.py --port /dev/ttyXXX erase_flash
```

y posteriormente escribe el firmware en la dirección `0`, con flash de 4 MB y modo DIO. ([PixelIt][3])

El agente deberá adaptar la sintaxis a la versión moderna de `esptool` instalada.

---

# 11. Fase G — Flashear PixelIt

Utilizar el firmware:

```text
*_d1_mini.bin
```

Objetivo lógico:

```text
PORT = puerto detectado

FLASH:
offset 0x0
flash size 4 MB
flash mode DIO
```

Como referencia, la documentación oficial utiliza una operación equivalente a:

```text
write flash
baud 1000000
flash_size 4MB
flash_mode dio
offset 0
```

([PixelIt][3])

### Regla

Si falla a baud alto:

```text
1,000,000
```

reintentar utilizando una velocidad más conservadora.

El agente debe registrar qué velocidad funcionó.

---

# 12. Fase H — Verificación post-flash

Después del flasheo:

1. reiniciar Wemos;
2. esperar el boot;
3. inspeccionar salida serial;
4. buscar errores de reset;
5. verificar que el ESP no esté entrando en un ciclo continuo de reinicio.

Registrar logs durante aproximadamente:

```text
30–60 segundos
```

El test debe diferenciar entre:

```text
BOOT OK
```

y

```text
BOOT LOOP
```

---

# 13. Fase I — Detectar hotspot

Si PixelIt no tiene Wi-Fi configurado, debe iniciar un Access Point llamado:

```text
PIXELIT
```

Ese comportamiento está documentado oficialmente. ([PixelIt][3])

El agente debe comprobar, si el sistema lo permite, que aparece el SSID:

```text
PIXELIT
```

Por ejemplo utilizando las herramientas de red disponibles en Linux.

No modificar automáticamente la red principal del usuario si eso puede interrumpir la sesión del agente.

Si necesita que el usuario se conecte manualmente:

```text
STATUS:
WAITING_FOR_USER
```

y proporcionar instrucciones.

---

# 14. CHECKPOINT MANUAL 1

Aquí Codex debe detenerse y pedir al usuario:

> Conéctate desde tu celular o computadora a la red Wi-Fi `PIXELIT` y confirma si aparece el portal de configuración.

Resultado:

```text
[ ] PIXELIT visible
[ ] se puede conectar
[ ] portal abre
```

Si los tres funcionan:

```text
FIRMWARE BASIC BOOT = PASS
```

---

# 15. Fase J — Configuración Wi-Fi

Configurar mediante PixelIt:

```text
SSID
password
```

Después del reinicio, comprobar que el dispositivo se conecta a la LAN.

PixelIt documenta que después de ingresar las credenciales de Wi-Fi se conecta a Internet y permite acceder a su interfaz web. ([PixelIt][3])

Intentar resolver:

```text
pixelit
```

y, si no funciona, determinar la IP por DHCP/mDNS o mediante el mecanismo disponible en la red.

---

# 16. Fase K — Verificar WebUI

Probar:

```text
http://pixelit/
```

y/o:

```text
http://<IP-DEL-WEMOS>/
```

Resultado requerido:

```text
HTTP reachable
PixelIt WebUI loads
```

Guardar:

```text
IP
hostname
firmware version
```

---

# 17. Preparación de hardware para primera matriz

Antes de conectar LEDs:

```text
Wemos D1 Mini

D2 / GPIO4
      │
    330 Ω
      │
      ▼
DIN matrix
```

Alimentación:

```text
FUENTE 5V
   │
   ├──── +5V MATRIX
   │
   └──── GND MATRIX
             │
             └──── GND WEMOS
```

Capacitor:

```text
       +5V
        │
        │ +
      ┌─┴─┐
      │   │ 1000 µF
      └─┬─┘
        │ -
        │
       GND
```

Usar **solo una matriz CJMCU 8×8** en esta prueba.

La documentación de PixelIt define para ESP8266:

```text
D2 → DIN
GND → GND
5V → VCC
```

y recomienda un capacitor de 1000 µF; también lista específicamente las matrices CJMCU 8×8 como hardware compatible. ([PixelIt][1])

---

# 18. Restricción de brillo

Para el primer encendido:

```text
BRIGHTNESS <= 10%
```

No realizar pruebas de:

```text
255,255,255
100% brightness
64 LEDs simultáneamente
```

en la primera validación.

El objetivo es verificar datos, no consumo máximo.

---

# 19. Fase L — Primera prueba visual

Desde PixelIt WebUI utilizar las herramientas de test.

Intentar mostrar algo simple:

```text
un color
```

o:

```text
un patrón
```

o:

```text
texto
```

El repositorio oficial indica que PixelIt incluye WebUI con configuración y pequeñas pruebas de pantalla. ([GitHub][2])

---

# 20. Test de color

Ejecutar sucesivamente:

```text
ROJO
VERDE
AZUL
```

Resultado esperado:

```text
RED    → LEDs rojos
GREEN  → LEDs verdes
BLUE   → LEDs azules
```

Esto permite comprobar:

```text
alimentación
data
orden RGB
driver WS2812B
```

---

# 21. Test de píxel

Si la WebUI/API permite seleccionar píxeles, probar:

```text
pixel 0
```

Después:

```text
pixel siguiente
```

y verificar visualmente la dirección de recorrido.

Documentar:

```text
start pixel location:
top-left / top-right / bottom-left / bottom-right

direction:
horizontal / vertical

layout:
linear / serpentine
```

---

# 22. CHECKPOINT MANUAL 2

Codex debe pedir:

> ¿Qué ves físicamente en la matriz?

Opciones:

```text
A. funciona correctamente
B. colores incorrectos
C. LEDs desordenados
D. todos apagados
E. algunos LEDs funcionan
F. Wemos se reinicia
```

No asumir que el hardware funciona basándose únicamente en que la API respondió HTTP 200.

---

# 23. Configuración futura de las cuatro matrices

**NO ejecutar todavía**, pero verificar que el firmware ofrece soporte para el layout:

```text
4 × CJMCU 8×8
```

El historial oficial de PixelIt incluye soporte específico para cuatro matrices CJMCU 8×8 y posteriormente para el layout tiled/column-major. ([GitHub][2])

No hardcodear un ID numérico de tipo de matriz sin verificarlo primero en la versión actual.

Buscar una opción descriptiva similar a:

```text
4x 8x8 CJMCU
Tiled CJMCU
Column Major
```

---

# 24. Casos de prueba

| ID     | Prueba           | Resultado esperado      |
| ------ | ---------------- | ----------------------- |
| FW-01  | Detectar USB     | Puerto serie encontrado |
| FW-02  | Identificar chip | ESP8266                 |
| FW-03  | Obtener firmware | `_d1_mini.bin`          |
| FW-04  | Flash            | Sin errores             |
| FW-05  | Boot             | Sin boot loop           |
| FW-06  | AP               | SSID `PIXELIT` visible  |
| FW-07  | Setup Wi-Fi      | Se conecta a LAN        |
| FW-08  | WebUI            | HTTP accesible          |
| LED-01 | D2 → DIN         | Matriz responde         |
| LED-02 | Rojo             | Rojo correcto           |
| LED-03 | Verde            | Verde correcto          |
| LED-04 | Azul             | Azul correcto           |
| LED-05 | Pixel test       | Dirección identificada  |

---

# 25. Criterio de aceptación

La Fase 1 se considera **PASS** únicamente si:

```text
✓ Wemos detectado como ESP8266
✓ firmware PixelIt oficial instalado
✓ firmware arranca sin boot-loop
✓ hotspot PIXELIT funciona
✓ Wemos se conecta al Wi-Fi
✓ WebUI es accesible
✓ una matriz WS2812B recibe datos por D2
✓ LEDs pueden mostrar al menos RGB
✓ dirección/orientación de la matriz está documentada
```

Resultado:

```text
FIRMWARE VALIDATION: PASS
```

---

# 26. Criterios de fallo

Generar:

```text
FIRMWARE VALIDATION: FAIL
```

si ocurre alguno:

```text
ESP no detectado
firmware incorrecto
flash falla persistentemente
boot loop
PIXELIT no aparece
WebUI no responde
D2 no genera salida funcional
matriz no responde
Wemos se reinicia al activar LEDs
```

Pero debe identificarse **en qué capa falló**:

```text
USB
│
├── ESP8266
├── firmware
├── Wi-Fi
├── WebUI
├── GPIO
├── alimentación
└── matriz
```

---

# 27. No hacer durante esta fase

No:

```text
× conectar las 4 matrices
× construir carcasa
× modificar PixelIt
× programar reloj propio
× integrar clima
× MQTT
× Home Assistant
× sensores
× animaciones complejas
× brillo máximo
× OTA
```

Todo eso pertenece a fases posteriores.

---

# 28. Entregables del agente

Crear:

```text
docs/
└── firmware-validation-report.md

logs/
├── environment.txt
├── esptool-chip-info.txt
├── flash.log
└── serial-boot.log

firmware/
├── pixelit_d1_mini.bin
└── SHA256SUMS.txt

backup/
└── wemos-original-flash.bin
```

El `firmware-validation-report.md` debe contener:

```text
Hardware detectado:
ESP8266 ...

Puerto:
...

PixelIt:
version ...

SHA256:
...

Flash:
PASS / FAIL

Boot:
PASS / FAIL

PIXELIT AP:
PASS / FAIL

Wi-Fi:
PASS / FAIL

WebUI:
PASS / FAIL

Matrix:
PASS / FAIL

Orientación:
...

Problemas:
...

Conclusión:
...
```

---

# 29. Instrucción importante para Codex

Puedes agregar al final exactamente esto:

```text
IMPORTANTE:

Trabaja incrementalmente.

No ejecutes operaciones de escritura sobre ningún dispositivo
hasta confirmar que el puerto corresponde al Wemos D1 Mini ESP8266.

No modifiques el código fuente de PixelIt durante esta fase.

Utiliza primero el firmware binario oficial para Wemos D1 Mini.

Cuando una prueba necesite observación física del LED, detente y
pregunta al usuario qué está viendo.

No marques una prueba de hardware como PASS basándote únicamente
en software.

Después de cada etapa registra comandos, stdout, stderr y resultado.

Si encuentras diferencias entre este PRD y la documentación oficial
actual de PixelIt, utiliza la documentación oficial como fuente de
verdad y documenta la diferencia.

El objetivo NO es terminar el reloj.

El objetivo es demostrar:

Wemos D1 Mini
      +
PixelIt
      +
Wi-Fi
      +
D2
      +
1 matriz WS2812B
      =
FUNCIONAL
```

### Qué debería pasar al terminar

Si todo sale bien, tendremos nuestro primer **milestone real**:

```text
                  INTERNET
                     │
                   Wi-Fi
                     │
                     ▼
              Wemos D1 Mini
              PixelIt ✓
                     │
                    D2
                     │
                  330 Ω
                     │
                     ▼
                ┌────────┐
                │ WS2812B│
                │  8 × 8 │
                └────────┘
                     ✓
```

Y recién ahí avanzaría al **PRD 02: convertir las cuatro matrices 8×8 en una única pantalla lógica 32×8**, donde verificaremos el encadenamiento `DOUT → DIN`, la orientación serpentina, los 256 píxeles y finalmente mostraremos la hora.

[1]: https://pixelit-project.github.io/hardware.html?utm_source=chatgpt.com "Hardware"
[2]: https://github.com/pixelit-project/PixelIt?utm_source=chatgpt.com "PixelIt - The Matrix Display"
[3]: https://pixelit-project.github.io/firmware.html?utm_source=chatgpt.com "Firmware"

---

# 30. Estado de ejecución — 2026-08-11

Esta sección registra el estado real alcanzado. Es el punto de reanudación para
la siguiente sesión.

## 30.1 Resumen

```text
Fase A  Entorno                         PASS
Fase B  Identificación ESP8266          PASS
Fase C  esptool                         PASS
Fase D  Firmware oficial                PASS
Fase E  Backup de flash                 PASS
Fase F  Borrado de flash                PASS
Fase G  Flash PixelIt oficial           PASS
Fase H  Boot sin reinicio continuo      PASS
Fase I  Hotspot PIXELIT                 PASS
Manual  Portal inicial                  PASS
Fase J  Primera conexión LAN            PASS
Fase K  WebUI/API                       PASS
Extra   Hotspot PIXELIT protegido WPA2  PASS
Fase L  Prueba visual de matriz         NOT RUN
```

Resultado parcial:

```text
FIRMWARE BASIC BOOT = PASS
FIRMWARE VALIDATION = IN PROGRESS
```

La validación completa no puede marcarse como `PASS` hasta probar físicamente
una matriz CJMCU WS2812B 8x8.

## 30.2 Hardware confirmado

```text
Puerto serie:        /dev/ttyUSB0
Enlace persistente:  /dev/serial/by-id/usb-1a86_USB2.0-Ser_-if00-port0
USB-Serie:           QinHeng CH340, 1a86:7523
Microcontrolador:    ESP8266EX
Flash:               4 MB
Chip ID:             0x00415e30
MAC:                 48:3f:da:41:5e:30
```

El acceso serie requiere pertenecer al grupo Linux `uucp`. El usuario ya fue
añadido. Si una sesión antigua no reconoce el grupo, cerrar sesión y volver a
entrar, o ejecutar la herramienta dentro de una sesión `newgrp uucp`.

## 30.3 Firmware oficial validado

Última versión estable verificada el 2026-08-11:

```text
PixelIt:       2.5.6
Build:         ESP8266_d1_mini
Archivo local: firmware/pixelit_d1_mini.bin
Tamaño:        498992 bytes
SHA256:        dbd119743e237732546541981fd038fe57c4531037c469021f95002f9b601284
```

Fuente oficial:

```text
https://github.com/pixelit-project/PixelIt/releases/tag/2.5.6
```

La imagen fue validada mediante `esptool image-info`: ESP8266, flash 4 MB,
modo DIO y checksum interno válido.

## 30.4 Backup recuperable

Antes del primer borrado se leyó la flash completa:

```text
Archivo: backup/wemos-original-flash.bin
Tamaño:  4194304 bytes
SHA256:  90a68dbd7e7cf2920c3332017884d2177abdd162480ab71817e4c4adbabe5a0e
```

No eliminar este archivo. Permite restaurar el contenido original del Wemos.

## 30.5 Primera validación oficial

El firmware oficial fue escrito correctamente a `0x0`:

```text
Baud:              1000000
Flash size:        4 MB
Flash mode:        DIO
Verificación hash: PASS
Boot loop:         no observado
AP inicial:        PIXELIT
Portal inicial:    http://192.168.4.1:80
WebUI LAN:         HTTP 200
Primera IP LAN:    192.168.1.58
```

La IP LAN fue asignada por DHCP y puede cambiar después de cada reconexión. No
asumir que `192.168.1.58` sigue vigente; localizar el dispositivo por su MAC.

## 30.6 Desviación autorizada: AP protegido

Después de completar la prueba básica oficial, el usuario autorizó
explícitamente modificar PixelIt para proteger el hotspot temporal. Esta es una
excepción consciente a la restricción inicial de no modificar código fuente.

Cambio mínimo aplicado sobre el tag oficial `2.5.6`:

```cpp
wifiManager.autoConnect(PIXELIT_AP_SSID, PIXELIT_AP_PASSWORD);
```

Estado:

```text
Fuente local:      pixelit-firmware/
Versión reportada: 2.5.6-secure-ap
Binario:           firmware/pixelit_d1_mini_secure_ap.bin
Tamaño:            499024 bytes
SHA256:            17f46e6cb0c49e9b18210f878ef25b6aa198ab84d62bb545b78b7bf3e413b06e
Compilación:       PASS
Flash:             PASS
WebUI post-flash:  PASS
SSID:              PIXELIT
Seguridad:         WPA2
```

La contraseña está únicamente en:

```text
pixelit-firmware/include/LocalSecrets.h
```

Reglas:

- no copiar la contraseña a este PRD, logs o reportes;
- `LocalSecrets.h` está ignorado por Git;
- el compilador exige una longitud de 8 a 63 caracteres;
- `LocalSecrets.h.example` contiene solo una plantilla sin secreto real.

El cambio protege únicamente el hotspot de configuración `PIXELIT`. No agrega
autenticación a WebUI ni REST API dentro de la LAN.

## 30.7 Estado exacto al pausar

Para verificar WPA2 se borraron intencionalmente las credenciales Wi-Fi usando:

```text
POST /api/wifireset
```

La red `PIXELIT` reapareció con señal 100% y seguridad WPA2. El usuario indicó
que ya se conectó a ella. El portal utiliza HTTP puerto 80:

```text
http://192.168.4.1/
```

No usar un puerto distinto. Si el portal cautivo no abre automáticamente,
escribir esa URL manualmente.

## 30.8 Próximos pasos exactos

Continuar en este orden:

1. Abrir `http://192.168.4.1/` mientras se está conectado a `PIXELIT`.
2. Seleccionar la red Wi-Fi local e ingresar su contraseña dentro del portal.
3. Guardar y esperar el reinicio del Wemos.
4. Reconectar la computadora a la red Wi-Fi local.
5. Buscar la nueva IP usando la MAC `48:3f:da:41:5e:30`.
6. Verificar `GET http://<IP>/api/matrixinfo`.
7. Confirmar versión `2.5.6-secure-ap` y WebUI HTTP 200.
8. Con alimentación desconectada, cablear una sola matriz CJMCU 8x8.
9. Mantener brillo máximo inicial de 10%.
10. Ejecutar tests rojo, verde, azul y píxel; pedir observación física al
    usuario antes de marcar resultados.

Cableado pendiente:

```text
Wemos D2 / GPIO4 ---- 330 ohm ---- DIN matriz
Fuente +5V ----------------------- +5V matriz
Fuente GND ----------------------- GND matriz
Fuente GND ----------------------- GND Wemos
Capacitor 1000 uF entre +5V y GND, respetando polaridad
```

No conectar las cuatro matrices todavía.

## 30.9 Evidencia y herramientas locales

```text
docs/firmware-validation-report.md
logs/environment.txt
logs/esptool-chip-info.txt
logs/backup.txt
logs/flash.log
logs/serial-boot.log
logs/secure-ap-validation.txt
firmware/METADATA.md
firmware/SHA256SUMS.txt
backup/wemos-original-flash.bin
```

Entorno Python y herramientas:

```text
.venv/
esptool 5.3.1
PlatformIO 6.1.19
```

CodeGraph está inicializado para el proyecto y fue usado para localizar el
punto de configuración Wi-Fi. Al pausar, el índice útil contiene el código
fuente PixelIt; antes de análisis posterior ejecutar `codegraph sync .` si hay
cambios nuevos.

## 30.10 Diferencia encontrada en documentación oficial

La documentación oficial describe `GET /api/wifireset`, pero el código fuente
PixelIt 2.5.6 registra esa ruta como `HTTP_POST`. La operación funcional usada
fue:

```text
POST /api/wifireset
```

El código fuente de la versión probada se considera fuente de verdad para esta
diferencia.

---

# 31. Prueba física de una matriz 8x8 — 2026-08-11

Se desarrolló la herramienta reproducible `tools/panel_test.py`. Envía pruebas a
`POST /api/screen` con brillo fijo `20/255` (7.8%) y como máximo ocho LED
solicitados simultáneamente. No modifica la configuración Wi-Fi ni el firmware.

Estado confirmado antes de probar:

```text
IP:                 192.168.1.58
MAC:                48:3f:da:41:5e:30
Firmware:           2.5.6-secure-ap
API:                PASS
Matriz configurada: 32x8, matrixType 1 (Column major)
Matriz física:      una CJMCU 8x8
Brillo de prueba:   20/255 (7.8%)
```

Pruebas enviadas y aceptadas por la API:

```text
diagonal roja:       HTTP/API PASS
diagonal verde:      HTTP/API PASS
diagonal azul:       HTTP/API PASS
píxel lógico 0:      HTTP/API PASS
píxel lógico 1:      HTTP/API PASS
apagado (64 ceros):  HTTP/API PASS
```

Resultado visual: **FAIL controlado**. Los patrones cambian la matriz, por lo que
hay alimentación y alguna comunicación de datos. Sin embargo, aparecen píxeles
adicionales y colores no solicitados. El comando de apagado deja varios LED
encendidos. Por ello todavía no se puede validar orden RGB, píxel inicial ni
recorrido.

Evidencia:

```text
evidence/panel-red.jpg
evidence/panel-green.jpg
evidence/panel-blue.jpg
evidence/panel-pixel-0.jpg
evidence/panel-pixel-1.jpg
evidence/panel-off.jpg
```

Diagnóstico actual, en orden de prioridad:

1. Verificar con alimentación desconectada que D2/GPIO4 llegue a **DIN**, no
   `DOUT`.
2. Verificar tierra común entre Wemos, matriz y fuente de 5 V.
3. Confirmar resistencia serie de 330 ohmios en datos y condensador de 1000 uF
   entre 5 V y GND con polaridad correcta.
4. Acortar cable de datos. Si persiste, usar conversor de nivel 3.3 V a 5 V
   (74AHCT125/74HCT14); la señal de 3.3 V puede ser marginal con matriz a 5 V.
5. Después de corregir señal, repetir `off`, RGB y píxeles. Solo entonces evaluar
   `matrixType` y orientación lógica.

No conectar las cuatro matrices ni aumentar brillo hasta que `off` apague los 64
LED de forma fiable.

---

# 32. Corrección y validación de cuatro matrices — 2026-08-11

Después de reajustar físicamente el montaje, se conectaron las cuatro matrices
CJMCU 8x8 en una fila (32x8). La repetición del comando `off` apagó correctamente
los 256 LED. El fallo de señal documentado en la sección 31 quedó resuelto; no se
registró qué conexión física cambió, por lo que DIN, tierra común y resistencia
deben conservarse exactamente como están en este punto.

La segunda cámara fue detectada y usada como evidencia:

```text
Modelo:      Osmo Action 5 Pro
Dispositivo: /dev/video2
Formato:     MJPEG
Resolución:  1920x1080
```

Configuración persistente segura aplicada antes de probar:

```text
matrixBrightness:          20/255 (7.8%)
matrixBrightnessAutomatic: false
bootScreenAktiv:           false
bootBatteryScreen:         false
Fuente declarada:          4.5 V, 3 A máximo
```

La configuración original `matrixType: 1` distribuyó incorrectamente las marcas
entre paneles. Se cambió mediante `POST /api/config` a:

```text
matrixType: 3
Tiled 4x 8x8 CJMCU (Column major)
```

Resultado del patrón de orientación con `matrixType: 3`:

```text
panel 1: L roja, esquina superior izquierda   PASS
panel 2: L verde, esquina superior derecha    PASS
panel 3: L azul, esquina inferior izquierda   PASS
panel 4: L blanca, esquina inferior derecha   PASS
```

Prueba funcional final:

```text
Texto:       HOLA
Resultado:   legible, continuo, horizontal y correctamente orientado
Estado:      PASS
```

Evidencia principal:

```text
evidence/full-matrix-off-retry.jpg
evidence/full-matrix-orientation-retry-type1.jpg
evidence/full-matrix-orientation-type3.jpg
evidence/full-matrix-text-hola.jpg
evidence/osmo-camera-test.jpg
```

La herramienta `tools/panel_test.py` ahora soporta `orientation` y `text`, además
de `off`, colores y píxeles. No elevar el brillo mientras se use la fuente de
4.5 V / 3 A.

---

# 33. WebUI móvil ligera y efecto Fogata — 2026-08-11

La WebUI oficial carga JavaScript externo y una galería de más de 2,000 patrones,
lo que puede agotar memoria en navegadores móviles. Se agregó una interfaz local
independiente, sin frameworks ni recursos externos:

```text
http://192.168.1.58/simple
```

Funciones disponibles:

```text
Mostrar reloj Perú
Mostrar texto
Iniciar Fogata animada
Detener efecto
Apagar matriz
```

La página fija el brillo de cada operación en `20/255` (7.8%). El efecto Fogata
genera frames 32x8 en el navegador y los envía a `/api/screen`; funciona mientras
la pestaña permanezca abierta. No usa Pixel Gallery.

Firmware instalado y verificado:

```text
Versión inicial: 2.5.6-smart-ui
Binario: firmware/pixelit_d1_mini_smart_ui.bin
SHA256:  be956c87ce7af6356a3c3221f48e864b92f3b8f954c19e58d9646df5fccb6757
RAM:     46272/81920 (56.5%)
Flash:   499008/1044464 (47.7%)
```

Validación posterior al flash:

```text
/simple HTTP:              PASS
API:                       PASS
matrixType 3 conservado:   PASS
brillo 20 conservado:      PASS
brillo automático OFF:     PASS
NTP pool.ntp.org:          PASS
zona Perú UTC-5:           PASS
horario de verano OFF:     PASS
Fogata por API:            PASS
Color configurable reloj:    PASS
Fogata vertical ascendente: PASS
```
