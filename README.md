# SmartDesktopClock

Reloj LED IoT para Wemos D1 Mini (ESP8266), con una matriz WS2812B de 32×8 y firmware basado en [PixelIt](https://github.com/pixelit-project/PixelIt).

Incluye interfaz web, escenas animadas autónomas, control de velocidad, código fuente y documentación de validación del hardware.

## Contenido

- `pixelit-firmware/`: firmware y su licencia original.
- `FIRST_TEST.md`: pruebas iniciales y evolución del proyecto.
- `docs/`: informes de validación.
- `tools/`: herramientas de prueba de la matriz.
- `evidence/`: fotografías de las pruebas.
- `logs/`: registros de validación.
- `firmware/`: metadatos y checksums de las compilaciones locales.

## Compilar

Instalar PlatformIO, copiar `pixelit-firmware/include/LocalSecrets.h.example` a `pixelit-firmware/include/LocalSecrets.h` y configurar las credenciales localmente según `pixelit-firmware/SECURE_AP.md`.

```bash
cd pixelit-firmware
pio run -e ESP8266_d1_mini
```

La publicación contiene una copia del código desarrollado. Se omiten el respaldo de la memoria del dispositivo, los binarios compilados y las credenciales. Los checksums y las referencias a esos archivos documentan pruebas locales; los archivos correspondientes no se distribuyen aquí.

## Licencia

La licencia del firmware PixelIt se conserva en `pixelit-firmware/LICENSE`.
