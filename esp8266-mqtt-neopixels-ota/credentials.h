// There should really a global settings file in the Arduino
// IDE that would take care of stuff like this by setting some
// "environment variables" that could be accessed by every sketch,
// see https://github.com/esp8266/Arduino/issues/1452

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "..."
#define WLAN_PASS       "..."

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "..."
#define AIO_KEY         "..."
