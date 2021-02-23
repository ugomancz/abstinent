#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "config.h"

#define DETECTED 0
#define NOT_DETECTED 1

AsyncWebServer server(80);
int totalRotations = 0;
int previous_sensor_value = NOT_DETECTED;

// returns true on rising edge
bool signal_detected(int sensor_pin)
{
  int current_sensor_value = digitalRead(SENSOR);
  if ((current_sensor_value == DETECTED) & (previous_sensor_value == NOT_DETECTED)) // rising edge
  {
    previous_sensor_value = current_sensor_value;
    return true;
  }
  previous_sensor_value = current_sensor_value;
  return false;
}

// returns total distance ran since start of program
float totalDistanceRan()
{
  return totalRotations * ROTATION_LENGTH;
}

// replaces placeholder with value
String processor(const String &var)
{
  if (var == "TOTAL_DISTANCE")
  {
    return String(totalDistanceRan());
  }
  return String();
}

void setup()
{
  // sensor setup
  pinMode(SENSOR, INPUT);

  // serial setup
  Serial.begin(115200);
  while (!Serial) {}

  // filesystem setup
  if (!LittleFS.begin())
  {
    Serial.printf("Error with filesystem\n");
  }

  // wifi connection setup
  Serial.printf("Connecting to %s\n", SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.printf(".");
  }

  // webserver setup

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/web/index.html", String(), false, processor);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/web/style.css", "text/css");
  });

  server.on("/totalDistance", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", (String(totalDistanceRan())).c_str());
  });
  server.serveStatic("/images/", LittleFS, "/images/");
  server.begin();
}

void loop()
{
  if (signal_detected(SENSOR))
  {
    totalRotations++;
  }
}
