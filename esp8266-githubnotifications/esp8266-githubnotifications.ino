/*
 * Check GitHub notifications
 * https://developer.github.com/changes/2012-10-26-notifications-api/
 * Based on the HTTP over TLS (HTTPS) example sketch by Ivan Grokhotkov.
 * This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "___";
const char* password = "___";
const char* gh_token = "___"; // Get a GitHub token with notification scope from https://github.com/settings/tokens

unsigned long previousMillis = 0;
unsigned long gh_poll_interval = 99; // The interval in seconds we are allowed to poll at. Gets updated from the respose header
String gh_last_modified;

const char* host = "api.github.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";

bool unread;
  
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED off
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  poll();

}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= gh_poll_interval * 1000) {
    previousMillis = currentMillis;   
    poll();
  }
}

void poll() {
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String url = "/notifications";
  Serial.print("requesting URL: ");
  Serial.println(url);

  String request = "GET " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266_GitHubNotifications\r\n" +
               "If-Modified-Since: " + gh_last_modified + "\r\n" +
               "Authorization: token " + gh_token + "\r\n" +
               "Connection: close\r\n\r\n";

  Serial.println(request);
  client.print(request);

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    
    if(line.indexOf("X-Poll-Interval:") == 0) {
      Serial.print("--> gh_poll_interval --> ");
      gh_poll_interval = line.substring(17).toInt();
      Serial.println(gh_poll_interval);
    }    

    if(line.indexOf("Last-Modified:") == 0) {
      Serial.print("--> gh_last_modified --> ");
      gh_last_modified = line.substring(15);
      Serial.println(gh_last_modified);
    }    
      
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');

  /*
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  */

  Serial.print("line.indexOf true: ");
  Serial.println(line.indexOf("\"unread\":true"));
  Serial.print("line.indexOf false: ");
  Serial.println(line.indexOf("\"unread\":false"));
  
  if(line.indexOf("\"unread\":true") >= 0) {
    unread = true;
  }
  if(line.indexOf("\"unread\":false") >= 0) {
    unread = false;
  }
  
  if(unread == true){
    digitalWrite(LED_BUILTIN, LOW); // LED on
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // LED off
  }
}

