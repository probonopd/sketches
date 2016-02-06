/***************************************************
  Set Neopixels and RGB LED on Yellow Board using MQTT w/ OTA
  Adafruit MQTT Library ESP8266 Example combined with ArduinoOTA,
  works on IDE 1.6.7 with ESP8266/Arduino staging as of Feb 6, 2016
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NeoPixelBus.h> // Use the UartDriven branch, as this works well with OTA ##############################
#include "credentials.h"

#define pixelCount 60 // this example assumes 4 pixels, making it smaller will cause a failure
#define pixelPin 2  // ignored for UartDriven branch (it is always GPIO2)
#define colorSaturation 128
NeoPixelBus strip = NeoPixelBus(pixelCount, pixelPin);

RgbColor red = RgbColor(colorSaturation, 0, 0);
RgbColor green = RgbColor(0, colorSaturation, 0);
RgbColor blue = RgbColor(0, 0, colorSaturation);
RgbColor white = RgbColor(colorSaturation);
RgbColor black = RgbColor(0);

HslColor hslRed( red );
HslColor hslGreen( green );
HslColor hslBlue( blue );
HslColor hslWhite( white );
HslColor hslBlack( black );

// Yellow board
// http://blog.squix.ch/2015/01/esp8266-test-board-review.html
#define B_LED 13
#define G_LED 12
#define R_LED 15

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
// WiFiClient client;
// or... use WiFiFlientSecure for SSL
WiFiClientSecure client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed called 'color' for subscribing to changes.
const char COLOR_FEED[] PROGMEM = AIO_USERNAME "/feeds/color";
Adafruit_MQTT_Subscribe color = Adafruit_MQTT_Subscribe(&mqtt, COLOR_FEED);

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();


void setColor(String hexstring)
{
  long number = (long) strtol( &hexstring[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;

  analogWrite(R_LED, r);
  analogWrite(G_LED, g);
  analogWrite(B_LED, b);

  RgbColor color = RgbColor(r, g, b);
  for (int i = 0; i < pixelCount; i++) {
    strip.SetPixelColor(i, color);
  }
  strip.Show();

}


void setup() {
  Serial.begin(115200);
  delay(10);

  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();
  // set the colors,
  // if they don't match in order, you may need to use NEO_GRB flag
  strip.SetPixelColor(0, red);
  strip.SetPixelColor(1, green);
  strip.SetPixelColor(2, blue);
  strip.SetPixelColor(3, white);
  strip.Show();


  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    strip.SetPixelColor(0, blue); // This seems to get ignored; why?
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    strip.SetPixelColor(0, green); // This seems to get ignored; why?
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&color);

  // RGB LED
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  digitalWrite(R_LED, LOW);
  digitalWrite(G_LED, LOW);
  digitalWrite(B_LED, LOW);

  strip.SetPixelColor(0, green);
  strip.SetPixelColor(1, black);
  strip.SetPixelColor(2, black);
  strip.SetPixelColor(3, black);
  strip.Show();

}

uint32_t x = 0;

void loop() {

  ArduinoOTA.handle();
  yield();

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &color) {
      Serial.println(F("Got color"));
      setColor((char *)color.lastread);
    }
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  if (! mqtt.ping()) {
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
