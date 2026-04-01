#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H


#include <stdint.h>
#include "board.h"


#define TIMER0_OCR0A_VALUE  (CPU_FREQ / 64 / 1000 - 1)   // Comparison value (16MHz / 64 / 1000Hz - 1)


extern volatile uint32_t system_millis;


//PUBLIC FUNCTIONS
/*
   Configures Timer 0 in CTC mode for 1ms interrupt-driven timekeeping.
   Must be called before any driver that uses get_millis().
*/
void system_timer_init(void);


/* 
   Returns the current millisecond count with atomic access.
   Since the AVR is an 8-bit architecture, reading a 32-bit variable (4 bytes)
   takes multiple CPU cycles. This function disables interrupts during the read
   to prevent the timer from updating the value mid-access, which would
   result in data corruption.
*/
uint32_t get_millis (void);


#endif