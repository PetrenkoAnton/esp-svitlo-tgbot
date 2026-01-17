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
#include "utils.h"

enum Status { CONNECTED, DISCONNECTED };
struct StatusData { Status status; time_t timestamp; };
Status currentStatus;
time_t lastTimestamp;

FastBot2 bot;
fb::Message message;

void setup()
{
  #ifdef DEBUG
  unsigned short i = 1;
  #endif
  Serial.begin(BAUD_RATE);

  EEPROM.begin(8);

  #ifdef CLEAR_EEPROM
  clearEEPROMData();
  #endif

  StatusData savedData;
  EEPROM.get(0, savedData);
  currentStatus = savedData.status;
  lastTimestamp = savedData.timestamp;

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    #ifdef DEBUG
    Serial.println("Connecting to WiFi... (" + String(i++) + ")");
    #endif
  }

  NTP.begin(GMT_OFFSET); 
  
  #ifdef DEBUG
  i = 1;
  #endif

  while(!NTP.updateNow())
  {
    delay(1000);
    #ifdef DEBUG
    Serial.println("Connecting to NTP... [" + String(i++) + "]");
    #endif
  }

  bot.attachUpdate(handle);
  bot.setToken(F(BOT_TOKEN));

  bot.setPollMode(fb::Poll::Long, 20000);
  bot.skipUpdates(-10);

  message.chatID = CHANNEL_ID;
  message.text = "Мікроконтролер підключено";

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
  String statusMsg = checkConnectionStatus();
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