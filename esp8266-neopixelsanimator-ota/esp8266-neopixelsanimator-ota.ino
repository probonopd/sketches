# This is known to work with Arduino 1.6.8 Hourly Build and esp core master
# as of Feb 6, 2016

#include <NeoPixelBus.h>
#include <functional>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "...";
const char* password = "...";

#define pixelCount 30 // make sure to set this to the number of pixels in your strip
#define pixelPin 2  // make sure to set this to the correct pin, ignored for UartDriven branch

NeoPixelBus strip = NeoPixelBus(pixelCount, pixelPin);
NeoPixelAnimator animations(&strip, NEO_CENTISECONDS);

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
    return;

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  strip.Begin();
  strip.Show();

  // just pick some colors
  for (uint16_t pixel = 0; pixel < pixelCount; pixel++)
  {
    RgbColor color = RgbColor(0, 0, 255);
    strip.SetPixelColor(pixel, color);
  }

  Serial.println();
  Serial.println("Running...");
}

void FadeToColor(uint16_t time, String hexstring) // hexstring is a string like #ff0000 for red; since values are centiseconds, 1000 = 10 seconds
{
  long number = (long) strtol( &hexstring[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  RgbColor color = RgbColor(r, g, b);

  for (uint16_t pixel = 0; pixel < pixelCount; pixel++)
  {
    RgbColor originalColor = strip.GetPixelColor(pixel); // each animation starts with the color that was present
    AnimUpdateCallback animUpdate = [ = ](float progress)
    {
      RgbColor updatedColor = RgbColor::LinearBlend(originalColor, color, progress); // progress will start at 0.0 and end at 1.0
      strip.SetPixelColor(pixel, updatedColor);
    };
    // animations.StartAnimation(pixel, time, animUpdate);
    animations.StartAnimation(pixel, time/2 +(pixel*time)/pixelCount/2, animUpdate); // Do not update all pixels at once but the leftmost twice as fast
  }
}

void loop()
{
  ArduinoOTA.handle();
  yield();
  
  if (animations.IsAnimating())
  {
    animations.UpdateAnimations(); // the normal loop just needs these two to run the active animations
    strip.Show();
  }
  else
  {
    Serial.println("FadeToColor...");
    FadeToColor(1000, "#ff0000");
  }
  
}
