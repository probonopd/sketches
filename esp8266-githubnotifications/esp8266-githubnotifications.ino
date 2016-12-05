/*
   Check GitHub notifications
   https://developer.github.com/changes/2012-10-26-notifications-api/
   Based on the HTTP over TLS (HTTPS) example sketch by Ivan Grokhotkov.
   This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "___";
const char* password = "___";
const char* gh_token = "___"; // Get a GitHub token with notification scope from https://github.com/settings/tokens

int ledState = LOW;

// Interval for checking GitHub
unsigned long previousMillis = 0;
unsigned long gh_poll_interval = 99; // The interval in seconds we are allowed to poll at. Gets updated from the respose header
String gh_last_modified;

const char* host = "api.github.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";

bool unread;

// Check for a certain keyword and blink if found
String keyword1 = "probonopd/";
bool keyword1_found;
unsigned long keyword1_previousMillis = 0;
const long keyword1_interval = 250; // Set blink rate for given keyword

bool is_initial_run = true;
bool http_not_modified = false;

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

  // Periodically poll GitHub according to the time interval defined by GitHub
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= gh_poll_interval * 1000) {
    previousMillis = currentMillis;
    poll();
  }

  if (keyword1_found == true) {

    if (currentMillis - keyword1_previousMillis >= keyword1_interval) {
      keyword1_previousMillis = currentMillis;
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
      digitalWrite(LED_BUILTIN, ledState);
    }
  } else {
    if (unread == true) {
      digitalWrite(LED_BUILTIN, LOW); // LED on
    } else {
      digitalWrite(LED_BUILTIN, HIGH); // LED off
    }
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

    if (line.indexOf("HTTP/") == 0) {
      if (line.indexOf("HTTP/1.1 304 Not Modified") == 0) {
        http_not_modified = true;
      } else {
        http_not_modified = false;
      }
    }

    if (line.indexOf("X-Poll-Interval:") == 0) {
      Serial.print("--> gh_poll_interval --> ");
      gh_poll_interval = line.substring(17).toInt();
      Serial.println(gh_poll_interval);
    }

    if (line.indexOf("Last-Modified:") == 0) {
      Serial.print("--> gh_last_modified --> ");
      gh_last_modified = line.substring(15);
      gh_last_modified.trim();
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

  if (line.indexOf("\"unread\":true") >= 0) {
    unread = true;
  }
  if (line.indexOf("\"unread\":false") >= 0) {
    unread = false;
  }

  /*
    Check for the presence of given keywords.
    But only do this check if the response was not a "304 Not Modified"
    which never contains the keywords we are looking for.
  */
  if (http_not_modified == false) {
    // Check for keyword1_found
    if (line.indexOf(keyword1) >= 0) {
      keyword1_found = true;
      Serial.println("keyword1_found");
    } else {
      keyword1_found = false;
    }
  }

  /*
    CAUTION: If SOMETHING changed, then GitHub will not give us a "304 Not Modified" BUT
    the response will not contain EVERYTHING but only the DELTA from the last full request that
    did not include a value in the If-Modified-Since: header.
    Hence, we need to ensure that whenever we receive a response that is NOT a "304 Not Modified"
    then we do not include a value in the If-Modified-Since: header in the next request.
    This costs us one X-RateLimit-Remaining credit.
    Of course we must not do this upon the initial run.
    Feel free to implement a full state machine , possibly using the filesystem,
    if you would like to change this.
  */
  if (is_initial_run == false) {
    if (http_not_modified == false) {
      gh_last_modified = "";
      Serial.println("Resetting http_not_modified to empty");
      poll();
    }
  }
  is_initial_run = false;

  /*
    Strangely I seem to be getting "HTTP/1.1 304 Not Modified" responses even if 
    I have read notifications since the last update!
    Why is that? What can I do about it? I want the blue light to shut off when I have
    read all my GitHub notifications!
  */

}

