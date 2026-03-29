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


uint32_t get_millis (void)
{
    uint32_t ms_copy;   // Temporary variable

    cli ();                    // Disable interrupts to protect the 32-bit read
    ms_copy = system_millis;   // Copy system_millis to a local variable while interrupts are disabled
    sei ();                    // Re-enable interrupts immediately after

    return ms_copy;   // Returns the protected copy
}