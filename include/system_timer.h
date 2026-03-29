#ifndef MCU_TIMER0
#define MCU_TIMER0


#include <stdint.h>


extern volatile uint32_t system_millis;


//PUBLIC FUNCTIONS
uint32_t get_millis (void);


#endif