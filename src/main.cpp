#include <Arduino.h>
#include <FastBot2.h>
#include <GyverNTP.h>

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
#include "timer.h"

StatusData current_data;
unsigned long timer_expire;

FastBot2 bot;
fb::Message message;

void setup()
{
  #ifdef DEBUG
  unsigned short i = 1;
  Serial.begin(BAUD_RATE);
  #endif

  EEPROM.begin(12);

  EEPROM.get(0, current_data);

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
}

void loop()
{
  bot.tick();
  NTP.tick();

  if (timer(timer_expire, INTERVAL)) {
    CheckResult result = checkConnectionStatus(current_data);
    if (result.changed) {
      postStatusToChannel(result.message);
    }
  }
}