#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <Ticker.h>
#include "config.h"

#define DETECTED 0
#define NOT_DETECTED 1

AsyncWebServer server(80);
Ticker ticker;
int totalRotations = 0;
int previousTotalRotations = 0;
float topSpeed = 0;
float currentSpeed = 0;
int previousSensorValue = NOT_DETECTED;

// returns true on rising edge
bool signal_detected(int sensor_pin)
{
  int currentSensorValue = digitalRead(SENSOR);
  if ((currentSensorValue == DETECTED) & (previousSensorValue == NOT_DETECTED)) // rising edge
  {
    previousSensorValue = currentSensorValue;
    return true;
  }
  previousSensorValue = currentSensorValue;
  return false;
}

// returns total distance ran since start of program
float totalDistanceRan()
{
  return totalRotations * ROTATION_LENGTH;
}

float toKmPerHour(float metersPerSecond)
{
  return metersPerSecond * 3.6F;
}

// calculates speed in last SPEED_MEASUREMENT_PERIOD seconds
void calculateSpeed()
{
  float currentRotationsPerSecond = (totalRotations - previousTotalRotations) / SPEED_MEASUREMENT_PERIOD;
  currentSpeed = currentRotationsPerSecond * ROTATION_LENGTH;
  if (currentSpeed > topSpeed)
  {
    topSpeed = currentSpeed;
  }
  previousTotalRotations = totalRotations;
}

// replaces placeholder with value
String processor(const String &var)
{
  if (var == "TOTAL_DISTANCE")
  {
    return String(totalDistanceRan());
  }
  else if (var == "TOP_SPEED")
  {
    return String(topSpeed);
  }
  else if (var == "CURRENT_SPEED")
  {
    return String(currentSpeed);
  }
  else if (var == "TOP_SPEED_KM")
  {
    return String(toKmPerHour(topSpeed));
  }
  else if (var == "CURRENT_SPEED_KM")
  {
    return String(toKmPerHour(currentSpeed));
  }
  return String();
}

void setup()
{
  // sensor setup
  pinMode(SENSOR, INPUT);

  // serial setup
  Serial.begin(115200);
  while (!Serial)
  {
  }

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

  // ticker timer setup
  ticker.attach(SPEED_MEASUREMENT_PERIOD, calculateSpeed);

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

  server.on("/topSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", (String(topSpeed)).c_str());
  });

  server.on("/currentSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", (String(currentSpeed)).c_str());
  });

  server.on("/topSpeedKm", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", (String(toKmPerHour(topSpeed))).c_str());
  });

  server.on("/currentSpeedKm", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", (String(toKmPerHour(currentSpeed))).c_str());
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
