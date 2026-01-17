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

String format_status_message(bool success)
{
  return success ? "Світло є" : "Світла немає";
}

String format_duration(time_t seconds)
{
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  return String(hours) + " год. " + String(minutes) + " хв.";
}

String format_current_message(bool success, time_t duration)
{
  String duration_str = format_duration(duration);
  if (success) return "Світло є вже " + duration_str;
  else return "Світла немає вже " + duration_str;
}

String format_change_message(bool success, time_t duration)
{
  String duration_str = format_duration(duration);
  if (success) return "Світло з'явилось.\nСвітла не було " + duration_str;
  else return "Світло зникло.\nСвітло було " + duration_str;
}

CheckResult check_connection_status(StatusData& data)
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
    message = format_change_message(success, duration);
  } else {
    message = format_current_message(success, duration);
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

void post_status_to_channel(String status) {
  fb::Message message;
  message.chatID = CHANNEL_ID;
  message.text = status;
  #ifdef DEBUG
  Serial.println(message.text);
  #endif
  bot.sendMessage(message);
}

void clear_eeprom_data()
{
  for (size_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0xFF); 
  }
}

String format_ping_message()
{
  bool ping_ok = Ping.ping(CHECK_IP);
  return String(CHECK_IP) + " is " + (ping_ok ? "connected." : "not connected.");
}