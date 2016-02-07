# esp8266-neopixelsanimator-ota-http

Simple sketch that connects to WLAN and opens a web server with a tiny UI that allows to set colors on an attached strip of NeoPixels with animations. Also supports ArduinoOTA.

Since it uses [WiFiManager](https://github.com/tzapu/WiFiManager), the SSID and password do not have to be hardcoded inside the source code, but users need to connect to the __AutoConnectAP__, select an SSID and enter the password once. (Note that these are stored in a persistent way that makes it possible to OTA update the sketch itself without re-entering these.)
