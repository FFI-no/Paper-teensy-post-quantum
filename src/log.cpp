#include <Arduino.h>
#include "log.h"

void log_begin(){
  Serial.begin(115200);
}

bool log_interface_ready(){
  return Serial.available();
}

void log_message(String msg) {
  Serial.print(msg);
}

void log_signature_not_verified(){
  Serial.println("Signature not verified.");
}

void log_signature_verified(){
  Serial.println("Signature verified.");
}

void log_print_bytes(const unsigned char *bytes, size_t len, const uint8_t wrap){
  char b[3] = {0};
  int j = 0;
  for (uint i = 0; i < len; i++){
    sprintf(b, "%02X", bytes[i]);
    Serial.print(b);
    Serial.print(" ");
    j++;
    if ((wrap > 0) && (j == wrap)){
      Serial.print('\n');
      j = 0;
    }
  }
  Serial.print('\n');
}
