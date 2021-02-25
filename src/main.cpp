#include <asyncHTTPrequest.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "config.h"

#define DETECTED 0
#define NOT_DETECTED 1

Ticker ticker;
asyncHTTPrequest request;
unsigned short newRotations = 0;
unsigned short previousSensorValue = NOT_DETECTED;
bool firstZero = false;

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
  if ((firstZero or newRotations > 0) and (request.readyState() == 0 || request.readyState() == 4)) {
#ifdef DEBUG
Serial.println(("request ready - new rotations: " + String(newRotations)).c_str());
#endif //DEBUG
    if (request.open("GET", ("http://abstinent.fun/api.php?apiKey=" + String(API_KEY) + "&newRotations=" + String(newRotations)).c_str())) {
      request.send();
      firstZero = (newRotations == 0) ? false : true;
      newRotations = 0;
#ifdef DEBUG
      Serial.println("request sent");
#endif //DEBUG
    }
#ifdef DEBUG
    else
    {
      Serial.println("request failed");
    }
#endif //DEBUG
  }
}

void setup()
{
  pinMode(SENSOR, INPUT);
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
  {
  }
#endif //DEBUG
  WiFi.begin(SSID, PASSWORD);
  while (!WiFi.isConnected())
  {
    delay(500);
  }
#ifdef DEBUG
  Serial.println("wifi connected");
  request.setDebug(true);
#endif //DEBUG
  ticker.attach(SPEED_MEASUREMENT_PERIOD, performUpdate);
}

void loop()
{
  if (signal_detected(SENSOR))
  {
    newRotations++;
  }
}
