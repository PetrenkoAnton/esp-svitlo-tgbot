#include <Arduino.h>
#include <GyverNTP.h>
#include <EEPROM.h>
#ifdef ESP8266
#include <ESP8266Ping.h>
#else
#include <ESP32Ping.h>
#endif
#include <FastBot2.h>
#include "utils.h"

String formatStatusMessage(bool success)
{
  return success ? "Світло є" : "Світла немає";
}

String formatDuration(time_t seconds)
{
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  return String(hours) + " год. " + String(minutes) + " хв.";
}

String formatCurrentMessage(bool success, time_t duration)
{
  String duration_str = formatDuration(duration);
  if (success) return "Світло є вже " + duration_str;
  else return "Світла немає вже " + duration_str;
}

String formatChangeMessage(bool success, time_t duration)
{
  String duration_str = formatDuration(duration);
  if (success) return "Світло з'явилось.\nСвітла не було " + duration_str;
  else return "Світло зникло.\nСвітло було " + duration_str;
}

CheckResult checkConnectionStatus(StatusData& data)
{
  time_t now = NTP.getUnix();
  time_t duration = now - data.timestamp;
  String ip = CHECK_IP;
  ip.trim();
  bool success = Ping.ping(ip.c_str());
  Status new_status = success ? CONNECTED : DISCONNECTED;
  bool changed = (new_status != data.status);
  String message;
  if (changed) {
    message = formatChangeMessage(success, duration);
  } else {
    message = formatCurrentMessage(success, duration);
  }
  if (changed) {
    data.status = new_status;
    data.timestamp = now;
    data.counter++;
    EEPROM.put(0, data);
    EEPROM.commit();
  }
  return CheckResult{changed, message};
}

void postStatusToChannel(String status) {
  fb::Message message;
  message.chatID = CHANNEL_ID;
  message.text = status;
  #ifdef DEBUG
  Serial.println(message.text);
  #endif
  bot.sendMessage(message);
}

void clearEEPROMData()
{
  for (size_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0xFF); 
  }
}