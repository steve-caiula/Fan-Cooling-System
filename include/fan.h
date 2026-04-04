#ifndef FAN_H
#define FAN_H 

#include <stdint.h>
#include "board.h"


// ICR1 VALUE
#define FAN_ICR1_VALUE  (CPU_FREQ / 25000UL - 1)   // 16MHz / 25kHz - 1 = 639


// STATUS
typedef enum 
{
   FAN_OK,
   FAN_ERROR_STALL,
   FAN_ERROR_UNDERSPEED
} fan_status_t;


// PUBLIC FUNCTIONS
/*
   Configures Timer 1 for 25kHz PWM on OC1A (D9) and
   INT0 for tachometer pulse counting on D2.
*/
void fan_init(void);


/*
   Sets fan speed by writing duty cycle percentage (0-100) to OCR1A.
   0 = off, 100 = maximum speed.
*/
void fan_set_speed(uint8_t duty_cycle);


/*
   Returns fan speed in RPM calculated over a 1 second sampling window.
   Non-blocking, must be called repeatedly from the main loop.
*/
uint16_t fan_get_rpm(void);


/*
   Returns fan operational status based on RPM and duty cycle.
   Detects stall (RPM = 0 with active PWM) and underspeed conditions.
*/
fan_status_t fan_get_status(uint16_t rpm, uint8_t duty_cycle);


#endif