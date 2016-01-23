# esp8266-mqtt-neopixels-ota

Receive a color set in adafruit.io via MQTT and frive Neopixels using a esp8266 with ArduinoOTA

## Libraries needed

 * __Adafruit MQTT Client.h__ from Libraries Manager
 * __ESP8266mDNS__ and __ArduinoOTA__ coming with esp8266/Arduino installed with Boards Manager http://arduino.esp8266.com/staging/package_esp8266com_index.json
 * __NeoPixelBus__ DmaDriven branch

## TODO

 * Implement animations (in a way that they don't block MQTT and/or OTA) - help is welcome
