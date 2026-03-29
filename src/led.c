#include <avr/io.h>
#include "board.h"
#include "led.h"

void led_init(void)
{
    DDRB |= (1 << LED_RED) | (1 << LED_YELLOW) | (1 << LED_BLUE) | (1 << LED_GREEN);
}

uint8_t led_on(led_color_t color)
{
    switch(color)
    {
        case GREEN:
            PORTB |= (1 << LED_GREEN);
        break;

        case BLUE:
            PORTB |= (1 << LED_BLUE);
        break;

        case YELLOW:
            PORTB |= (1 << LED_YELLOW);
        break;

        case RED:
            PORTB |= (1 << LED_RED);
        break;

        default:
            return 1;
        break;
    }

    return 0;
}

uint8_t led_off(led_color_t color)
{
    switch(color)
    {
        case GREEN:
            PORTB &= ~(1 << LED_GREEN);
        break;

        case BLUE:
            PORTB &= ~(1 << LED_BLUE);
        break;

        case YELLOW:
            PORTB &= ~(1 << LED_YELLOW);
        break;

        case RED:
            PORTB &= ~(1 << LED_RED);
        break;

        default:
            return 1 ;
        break;
    }

    return 0;
}