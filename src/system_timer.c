/*
   System timer driver using Timer 0 in CTC mode.
   Provides millisecond resolution timekeeping via get_millis().
   Used by all drivers for non-blocking state machine timing.
*/


#include <avr/interrupt.h>
#include "system_timer.h"


volatile uint32_t system_millis = 0;   // Global millisecond counter updated by TIMER0


/*
   Timer 0 compare match interrupt handler.
   Increments system_millis every 1ms.
*/
ISR (TIMER0_COMPA_vect)
{
    system_millis++;   // + 1 millis
}


void system_timer_init(void)
{
    TCCR0A |= (1 << WGM01);                // CTC mode
    TCCR0B |= (1 << CS01) | (1 << CS00);   // Prescaler 64
    OCR0A = TIMER0_OCR0A_VALUE;            // Comparison value (16MHz / 64 / 1000Hz - 1)
    TIMSK0 |= (1 << OCIE0A);               // Enable interrupt for A comparison
}


uint32_t get_millis (void)
{
    uint32_t ms_copy;   // Temporary variable

    cli ();                    // Disable interrupts to protect the 32-bit read
    ms_copy = system_millis;   // Copy system_millis to a local variable while interrupts are disabled
    sei ();                    // Re-enable interrupts immediately after

    return ms_copy;   // Returns the protected copy
}