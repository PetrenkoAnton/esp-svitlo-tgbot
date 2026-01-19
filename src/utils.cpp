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
  String result = String(hours) + " год. " + String(minutes) + " хв.";
  #ifdef DEBUG
  int secs = seconds % 60;
  result += " " + String(secs) + " сек.";
  #endif
  return result;
}

String format_current_message(Status status, time_t duration)
{
  String duration_str = format_duration(duration);
  if (status == CONNECTED) return "Світло є вже " + duration_str;
  else return "Світла немає вже " + duration_str;
}

String format_change_message(Status status, time_t duration)
{
  String duration_str = format_duration(duration);
  if (status == CONNECTED) return "+ Увімкнення світла.\nСвітла не було " + duration_str;
  else return "- Відключення світла.\nСвітло було " + duration_str;
}

bool is_connected_to_check_ip()
{
  return Ping.ping(CHECK_IP);
}

void save_status_data(const StatusData& data)
{
  EEPROM.put(0, data);
  EEPROM.commit();
}

void update_status_data(StatusData& data, Status status)
{
  time_t now = NTP.getUnix();
  data.status = status;
  data.timestamp = now;
  data.counter++;
  save_status_data(data);
}

void send_message(const String& text, const String& chatID)
{
  fb::Message message;
  message.text = text;
  message.chatID = chatID;

  bot.sendMessage(message);
}

String format_ping_message()
{
  bool ping_ok = is_connected_to_check_ip();
  return String(CHECK_IP) + " is " + (ping_ok ? "connected." : "not connected.");
}

void post_status_to_channel(String status, String chatID)
{
  send_message(status, chatID);
}

Status perform_connection_check()
{
  bool success = is_connected_to_check_ip();
  return success ? CONNECTED : DISCONNECTED;
}