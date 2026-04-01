#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "fan.h"
#include "system_timer.h"
#include "temp_sensor.h"
#include "lcd.h"
#include "buzzer.h"
#include "led.h"
#include "config.h"


int main(void)
{
    system_timer_init();
    fan_init();
    lcd_init();
    buzzer_init();
    led_init();
    sei();

    int16_t raw_temperature;
    float temp_celsius;
    char temp_string[10];

    while (1) 
    {
        raw_temperature = get_raw_temperature();
        temp_celsius = convert_to_celsius(raw_temperature);

        lcd_set_cursor(0, 0);
        lcd_print("Temp: ");
        dtostrf(temp_celsius, 4, 2, temp_string);
        lcd_print(temp_string);
        lcd_send_byte(LCD_DEGREE_SYMBOL, LCD_DATA);
        lcd_print("C   ");

        if (temp_celsius <= FAN_TEMP_SILENT) 
        {
            fan_set_speed(FAN_SPEED_SILENT);
        }

        else if (temp_celsius > FAN_TEMP_SILENT && temp_celsius <= FAN_TEMP_NORMAL) 
        {
            fan_set_speed(FAN_SPEED_NORMAL);
        }

        else if (temp_celsius > FAN_TEMP_NORMAL && temp_celsius <= FAN_SPEED_PERFORMANCE) 
        {
            fan_set_speed(FAN_SPEED_PERFORMANCE);
        }

        else if (temp_celsius >= FAN_TEMP_CRITICAL)
        {
            fan_set_speed(FAN_SPEED_MAX);
        }
    }
}