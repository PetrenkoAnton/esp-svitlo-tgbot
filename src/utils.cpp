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
  bool success = is_connected_to_check_ip();
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
    save_status_data(data);
  }
  return CheckResult{changed, message};
}

void post_status_to_channel(String status) {
  send_message(status, CHANNEL_ID);
}

void clear_eeprom_data()
{
  for (size_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0xFF); 
  }
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
  EEPROM.put(0, data);
  EEPROM.commit();
}

void debug_print(const String& msg)
{
  #ifdef DEBUG
  Serial.println(msg);
  #endif
}

void send_message(const String& text, const String& chatID)
{
  fb::Message message;
  message.text = text;
  message.chatID = chatID;
  debug_print(text);
  bot.sendMessage(message);
}

void connect_to_wifi()
{
  #ifdef DEBUG
  unsigned short i = 1;
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    #ifdef DEBUG
    debug_print("Connecting to WiFi... (" + String(i++) + ")");
    #endif
  }
}

void init_bot()
{
  bot.attachUpdate(handle);
  bot.setToken(F(BOT_TOKEN));
  bot.setPollMode(fb::Poll::Long, 20000);
  bot.skipUpdates(-10);
}

void send_startup_message()
{
  send_message("Мікроконтролер підключено", CHANNEL_ID);
  #ifdef DEBUG
  unsigned short i = 1;
  #endif
  while (!bot.lastBotMessage())
  {
    delay(500);
    debug_print("Connecting to Telegram... (" + String(i++) + ")");
  }
}