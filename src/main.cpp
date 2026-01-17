#include <Arduino.h>
#include <FastBot2.h>

#include <EEPROM.h>
#ifdef ESP8266
#include <ESP8266Ping.h>
#include <ESP8266WiFi.h>
#else
#include <ESP32Ping.h>
#include <WiFi.h>
#endif

#include "handler.h"
#include "utils.h"

StatusData currentData;
unsigned long lastCheck;

FastBot2 bot;
fb::Message message;

void setup()
{
  #ifdef DEBUG
  unsigned short i = 1;
  #endif
  Serial.begin(BAUD_RATE);

  EEPROM.begin(12);

  EEPROM.get(0, currentData);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    #ifdef DEBUG
    Serial.println("Connecting to WiFi... (" + String(i++) + ")");
    #endif
  }

  bot.attachUpdate(handle);
  bot.setToken(F(BOT_TOKEN));

  bot.setPollMode(fb::Poll::Long, 20000);
  bot.skipUpdates(-10);

  message.chatID = CHANNEL_ID;
  message.text = "Мікроконтролер підключено";

  #ifdef DEBUG
  Serial.println(message.text);
  #endif
  bot.sendMessage(message);

  #ifdef DEBUG
  i = 1;
  #endif

  while (!bot.lastBotMessage())
  {
    delay(500);
    #ifdef DEBUG
    Serial.println("Connecting to Telegram... (" + String(i++) + ")");
    #endif
  }

  // Check connection status after setup
  CheckResult res = checkConnectionStatus(currentData);
  String statusMsg = res.message;
  fb::Message statusMessage;
  statusMessage.chatID = CHANNEL_ID;
  statusMessage.text = statusMsg;
  #ifdef DEBUG
  Serial.println(statusMessage.text);
  #endif
  bot.sendMessage(statusMessage);

  lastCheck = millis();
}

void loop()
{
  bot.tick();

  if (millis() - lastCheck > INTERVAL * 1000UL) {
    CheckResult res = checkConnectionStatus(currentData);
    if (res.changed) {
      postStatusToChannel(res.message);
    }
    lastCheck = millis();
  }
}