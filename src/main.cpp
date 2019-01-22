#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "mainPage.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NUM_LEDS 32
#define PIN 15
#define BRIGHTNESS 20

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void clearStrip() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));  // change RGB color value
                                                   // here
  }
  strip.show();
}

String checkTeamcityBuildStatus(String teamcityHost, String buildId) {
  HTTPClient https;
  String teamcityUrl = "https://" + teamcityHost +
                       "/app/rest/builds?locator=buildType:(id:" + buildId +
                       "),count:1&fields=count,build(number,status,statusText,"
                       "agent,lastChange,tags,pinned)";
  String fingerprint = "TeamCityHost-SSL-SHA1-Fingerprint";
  https.begin(teamcityUrl, fingerprint);
  https.addHeader("Accept", "application/json");
  https.addHeader("Authorization", "Basic BASE64_YOUR_TEAMCITY_ADMIN_PASSWORD");
  int httpCode = https.GET();
  String payload = https.getString();

  if (httpCode == 200) {
    // Parsing
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) +
                            JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) +
                            JSON_OBJECT_SIZE(6) + 290;
    DynamicJsonBuffer jsonBuffer(capacity);

    https.end();
    JsonObject &teamcityResponse = jsonBuffer.parseObject(payload);
    // parseObject() succeeded
    const char *buildStatus = teamcityResponse["build"][0]["status"];

    return buildStatus;
  } else {
    https.end();
    Serial.println("Failed to GET build status");
    // parseObject() failed
    return "Error occurred while fetching TeamCity build status";
  }
}

void startBlink() {
  String buildStatus = checkTeamcityBuildStatus("TeamCityServerHostname",
                                                "TeamCity_Project_Build_Id");

  if (buildStatus == "SUCCESS") {
    colorWipe(strip.Color(0, 255, 0), 10);
  } else {
    colorWipe(strip.Color(255, 0, 0), 10);
  }

  // Create the response object
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  response["status"] = 200;
  response["message"] = "Successfully started blinking";
  response["buildStatus"] = buildStatus;
  String json;
  response.prettyPrintTo(json);
  server.send(200, "application/json", json);
}

void stopBlink() {
  clearStrip();

  // Create the response object
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  response["status"] = 200;
  response["message"] = "Successfully stopped blinking";
  String json;
  response.prettyPrintTo(json);
  server.send(200, "application/json", json);
}

void handleRoot() { server.send(200, "text/html", String(MAIN_page)); }

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void setup(void) {
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show();  // Initialize all pixels to 'off'

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/startBlink", startBlink);
  server.on("/stopBlink", stopBlink);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}