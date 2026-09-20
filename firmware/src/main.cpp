#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Kelvinator.h>

// =========================
// IR
// =========================

const uint16_t IR_SEND_PIN = 15;

IRKelvinatorAC ac(IR_SEND_PIN);

// =========================
// State
// =========================

int currentState = -1;

// -1 = belum tahu
//  0 = OFF
//  1 = ON

// =========================
// WiFi
// =========================

void connectWiFi()
{
  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// =========================
// Get API state
// =========================

int getACState()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected");

    return -1;
  }

  WiFiClientSecure client;

  // Testing only.
  // Later bisa diganti dengan certificate validation.
  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, API_URL))
  {
    Serial.println("HTTP begin failed");

    return -1;
  }

  int httpCode = http.GET();

  if (httpCode != 200)
  {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);

    http.end();

    return -1;
  }

  String response = http.getString();

  http.end();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error)
  {
    Serial.println("JSON parse failed");
    return -1;
  }

  Serial.print("API response: ");
  Serial.println(response);

  if (doc["settings"]["on"])
  {
    return 1;
  }

  if (!doc["settings"]["on"])
  {
    return 0;
  }

  Serial.println("Invalid API response");

  return -1;
}

// =========================
// Send AC ON
// =========================

void turnACOn()
{
  Serial.println("Sending AC ON...");

  ac.on();
  ac.setMode(kKelvinatorCool);
  ac.setTemp(22);
  ac.setFan(kKelvinatorFanMax);
  ac.setLight(true);

  ac.send();

  Serial.println("AC ON command sent");
}

// =========================
// Send AC OFF
// =========================

void turnACOff()
{
  Serial.println("Sending AC OFF...");

  ac.off();
  ac.send();

  Serial.println("AC OFF command sent");
}

// =========================
// Setup
// =========================

void setup()
{
  Serial.begin(115200);

  ac.begin();

  connectWiFi();

  Serial.println("Getting initial AC state...");

  int state = getACState();

  if (state != -1)
  {
    currentState = state;

    Serial.print("Initial state: ");
    Serial.println(currentState == 1 ? "ON" : "OFF");
  }
  else
  {
    Serial.println("Failed to get initial state");
  }
}

// =========================
// Loop
// =========================

void loop()
{

  int newState = getACState();

  // API request gagal
  if (newState == -1)
  {
    delay(1000);
    return;
  }

  // State berubah
  if (newState != currentState)
  {

    Serial.print("State changed: ");

    Serial.print(currentState);
    Serial.print(" -> ");
    Serial.println(newState);

    if (newState == 1)
    {
      turnACOn();
    }
    else
    {
      turnACOff();
    }

    currentState = newState;
  }

  // State tidak berubah
  else
  {
    Serial.println("No change");
  }

  delay(1000);
}