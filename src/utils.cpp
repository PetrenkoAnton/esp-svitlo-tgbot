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
#include "handler.h"

String format_duration(time_t seconds)
{
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  return String(hours) + " год. " + String(minutes) + " хв.";
}

String format_change_message(bool success, time_t duration)
{
  String duration_str = format_duration(duration);
  if (success) return "+ Увімкнення світла.\nСвітла не було " + duration_str;
  else return "- Відключення світла.\nСвітло було " + duration_str;
}

CheckResult check_connection_status(StatusData& data)
{
  bool success = is_connected_to_check_ip();
  Status new_status = success ? CONNECTED : DISCONNECTED;
  bool changed = (new_status != data.status);
  String message;
  if (changed) {
    time_t now = NTP.getUnix();
    time_t duration = now - data.timestamp;
    message = format_change_message(success, duration);

    data.status = new_status;
    data.timestamp = now;
    data.counter++;
    save_status_data(data);
  }
  return CheckResult{changed, message};
}

void post_status_to_channel(String status) {
  send_message(status, CHANNEL_ID);
}

String format_ping_message()
{
  bool ping_ok = is_connected_to_check_ip();
  return String(CHECK_IP) + " is " + (ping_ok ? "connected." : "not connected.");
}

bool is_connected_to_check_ip()
{
  return Ping.ping(CHECK_IP);
}

void save_status_data(const StatusData& data)
{
  Serial.println("Saving to EEPROM: " + String(data.status) + " | " + String(data.timestamp) + " | " + String(data.counter));

  EEPROM.put(0, data);
  EEPROM.commit();
}

void send_message(const String& text, const String& chatID)
{
  fb::Message message;
  message.text = text;
  message.chatID = chatID;

  bot.sendMessage(message);
}