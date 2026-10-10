# Protected PixelIt configuration access point

This local modification adds WPA2 authentication to the temporary `PIXELIT`
configuration access point. It does not add authentication to the WebUI or REST
API on the local network.

## Configure the secret

Copy `include/LocalSecrets.h.example` to `include/LocalSecrets.h`, then replace
the access-point password, preferred 2.4 GHz SSID, and Wi-Fi password. Passwords
must contain 8 to 63 printable ASCII characters. `LocalSecrets.h` is ignored by
Git. ESP8266 does not support 5 GHz or WPA3-only networks.

Edit locally:

```cpp
#define PIXELIT_AP_SSID "PIXELIT"
#define PIXELIT_AP_PASSWORD "your-private-ap-password"
#define PIXELIT_WIFI_SSID "your-2.4ghz-network"
#define PIXELIT_WIFI_PASSWORD "your-wifi-password"
```

## Build

```bash
pio run -e ESP8266_d1_mini
```

The compiler rejects passwords outside the WPA2 length limits.
