#pragma once

#include <GyverNTP.h>

enum Status { UNDEFINED, DISCONNECTED, CONNECTED };
struct StatusData { Status status; time_t timestamp; unsigned short counter; };
struct CheckResult { bool changed; String message; Status new_status; };

extern StatusData status_data;
extern FastBot2 bot;
extern GyverNTP NTP;

CheckResult check_connection_status(StatusData& data);
String format_duration(time_t seconds);
String format_current_message(bool success, time_t duration);
String format_change_message(bool success, time_t duration);
void post_status_to_channel(String status, String chatID = CHANNEL_ID);
String format_ping_message();

bool is_connected_to_check_ip();
void save_status_data(const StatusData& data);
void update_status_data(StatusData& data, Status new_status);
void send_message(const String& text, const String& chatID);

String format_ping_message();