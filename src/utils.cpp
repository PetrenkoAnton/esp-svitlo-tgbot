#include <Arduino.h>
#include <GyverNTP.h>
#include <EEPROM.h>
#ifdef ESP8266
#include <ESP8266Ping.h>
#else
#include <ESP32Ping.h>
#endif

enum Status { CONNECTED, DISCONNECTED };
struct StatusData { Status status; time_t timestamp; };
extern Status currentStatus;
extern time_t lastTimestamp;

String formatStatusMessage(bool success)
{
  return success ? "Світло є" : "Світла немає";
}

String formatDuration(time_t seconds)
{
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  return String(hours) + " годин " + String(minutes) + " хвилин";
}

String formatCurrentMessage(bool success, time_t duration)
{
  String dur = formatDuration(duration);
  if (success) return "Світло є вже " + dur + ".";
  else return "Світла немає вже " + dur + ".";
}

String formatChangeMessage(bool success, time_t duration)
{
  String dur = formatDuration(duration);
  if (success) return "Світло з'явилось. Світла не було " + dur + ".";
  else return "Світло зникло. Світла було " + dur + ".";
}

String checkConnectionStatus()
{
  time_t now = NTP.getUnix();
  time_t duration = now - lastTimestamp;
  String ip = CHECK_IP;
  ip.trim();
  bool success = Ping.ping(ip.c_str());
  Status newStatus = success ? CONNECTED : DISCONNECTED;
  bool changed = (newStatus != currentStatus);
  String message;
  if (changed) {
    message = formatChangeMessage(success, duration);
  } else {
    message = formatCurrentMessage(success, duration);
  }
  if (changed) {
    currentStatus = newStatus;
    lastTimestamp = now;
    StatusData newData = {currentStatus, lastTimestamp};
    EEPROM.put(0, newData);
    EEPROM.commit();
  }
  return message;
}

#ifdef CLEAR_EEPROM
void clearEEPROMData()
{
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.put(i, 255);
  }
}
#endif