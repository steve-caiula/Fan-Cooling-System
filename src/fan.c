#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "fan.h"
#include "timer0.h"

volatile uint16_t pulse_count = 0;

ISR (INT0_vect)
{
    pulse_count++;
}

void fan_init (void)
{
    DDRB |= (1 << PB1);

    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);

    ICR1 = FAN_ICR1_VALUE; // Frequency 25kHz

    DDRD &= ~(1 << PD2);

    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
}

void fan_set_speed (uint8_t duty_cycle)
{
    OCR1A = (ICR1 * duty_cycle) / 100;
}

uint16_t fan_get_rpm (void)
{
    static uint32_t last_sample_time;
    static uint16_t last_pulse_count;
    static uint16_t rpm;
    
    uint16_t elapsed_pulse;

    uint32_t current_time = get_millis ();

    if ((current_time - last_sample_time) >= 1000)
    {
        cli ();
        elapsed_pulse = pulse_count - last_pulse_count;
        last_pulse_count = pulse_count;
        sei ();

        rpm = (elapsed_pulse / 2) * 60;
        last_sample_time = current_time;
    }

    return rpm;
}

uint8_t fan_get_status ()
{

}