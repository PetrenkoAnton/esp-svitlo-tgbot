#pragma once

#include "utils.h"

void handle(fb::Update &u);
void handle_message(fb::Update &u);
void message_builder(String text, fb::Update &u);
void handle_status_check(StatusData& status_data);
void perform_initial_check(StatusData& status_data);
void perform_regular_check(StatusData& status_data);