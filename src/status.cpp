#include <Arduino.h>
#include "status.h"

void init_lights() {
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

void set_lights(const int status) {
  if (status >= LEDGREEN) {
    digitalWrite(greenLED, HIGH);
  } else {
    digitalWrite(greenLED, LOW);
  }
  if (status % LEDGREEN >= LEDYELLOW) {
    digitalWrite(yellowLED, HIGH);
  } else {
    digitalWrite(yellowLED, LOW);
  }
  if (status % LEDYELLOW >= LEDRED) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(redLED, LOW);
  }
}
