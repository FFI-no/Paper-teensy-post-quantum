#ifndef STATUS_H
#define STATUS_H

const int greenLED = 35;
const int yellowLED = 34;
const int redLED = 33;

const int NOLIGHTS = 0;
const int LEDRED = 1;
const int LEDYELLOW = 2;
const int LEDGREEN = 4;

void init_lights();
void set_lights(const int status);

#endif
