#include <FastBot2.h>
#include "handler.h"
#include <GyverNTP.h>

extern FastBot2 bot;

void handle(fb::Update &u)
{
  if (u.isMessage() && u.message().text().hash() == "/start"_h)
    handle_message(u);
  if (u.isQuery())
    handle_query(u);
}

void handle_message(fb::Update &u)
{
  message_builder("/start", u);
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
