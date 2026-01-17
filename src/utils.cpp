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

String checkConnectionStatus()
{
  String ip = CHECK_IP;
  ip.trim();
  bool success = Ping.ping(ip.c_str());
  Status newStatus = success ? CONNECTED : DISCONNECTED;
  if (newStatus != currentStatus) {
    currentStatus = newStatus;
    lastTimestamp = NTP.getUnix();
    StatusData newData = {currentStatus, lastTimestamp};
    EEPROM.put(0, newData);
    EEPROM.commit();
  }
  return formatStatusMessage(success);
}