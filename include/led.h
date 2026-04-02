#ifndef LED_H
#define LED_H


#include <stdint.h>


// COLORS
typedef enum 
{
   GREEN,
   BLUE,
   YELLOW,
   RED
} led_color_t;


// PUBLIC FUNCTIONS
/*
   Configures PB2-PB5 as output. Must be called once at startup.
*/
void led_init(void);


/*
   Turns on the LED of the specified color.
   Returns 0 on success, 1 on invalid argument.
*/
uint8_t led_on(led_color_t color);


/*
   Turns off the LED of the specified color.
   Returns 0 on success, 1 on invalid argument.
*/
uint8_t led_off(led_color_t color);


#endif