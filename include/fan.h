#ifndef FAN_H
#define FAN_H 

#include "pin_definitions.h"

#define FAN_ICR1_VALUE (CPU_FREQ / 25000UL - 1)  // 16MHz / 25kHz - 1 = 639

#endif