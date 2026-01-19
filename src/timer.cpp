#include "timer.h"
#include <Arduino.h>

bool timer(unsigned long &timer_expire, const unsigned long period) {
  if (millis() - timer_expire >= period * 1000) {
    timer_expire = millis();
    return true;
  }

  return false;
}