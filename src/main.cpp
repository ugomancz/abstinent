#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiClientSecureBearSSL.h>
#include "config.h"

#define DETECTED 0
#define NOT_DETECTED 1

Ticker ticker;
unsigned short newRotations = 0;
unsigned short previousSensorValue = NOT_DETECTED;
bool sendPost = false;

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
  sendPost = true;
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
  if (sendPost /*and newRotations > 0*/)
  {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(SERVER_FOOTPRINT);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://abstinent.fun/api.php")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.POST("{\"apiKey\":\"" + String(API_KEY) + "\",\"newRotations\":\"" + String(newRotations) + "\"}");

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  sendPost = false;
}
