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
  Serial.begin(BAUD_RATE);
  #endif

  EEPROM.begin(sizeof(status_data));
  EEPROM.get(0, status_data);

  connect_to_wifi();
  init_bot();
  send_startup_message();
}

void loop()
{
  bot.tick();
  NTP.tick();

  if (timer(timer_expire, INTERVAL)) {
    handle_status_check(status_data);
  }
}