#pragma once

void handle(fb::Update &u);
void handle_message(fb::Update &u);
void handle_query(fb::Update &u);
void message_builder(String text, fb::Update &u);
void handle_status_check();