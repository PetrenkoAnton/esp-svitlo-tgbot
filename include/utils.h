#pragma once

enum Status { CONNECTED, DISCONNECTED };
struct StatusData { Status status; time_t timestamp; unsigned long counter; };
struct CheckResult { bool changed; String message; };

extern StatusData currentData;
extern FastBot2 bot;

String formatStatusMessage(bool success);
CheckResult checkConnectionStatus(StatusData& data);
String formatDuration(time_t seconds);
String formatCurrentMessage(bool success, time_t duration);
String formatChangeMessage(bool success, time_t duration);
void postStatusToChannel(String status);
void clearEEPROMData();
#ifdef CLEAR_EEPROM
void clearEEPROMData();
#endif