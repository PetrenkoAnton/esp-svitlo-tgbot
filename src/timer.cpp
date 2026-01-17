#include "timer.h"
#include <Arduino.h>

bool timer(unsigned long &expire, const unsigned long period) {
  if (millis() - expire >= period) {
    expire = millis();
    return true;
  }

  return false;
}