#include <FastBot2.h>
#include "handler.h"
#include "utils.h"
#include <ESP8266WiFi.h>
#include <GyverNTP.h>

extern FastBot2 bot;
extern GyverNTP NTP;

const uint32_t CMD_START = "/start"_h;
const uint32_t CMD_STATUS = "/status"_h;
const uint32_t CMD_CLEAR_EEPROM = "/clear_eeprom"_h;

const String START_MESSAGE = "Available commands:\n\n/start - Show this message\n/status - Get system status\n/clear_eeprom - Clear EEPROM data";
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
  // debug
  info += "\n\n\nStatusData: " + String(status_data.status) + " | " + String(status_data.timestamp) + " | " + String(status_data.counter);
  return info;
}

void perform_initial_check(StatusData& status_data)
{
  bool success = is_connected_to_check_ip();
  status_data.status = success ? CONNECTED : DISCONNECTED;
  status_data.timestamp = NTP.getUnix();
  save_status_data(status_data);
  String message = "Наразі світл" + String(success ? "о є" : "а немає") + " (поточна тривалість невідома і буде вираховуватись з цього моменту).";
  post_status_to_channel(message);
}

void perform_regular_check(StatusData& status_data)
{
  CheckResult result = check_connection_status(status_data);
  if (result.changed) {
    post_status_to_channel(result.message);
  }
}

void handle_status_check(StatusData& status_data)
{
  if (status_data.status == UNDEFINED) {
    perform_initial_check(status_data);
  } else {
    perform_regular_check(status_data);
  }
}
