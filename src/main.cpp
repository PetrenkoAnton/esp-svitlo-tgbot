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

StatusData status_data;
unsigned long timer_expire;

FastBot2 bot;

void setup()
{
  #ifdef DEBUG
  unsigned short i = 1;
  Serial.begin(BAUD_RATE);
  #endif

  EEPROM.begin(sizeof(status_data));
  EEPROM.get(0, status_data);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    #ifdef DEBUG
    Serial.println("Connecting to WiFi... (" + String(i++) + ")");
    #endif
  }

  NTP.begin();

  bot.attachUpdate(handle);
  bot.setToken(F(BOT_TOKEN));
  bot.setPollMode(fb::Poll::Long, 20000);
  bot.skipUpdates(-10);

  send_message("ESPxx connected", ADMIN_ID);

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

  handle_status_check(status_data);
}

void loop()
{
  bot.tick();
  NTP.tick();

  if (timer(timer_expire, INTERVAL)) {
    handle_status_check(status_data);
  }
}