#include <Arduino.h>
#include <FastBot2.h>

#include <GyverNTP.h>

#include "handler.h"

FastBot2 bot;
fb::Message message;

void setup()
{
  unsigned short i,j;
  Serial.begin(BAUD_RATE);

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

  message.chatID = LOGGER_GROUP_ID;
  message.text = "ESPxx connected";

  bot.sendMessage(message);
  while (!bot.lastBotMessage())
  {
    delay(500);
    Serial.println("Connecting to Telegram... (" + String(j++) + ")");
  }
}

void loop()
{
  bot.tick();
  NTP.tick();
}