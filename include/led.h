#ifndef LED_H
#define LED_H


#include <stdint.h>
#include <stdint.h>


typedef enum 
{
    GREEN,
    BLUE,
    YELLOW,
    RED
} led_color_t;


void led_init(void);
uint8_t led_on(led_color_t color);
uint8_t led_off(led_color_t color);


#endif