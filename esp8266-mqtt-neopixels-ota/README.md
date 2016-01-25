# esp8266-mqtt-neopixels-ota

Receive a color set in adafruit.io via MQTT and frive Neopixels using a esp8266 with ArduinoOTA

## Libraries needed

 * [__Adafruit_MQTT_Library__](https://github.com/adafruit/Adafruit_MQTT_Library) from Libraries Manager
 * __ESP8266mDNS__ and __ArduinoOTA__ coming with esp8266/Arduino installed with Boards Manager and `http://arduino.esp8266.com/staging/package_esp8266com_index.json`
 * [__NeoPixelBus__](https://github.com/Makuna/NeoPixelBus/tree/DmaDriven) DmaDriven branch

## Hardware needed
 * A strip of Neopixels attached to the TXD pin.
 * I am using a [yellow esp8266 board](http://blog.squix.ch/2015/01/esp8266-test-board-review.html) for this. This board has a built-in RGB LED connected to GPIO13, GPIO12 and GPIO15. I am setting this RGB LED to the same color as the Neopixels, but that is of course not required.

## TODO
 * Get rid of the `.h` file by replacing it with WiFiManager (to handle both WLAN and MQTT credentials storage); blocked by https://github.com/tzapu/WiFiManager/issues/81
 * Use the neopixel animator class? See the examples. sticilface: "I think the example actually are blocking and call delay. You can easily change it to use a time diff function instead. I regularly run all this together with no issues.
If ur struggling I can give you an example."
 * Implement animations (in a way that they don't block MQTT and/or OTA) - help is welcome. me-no-dev notes that "OTA and MQTT should run async and your pixels in the loop, then you would not have that problem. OTA needs attention after successful handshake with the computer and that has tomeout of around 10-20 seconds. Do not worry much about OTA - maybe if Espressif gets RTOS for ESP at the same level as NON_OS (this) we will move over and forget about issues like this one"
 * Get initial values, https://forums.adafruit.com/viewtopic.php?f=56&t=88407 and https://github.com/adafruit/Adafruit_MQTT_Library/issues/20 ("retain" is the keyword)
 * Fire effect http://www.walltech.cc/neopixel-fire-effect/
 * See https://github.com/sticilface/ESP8266-wifi/tree/ESP8266_wifi
 * See https://github.com/AdySan/ESP8266_WebSockets_NeoPixels
 * See https://github.com/jeffeb3/esp8266_lights
