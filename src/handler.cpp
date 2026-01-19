#include <FastBot2.h>
#include "handler.h"
#include "utils.h"
#include <ESP8266WiFi.h>
#include <GyverNTP.h>

extern FastBot2 bot;
extern GyverNTP NTP;

const uint32_t CMD_START = "/start"_h;
const uint32_t CMD_STATUS = "/status"_h;
const uint32_t CMD_CURRENT = "/current"_h;
const uint32_t CMD_CLEAR_EEPROM = "/clear_eeprom"_h;

const String START_MESSAGE = "Available commands:\n\n/start - Show this message\n/status - Get system status\n/current - Get current electricity status\n/clear_eeprom - Clear EEPROM data";
const String EEPROM_CLEARED_MESSAGE = "EEPROM cleared";

void handle(fb::Update &u)
{
  if (u.isMessage())
    handle_message(u);
}

void handle_message(fb::Update &u)
{
  if (u.message().from().id() != ADMIN_ID) return;
  uint32_t cmd = u.message().text().hash();
  switch (cmd) {
    case CMD_START:
      message_builder(START_MESSAGE, u);
      break;
    case CMD_STATUS:
      message_builder(build_status_info(), u);
      break;
    case CMD_CURRENT:
      handle_status_check(status_data, true);
      break;
    case CMD_CLEAR_EEPROM:
      status_data.status = UNDEFINED;
      status_data.timestamp = 0;
      status_data.counter++;
      save_status_data(status_data);
      message_builder(EEPROM_CLEARED_MESSAGE, u);
      break;
    default:
      // Ignore all other commands and messages
      break;
  }
}

void message_builder(String text, fb::Update &u)
{
  send_message(text, String(u.message().chat().id()));
}

String build_status_info()
{
  String info = format_ping_message() + "\n\n";
  info += "IP: " + WiFi.localIP().toString() + "\n";
  info += "EEPROM rewrites: " + String(status_data.counter);
  #if DEBUG
  Serial.println("StatusData:" + String(status_data.status) + " | " + String(status_data.timestamp) + " | " + String(status_data.counter));
  #endif
  return info;
}

}

void perform_initial_check(StatusData& status_data, bool is_manual_call)
{
  bool save_to_eeprom = !is_manual_call;
  String chatID = is_manual_call ? ADMIN_ID : CHANNEL_ID;
  Status new_status = perform_connection_check();
  status_data.status = new_status;
  status_data.timestamp = NTP.getUnix();
  if (save_to_eeprom) save_status_data(status_data);
  String message = "Наразі світл" + String((new_status == CONNECTED) ? "о є" : "а немає") + " (поточна тривалість невідома і буде вираховуватись з цього моменту).";
  post_status_to_channel(message, chatID);
}

void perform_regular_check(StatusData& status_data, bool is_manual_call)
{
  bool save_to_eeprom = !is_manual_call;
  String chat_id = is_manual_call ? ADMIN_ID : CHANNEL_ID;
  Status new_status = perform_connection_check();
  bool changed = (new_status != status_data.status);
  time_t duration = NTP.getUnix() - status_data.timestamp;
  
  if (changed) {
    String message = format_change_message((new_status == CONNECTED), duration);

    if (save_to_eeprom) update_status_data(status_data, new_status);
    send_message(message, ADMIN_ID);
    post_status_to_channel(message, chat_id);
  } else {
    String message = format_current_message((new_status == CONNECTED), duration);
    post_status_to_channel(message, chat_id);
  }
}

void handle_status_check(StatusData& status_data, bool is_manual_call)
{
    if (status_data.status == UNDEFINED) {
      perform_initial_check(status_data, is_manual_call);
    } else {
      perform_regular_check(status_data, is_manual_call);
    }
}
  bool success = is_connected_to_check_ip();
}
