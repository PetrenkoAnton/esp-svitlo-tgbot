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
struct StatusData { Status status; DateTime datetime; };
extern Status currentStatus;

void handle(fb::Update &u)
{
  if (u.isMessage() && (u.message().text().hash() == "/start"_h || u.message().text().hash() == "/ip"_h || u.message().text().hash() == "/check"_h))
    handle_message(u);
  if (u.isQuery())
    handle_query(u);
}

void handle_message(fb::Update &u)
{
  if (u.message().text().hash() == "/ip"_h)
    message_builder(WiFi.localIP().toString(), u);
  else if (u.message().text().hash() == "/check"_h)
  {
    String ip = LOCAL_IP;
    ip.trim();
    bool success = Ping.ping(ip.c_str());
    Status newStatus = success ? CONNECTED : DISCONNECTED;
    if (newStatus != currentStatus) {
      currentStatus = newStatus;
      EEPROM.write(0, currentStatus);
      EEPROM.commit();
    }
    String status = success ? "Світло є" : "Світла немає";
    message_builder(status, u);    // Also post to group
    fb::Message groupMsg;
    groupMsg.chatID = CHANNEL_ID;
    groupMsg.text = status;
    bot.sendMessage(groupMsg);  }
  else
    message_builder("Available commands:\n/start - Show this message\n/ip - Get local IP\n/check - Check connection to LOCAL_IP", u);
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
