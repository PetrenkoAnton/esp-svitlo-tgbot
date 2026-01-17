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

extern FastBot2 bot;

const uint32_t CMD_START = "/start"_h;
const uint32_t CMD_STATUS = "/status"_h;
const uint32_t CMD_CLEAR_EEPROM = "/clear_eeprom"_h;
const uint32_t CMD_REWRITE_EEPROM = "/rewrite_eeprom"_h;

void handle(fb::Update &u)
{
  if (u.isMessage())
    handle_message(u);
  if (u.isQuery())
    handle_query(u);
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
      String info = checkConnectionStatus(currentData).message + "\n\n";
      info += "IP: " + WiFi.localIP().toString() + "\n";
      info += "EEPROM writes: " + String(currentData.counter);
      message_builder(info, u);
      break;
    }
    case CMD_CLEAR_EEPROM:
      clearEEPROMData();
      message_builder("EEPROM cleared", u);
      break;
    case CMD_REWRITE_EEPROM:
      EEPROM.put(0, currentData);
      EEPROM.commit();
      message_builder("EEPROM rewritten", u);
      break;
    default:
      // Ignore all other commands and messages
      break;
  }
}

void handle_query(fb::Update &u)
{
  message_builder("Unknown query", u);

  bot.answerCallbackQuery(u.query().id(), "Success");
}

void message_builder(String text, fb::Update &u)
{
  fb::Message message;
  message.text = text;
  message.chatID = u.isQuery()
                       ? u.query().message().chat().id()
                       : u.message().chat().id();

  #ifdef DEBUG
  Serial.println(message.text);
  #endif
  bot.sendMessage(message);
}
