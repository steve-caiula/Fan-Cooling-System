#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint16_t pulse_count;

ISR (INT0_vect)
{

}

void fan_init ()
{

}

void fan_set_speed (uint8_t speed_percent)
{
    OCR1A = (ICR1 * speed_percent) / 100;
}

void fan_get_rpm ()
{

}

uint8_t fan_get_status ()
{

}