#pragma once

enum Status { UNDEFINED, DISCONNECTED, CONNECTED };
struct StatusData { Status status; time_t timestamp; unsigned short counter; };
struct CheckResult { bool changed; String message; };

extern StatusData status_data;
extern FastBot2 bot;

String format_status_message(bool success);
CheckResult check_connection_status(StatusData& data);
String format_duration(time_t seconds);
String format_current_message(bool success, time_t duration);
String format_change_message(bool success, time_t duration);
void post_status_to_channel(String status);
void clear_eeprom_data();
bool is_connected();
String format_status_message();