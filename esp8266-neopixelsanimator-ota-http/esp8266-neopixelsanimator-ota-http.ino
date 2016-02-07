// Tested with /Arduino-1.6.8.hourly201602020926.esp04c6609-x86_64.AppImage
// Set color like this: http://esp8266-xxxxxx.local./?color=xffee55 (replace with your ChipID)

#include <NeoPixelBus.h> // UartDriven branch; GPIO2
#include <functional>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

const char* ssid = "xxx";
const char* password = "xxx";

ESP8266WebServer server(80);

#define pixelCount 30 // make sure to set this to the number of pixels in your strip
#define pixelPin 2  // make sure to set this to the correct pin, ignored for UartDriven branch
#define bonjourName = "colorfader"; // the name this can be accessed at in the network

NeoPixelBus strip = NeoPixelBus(pixelCount, pixelPin);
NeoPixelAnimator animations(&strip, NEO_CENTISECONDS);

void setup()
{

  // Black
  strip.Begin();
  strip.Show();

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

  Serial.println();
  Serial.println("Running...");

  server.onNotFound(handleRequest);

  server.begin();
  Serial.println("HTTP server started");

  // Advertise http service
  MDNS.addService("http", "tcp", 80);

  FadeToColor(100, "#000100"); // Darkest green
}

void handleRequest() {
  String message = "<html><body>\n";
  message += "<a href='?color=xffdd55'>Neutral White</a><br>\n";
  message += "<a href='?color=xffee33'>Warm White</a><br>\n";
  message += "<a href='?color=xddeeff'>Cool White</a><br>\n";
  message += "<a href='?color=x0066ff'>Blue</a><br>\n";
  message += "<a href='?color=x000000'>Black</a><br>\n";
  message += "<pre>\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "color") {
      Serial.println("FadeToColor...");
      FadeToColor(1000, server.arg(i));
    }
    message += "</pre></body></html>\n";
  }
  server.send(200, "text/html", message);

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
    animations.StartAnimation(pixel, time / 2 + (pixel * time) / pixelCount / 2, animUpdate); // Do not update all pixels at once but the leftmost twice as fast
  }
}

void loop()
{
  ArduinoOTA.handle();
  yield();

  server.handleClient();

  if (animations.IsAnimating())
  {
    animations.UpdateAnimations(); // the normal loop just needs these two to run the active animations
    strip.Show();
  }

}
