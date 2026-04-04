/*
   Buzzer driver for passive piezo buzzer on OC2B (D3).
   Generates tones in hardware via Timer 2 CTC mode.
   Provides continuous alarm for sensor faults and
   intermittent alarm for LCD faults.
*/


#include <avr/io.h>
#include "buzzer.h"
#include "system_timer.h"


static uint8_t buzzer_status = 0;   // Tracks current buzzer state: 0 = off, 1 = on


void buzzer_init(void)
{
    DDRD |= (1 << BUZZER);   // Set buzzer pin as output
}


/*
   Configures Timer 2 in CTC mode to generate a tone on OC2B (D3).
   The hardware toggles the pin automatically at the target frequency.
*/
static void buzzer_set_tone(void)
{
    TCCR2A = 0;   // Reset Timer 2 control registers before reconfiguring
    TCCR2B = 0;   // Ensures clean state after previous buzzer_stop() or buzzer_alarm_critical()
    
    TCCR2A = (1 << WGM21) | (1 << COM2B0);                             // CTC mode, toggle OC2B (D3) on compare match
    TCCR2B = (0 << WGM22) | (1 << CS22) | (0 << CS21) | (0 << CS20);   // Prescaler 64

    OCR2A = BUZZER_OCR2A_VALUE;    // Set compare value for target frequency
}


void buzzer_stop(void)
{
    TCCR2A = 0;   // Disable CTC mode and OC2B toggle
    TCCR2B = 0;   // Stop timer clock

    buzzer_status = 0;
}


void buzzer_alarm_critical(void)
{
    if (buzzer_status == 0) 
    {
        buzzer_set_tone();   // Start continuous tone
        buzzer_status = 1;   // Mark as active to prevent timer reconfiguration
    }
}


void buzzer_alarm_warning(void)
{
    static uint32_t last_toggle = 0;        // Timestamp of last buzzer state change
    uint32_t current_time = get_millis();   // Current system time in milliseconds

    if (buzzer_status == 1)
    {
        if (current_time - last_toggle >= BUZZER_BLINK_ON_MS)   // Tone on time elapsed
        {
            buzzer_stop();                // Silence buzzer
            last_toggle = current_time;   // Reset timestamp
            buzzer_status = 0;            // Update state to off
        }
    }

    else 
    {
        if (current_time - last_toggle >= BUZZER_BLINK_OFF_MS)   // Tone off time elapsed
        {
            buzzer_set_tone();            // Activate buzzer
            last_toggle = current_time;   // Reset timestamp
            buzzer_status = 1;            // Update state to on
        }
    }
}