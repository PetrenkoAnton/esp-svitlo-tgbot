#include <Arduino.h>
#include <time.h>
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
  if (seconds < 0) return "(невідомо)";
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
  if (status == CONNECTED) return "💡 Світло є вже " + duration_str;
  else return "🚫 Світла немає вже " + duration_str;
}

String format_change_message(Status status, time_t duration)
{
  String duration_str = format_duration(duration);
  if (status == CONNECTED) return "💡 Увімкнення світла.\nСвітла не було " + duration_str;
  else return "🚫 Відключення світла.\nСвітло було " + duration_str;
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
  data.status = status;
  data.timestamp = time(NULL);

  data.counter++;
  save_status_data(data);
}

void send_message(const String& text, const String& chat_id)
{
  fb::Message message;
  message.text = text;
  message.chatID = chat_id;

  bot.sendMessage(message);
}

String format_ping_message()
{
  return String(CHECK_IP) + " is " + (is_connected_to_check_ip() ? "connected." : "not connected.");
}

Status perform_connection_check()
{
  return is_connected_to_check_ip() ? CONNECTED : DISCONNECTED;
}