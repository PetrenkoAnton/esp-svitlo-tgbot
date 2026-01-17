#pragma once

String formatStatusMessage(bool success);
String checkConnectionStatus();
String formatDuration(time_t seconds);
String formatCurrentMessage(bool success, time_t duration);
String formatChangeMessage(bool success, time_t duration);
#ifdef CLEAR_EEPROM
void clearEEPROMData();
#endif