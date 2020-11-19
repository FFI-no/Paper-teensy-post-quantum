#include "hal.h"
#include <Arduino.h>

extern "C"{
  void hal_setup(const enum clock_mode clock)
  {
    // Ignorerer clock_mode, må settes i makefila.
    Serial.begin(115200);
    Serial1.begin(9600);
  }
  void hal_send_str(const char* in)
  {
    // Hvis denne bare sender ett tegn må vi konvertere litt
    Serial.println(in);
  }

  uint64_t hal_get_time()
  {
    return millis();
  }
}
