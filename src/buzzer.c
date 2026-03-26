#include <avr/io.h>
#include <stdint.h>
#include "pin_definitions.h"
#include "buzzer.h"
#include "timer0.h"

void buzzer_init (void)
{
    DDRD |= (1 << BUZZER);
}

static void buzzer_set_tone (void)
{
    TCCR2A = (1 << WGM21) | (1 << COM2B0);
    TCCR2B = (0 << WGM22) | (1 << CS22) | (0 << CS21) | (0 << CS20);

    OCR2A = BUZZER_OCR2A_VALUE;
}

static void buzzer_stop (void)
{
    TCCR2A = 0;
    TCCR2B = 0;
}

void buzzer_alarm_sensor (void)
{
    buzzer_set_tone ();
}

void buzzer_alarm_lcd (void)
{
    
    static uint32_t last_toggle = 0;
    static uint8_t buzzer_status = 1;

    uint32_t current_time = get_millis();

    if (buzzer_status == 1)
    {
        if (current_time - last_toggle >= BUZZER_BLINK_ON_MS)
        {
            buzzer_stop ();
            last_toggle = current_time;
            buzzer_status = 0;
        }
    }

    else 
    {
        if (current_time - last_toggle >= BUZZER_BLINK_OFF_MS)
        {
            buzzer_set_tone ();
            last_toggle = current_time;
            buzzer_status = 1;
        }
    }
}