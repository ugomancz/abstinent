#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "config.h"

#define DETECTED 0
#define NOT_DETECTED 1

Ticker ticker;
unsigned short newRotations = 0;
unsigned short previousSensorValue = NOT_DETECTED;

// returns true on rising edge
bool signal_detected(int sensor_pin)
{
  int currentSensorValue = digitalRead(SENSOR);
  bool result = ((currentSensorValue == DETECTED) and (previousSensorValue == NOT_DETECTED));
  previousSensorValue = currentSensorValue;
  return result;
}

void performUpdate()
{
  if (newRotations > 0 and WiFi.isConnected())
  {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, SERVER_NAME);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"api_key\":\"aac69111be3000c6f7608a1924b6f460\",\
    \"newRotations\":\"" + String(newRotations) +
                                     "\"}");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
    newRotations = 0;
  }
}

void setup()
{
  pinMode(SENSOR, INPUT);
  Serial.begin(115200);

  // wifi connection setup
  WiFi.begin(SSID, PASSWORD);
  while (!WiFi.isConnected())
  {
    delay(500);
  }
  Serial.println("Connected");

  // ticker timer setup
  ticker.attach(SPEED_MEASUREMENT_PERIOD, performUpdate);
}

void loop()
{
  if (signal_detected(SENSOR))
  {
    newRotations++;
  }
}
