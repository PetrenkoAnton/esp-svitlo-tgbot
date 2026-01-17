#include <FastBot2.h>
#include "handler.h"
#include <GyverNTP.h>
#include "ESP8266WiFi.h"
#ifdef ESP8266
#include <ESP8266Ping.h>
#else
#include <ESP32Ping.h>
#endif
#include <EEPROM.h>

extern FastBot2 bot;

enum Status { CONNECTED, DISCONNECTED };
struct StatusData { Status status; time_t timestamp; };
extern Status currentStatus;
extern time_t lastTimestamp;

const uint32_t CMD_START = "/start"_h;
const uint32_t CMD_IP = "/ip"_h;
const uint32_t CMD_CHECK = "/check"_h;

void handle(fb::Update &u)
{
  if (u.isMessage() && (u.message().text().hash() == "/start"_h || u.message().text().hash() == "/ip"_h || u.message().text().hash() == "/check"_h))
    handle_message(u);
  if (u.isQuery())
    handle_query(u);
}

void handle_message(fb::Update &u)
{
  uint32_t cmd = u.message().text().hash();
  switch (cmd) {
    case CMD_IP:
      message_builder(WiFi.localIP().toString(), u);
      break;
    case CMD_CHECK: {
      String ip = CHECK_IP;
      ip.trim();
      bool success = Ping.ping(ip.c_str());
      Status newStatus = success ? CONNECTED : DISCONNECTED;
      if (newStatus != currentStatus) {
        currentStatus = newStatus;
        lastTimestamp = NTP.getUnix();
        StatusData newData = {currentStatus, lastTimestamp};
        EEPROM.put(0, newData);
        EEPROM.commit();
      }
      String status = formatStatusMessage(success);
      message_builder(status, u);
      // Also post to group
      fb::Message message;
      message.chatID = CHANNEL_ID;
      message.text = status;
      bot.sendMessage(message);
      break;
    }
    default:
      message_builder("Available commands:\n/start - Show this message\n/ip - Get local IP\n/check - Check connection to CHECK_IP", u);
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

  bot.sendMessage(message);
}

String formatStatusMessage(bool success)
{
  return success ? "Світло є" : "Світла немає";
}
