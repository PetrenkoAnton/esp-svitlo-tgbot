#include <Arduino.h>
#include <FastBot2.h>

#include <GyverNTP.h>
#include <EEPROM.h>
#ifdef ESP8266
#include <ESP8266Ping.h>
#else
#include <ESP32Ping.h>
#endif

#include "handler.h"

enum Status { CONNECTED, DISCONNECTED };
struct StatusData { Status status; DateTime datetime; };
Status currentStatus;

FastBot2 bot;
fb::Message message;

void setup()
{
  unsigned short i,j;
  Serial.begin(BAUD_RATE);

  EEPROM.begin(4);
  currentStatus = (Status)EEPROM.read(0);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi... (" + String(i++) + ")");
  }

  NTP.begin(GMT_OFFSET);   

  bot.attachUpdate(handle);
  bot.setToken(F(BOT_TOKEN));

  bot.setPollMode(fb::Poll::Long, 20000);
  bot.skipUpdates(-10);

  message.chatID = CHANNEL_ID;
  message.text = "ESPxx connected";

  bot.sendMessage(message);
  while (!bot.lastBotMessage())
  {
    delay(500);
    Serial.println("Connecting to Telegram... (" + String(j++) + ")");
  }

  // Check connection status after setup
  String ip = LOCAL_IP;
  ip.trim();
  bool success = Ping.ping(ip.c_str());
  Status newStatus = success ? CONNECTED : DISCONNECTED;
  if (newStatus != currentStatus) {
    currentStatus = newStatus;
    EEPROM.write(0, currentStatus);
    EEPROM.commit();
  }
  String statusMsg = success ? "Connection OK to " + ip : "No connection to " + ip;
  fb::Message statusMessage;
  statusMessage.chatID = CHANNEL_ID;
  statusMessage.text = statusMsg;
  bot.sendMessage(statusMessage);
}

void loop()
{
  bot.tick();
  NTP.tick();
}