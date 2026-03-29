#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H


#include <stdint.h>


extern volatile uint32_t system_millis;


//PUBLIC FUNCTIONS
/* 
   Returns the current millisecond count with atomic access.
   Since the AVR is an 8-bit architecture, reading a 32-bit variable (4 bytes)
   takes multiple CPU cycles. This function disables interrupts during the read
   to prevent the timer from updating the value mid-access, which would
   result in data corruption.
*/
uint32_t get_millis (void);


#endif