# esp8266-mqtt-neopixels-ota

Receive a color set in adafruit.io via MQTT and frive Neopixels using a esp8266 with ArduinoOTA

## Libraries needed

 * [__Adafruit_MQTT_Library__](https://github.com/adafruit/Adafruit_MQTT_Library) from Libraries Manager
 * __ESP8266mDNS__ and __ArduinoOTA__ coming with esp8266/Arduino installed with Boards Manager and `http://arduino.esp8266.com/staging/package_esp8266com_index.json`
 * [__NeoPixelBus__](https://github.com/Makuna/NeoPixelBus/tree/DmaDriven) DmaDriven branch

## TODO

 * Implement animations (in a way that they don't block MQTT and/or OTA) - help is welcome. me-no-dev notes that "OTA and MQTT should run async and your pixels in the loop, then you would not have that problem. OTA needs attention after successful handshake with the computer and that has tomeout of around 10-20 seconds. Do not worry much about OTA - maybe if Espressif gets RTOS for ESP at the same level as NON_OS (this) we will move over and forget about issues like this one"
 * Get initial values, https://forums.adafruit.com/viewtopic.php?f=56&t=88407
