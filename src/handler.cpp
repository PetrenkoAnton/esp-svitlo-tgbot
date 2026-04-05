#include <FastBot2.h>
#include <time.h>
#include "handler.h"
#include "utils.h"
#include <ESP8266WiFi.h>

extern FastBot2 bot;

Status pending_status = UNDEFINED;
time_t pending_timestamp = 0;
Status original_status = UNDEFINED;

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

void perform_initial_check(StatusData& status_data, bool is_manual_call)
{
  Status status = perform_connection_check();
  status_data.status = status;
  status_data.timestamp = time(NULL);

  save_status_data(status_data);

  String message = String((status == CONNECTED) ? "💡" : "🚫") + " Наразі світл" + String((status == CONNECTED) ? "о є" : "а немає") + ".";
  
  if (is_manual_call) send_message(message, ADMIN_ID);
}

void perform_regular_check(StatusData& status_data, bool is_manual_call)
{
  Status status = perform_connection_check();
  bool changed = (status != status_data.status);
  time_t current_time;
  current_time = time(NULL);

  time_t duration = current_time - status_data.timestamp;
  
  if (changed) {
    if (pending_status == UNDEFINED) {
      original_status = status_data.status;
      pending_status = status;
      pending_timestamp = current_time;
      // send pending message
      String msg = "⚠️ Possible status change detected: from " + String(original_status == CONNECTED ? "connected" : "disconnected") + " to " + String(status == CONNECTED ? "connected" : "disconnected") + ". Waiting for confirmation.";
      send_message(msg, ADMIN_ID);
    } else if (pending_status == status && current_time - pending_timestamp >= INTERVAL) {
      // confirmed
      String message = format_change_message(status, duration);

      update_status_data(status_data, status);

      if (is_manual_call) send_message(message, ADMIN_ID);
      
      send_message(message, CHANNEL_ID);
      // send confirmed to admin
      String confirm_msg = "✅ Status change confirmed: " + String(original_status == CONNECTED ? "connected" : "disconnected") + " → " + String(status == CONNECTED ? "connected" : "disconnected");
      send_message(confirm_msg, ADMIN_ID);
      pending_status = UNDEFINED;
      pending_timestamp = 0;
      original_status = UNDEFINED;
    } else if (status == original_status) {
      // declined
      String decline_msg = "❌ Status change declined: returned to " + String(status == CONNECTED ? "connected" : "disconnected");
      send_message(decline_msg, ADMIN_ID);
      pending_status = UNDEFINED;
      pending_timestamp = 0;
      original_status = UNDEFINED;
    } else if (pending_status != status) {
      // new change
      original_status = status_data.status;
      pending_status = status;
      pending_timestamp = current_time;
      // send new pending
      String msg = "⚠️ Possible status change detected: from " + String(original_status == CONNECTED ? "connected" : "disconnected") + " to " + String(status == CONNECTED ? "connected" : "disconnected") + ". Waiting for confirmation.";
      send_message(msg, ADMIN_ID);
    }
  } else {
    // no change, reset
    pending_status = UNDEFINED;
    pending_timestamp = 0;
    original_status = UNDEFINED;
    if (is_manual_call) send_message(format_current_message(status, duration), ADMIN_ID);
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
