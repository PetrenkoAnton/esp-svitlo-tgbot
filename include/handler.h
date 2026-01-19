#pragma once

#include "utils.h"

void handle(fb::Update &u);
void handle_message(fb::Update &u);
void message_builder(String text, fb::Update &u);
String build_status_info();
void handle_status_check(StatusData& status_data, bool is_manual_call = false);
void perform_initial_check(StatusData& status_data, bool is_manual_call = false);
void perform_regular_check(StatusData& status_data, bool is_manual_call = false);
