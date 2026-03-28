#ifndef FAN_H
#define FAN_H 

#include <avr/interrupt.h>
#include "board.h"

#define FAN_ICR1_VALUE (CPU_FREQ / 25000UL - 1)  // 16MHz / 25kHz - 1 = 639

#define FAN_RPM_PER_DUTY    18  // 1800 RPM max / 100
#define FAN_SPEED_TOLERANCE 70  // Minimum acceptable RPM percentage

// FAN STATUS
typedef enum 
{
    FAN_OK,
    FAN_ERROR_STALL,
    FAN_ERROR_UNDERSPEED
} fan_status_t;

// PUBLIC FUNCTIONS
ISR (INT0_vect);
void fan_init (void);
void fan_set_speed (uint8_t duty_cycle);
uint16_t fan_get_rpm (void);
fan_status_t fan_get_status (uint16_t rpm, uint8_t duty_cycle);

#endif