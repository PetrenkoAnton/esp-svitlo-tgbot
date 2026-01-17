#include <FastBot2.h>
#include "handler.h"
#include "utils.h"
#include <ESP8266WiFi.h>
#ifdef ESP8266
#include <ESP8266Ping.h>
#else
#include <ESP32Ping.h>
#endif
#include <EEPROM.h>
#include <GyverNTP.h>

extern FastBot2 bot;
extern GyverNTP NTP;

const uint32_t CMD_START = "/start"_h;
const uint32_t CMD_STATUS = "/status"_h;
const uint32_t CMD_CLEAR_EEPROM = "/clear_eeprom"_h;
const uint32_t CMD_REWRITE_EEPROM = "/rewrite_eeprom"_h;

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
      message_builder("Available commands:\n\n/start - Show this message\n/status - Get system status\n/clear_eeprom - Clear EEPROM data\n/rewrite_eeprom - Rewrite current data to EEPROM", u);
      break;
    case CMD_STATUS: {
      String info = format_ping_message() + "\n\n";
      info += "IP: " + WiFi.localIP().toString() + "\n";
      info += "EEPROM rewrites: " + String(status_data.counter);
      message_builder(info, u);
      break;
    }
    case CMD_CLEAR_EEPROM:
      clear_eeprom_data();
      message_builder("EEPROM cleared", u);
      break;
    case CMD_REWRITE_EEPROM:
      status_data.counter++;
      EEPROM.put(0, status_data);
      EEPROM.commit();
      message_builder("EEPROM rewritten", u);
      break;
    default:
      // Ignore all other commands and messages
      break;
  }
}

void message_builder(String text, fb::Update &u)
{
  fb::Message message;
  message.text = text;
  message.chatID = u.message().chat().id();

  #ifdef DEBUG
  Serial.println(message.text);
  #endif
  bot.sendMessage(message);
}

void handle_status_check()
{
  if (status_data.status == UNDEFINED) {
    bool success = Ping.ping(CHECK_IP);
    status_data.status = success ? CONNECTED : DISCONNECTED;
    status_data.timestamp = NTP.getUnix();
    EEPROM.put(0, status_data);
    EEPROM.commit();
    String message = "Наразі світло " + String(success ? "є" : "немає") + ", (поточна тривалість невідома)";
    post_status_to_channel(message);
  } else {
    CheckResult result = check_connection_status(status_data);
    if (result.changed) {
      post_status_to_channel(result.message);
    }
  }
}
