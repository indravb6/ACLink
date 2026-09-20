#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Kelvinator.h>

const uint16_t IR_SEND_PIN = 15;

IRKelvinatorAC ac(IR_SEND_PIN);

String eventId = "";
JsonDocument settings;

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

void updateACState()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected");
    return;
  }

  WiFiClientSecure client;

  // Testing only.
  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, API_URL))
  {
    Serial.println("HTTP begin failed");
    return;
  }

  int httpCode = http.GET();

  if (httpCode != 200)
  {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
    http.end();
    return;
  }

  String response = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error)
  {
    Serial.println("JSON parse failed");
    return;
  }

  Serial.print("API response: ");
  Serial.println(response);

  if (doc.containsKey("settings"))
  {
    settings = doc["settings"];
  }
  else
  {
    Serial.println("Invalid API response");
  }
}

void sendCommand()
{
  Serial.println("Sending AC ON...");

  if (settings["on"])
  {
    ac.on();
  }
  else
  {
    ac.off();
  }
  switch (settings["mode"].as<String>().c_str()[0])
  {
  case 'c':
    ac.setMode(kKelvinatorCool);
    break;
  case 'f':
    ac.setMode(kKelvinatorFan);
    break;
  case 'h':
    ac.setMode(kKelvinatorHeat);
    break;
  case 'd':
    ac.setMode(kKelvinatorDry);
    break;
  default:
    break;
  }
  ac.setTemp(settings["targetTemperature"].as<int>());
  ac.setTurbo(false);
  ac.setLight(true);
  switch (settings["fanSpeed"].as<String>().c_str()[0])
  {
  case 'a':
    ac.setFan(kKelvinatorFanAuto);
    break;
  case 'm':
    ac.setFan(3);
    break;
  case 'l':
    ac.setFan(1);
    break;
  case 'h':
    ac.setFan(5);
    break;
  case 't':
    ac.setFan(5);
    ac.setTurbo(true);
    break;
  default:
    break;
  }

  String swing = settings["swing"].as<String>();
  if (swing.compareTo("1") == 0)
    ac.setSwingVertical(false, kKelvinatorSwingVHighest);
  else if (swing.compareTo("2") == 0)
    ac.setSwingVertical(false, kKelvinatorSwingVUpperMiddle);
  else if (swing.compareTo("3") == 0)
    ac.setSwingVertical(false, kKelvinatorSwingVMiddle);
  else if (swing.compareTo("4") == 0)
    ac.setSwingVertical(false, kKelvinatorSwingVLowerMiddle);
  else if (swing.compareTo("5") == 0)
    ac.setSwingVertical(false, kKelvinatorSwingVLowest);
  else if (swing.compareTo("1-5") == 0)
    ac.setSwingVertical(true, kKelvinatorSwingVAuto);
  else if (swing.compareTo("1-3") == 0)
    ac.setSwingVertical(true, kKelvinatorSwingVHighAuto);
  else if (swing.compareTo("2-4") == 0)
    ac.setSwingVertical(true, kKelvinatorSwingVMiddleAuto);
  else if (swing.compareTo("3-5") == 0)
    ac.setSwingVertical(true, kKelvinatorSwingVLowAuto);

  ac.send();

  Serial.println("AC command sent");
}

void setup()
{
  Serial.begin(115200);

  ac.begin();

  connectWiFi();
}

void loop()
{
  updateACState();

  if (settings["id"] == eventId)
  {
    Serial.println("No new event");
    delay(1000);
    return;
  }

  eventId = settings["id"].as<String>();
  sendCommand();

  delay(1000);
}