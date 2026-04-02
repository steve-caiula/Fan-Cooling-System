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
    int16_t raw_temperature;
    float temp_celsius;
    char temp_string[10];
    uint16_t rpm;
    fan_status_t fan_status;
    uint8_t duty_cycle;
    uint8_t lcd_error;
    uint8_t buzzer_status;
    uint8_t led_error;

    system_timer_init();
    fan_init();
    buzzer_init();
    led_init();

    lcd_error = lcd_init() == 1 ? 1 : 0;

    sei();

    while (1) 
    {   
        raw_temperature = get_raw_temperature();
        temp_celsius = convert_to_celsius(raw_temperature);

        if (raw_temperature == TEMP_SENSOR_ERROR) 
        {
            fan_set_speed(FAN_SPEED_MAX);
            
            duty_cycle = FAN_SPEED_MAX;
            rpm = fan_get_rpm();
            fan_status = fan_get_status(rpm, duty_cycle);
            
            lcd_error = lcd_set_cursor(0, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("TEMP. SENSOR ERROR  ")  == 1  ? 1 : 0;
            lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("FAN SPEED MAX       ") == 1 ? 1 : 0;
            lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("FAN OK              ") == 1 ? 1 : 0;

            led_error = led_off(RED) == 1 ? 1 : 0;
            led_error = led_on(YELLOW) == 1 ? 1 : 0;
            led_error = led_off(GREEN) == 1 ? 1 : 0;
        }

        else 
        {
            lcd_error = lcd_set_cursor(0, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("Temp: ")  == 1  ? 1 : 0;
            dtostrf(temp_celsius, 4, 2, temp_string);
            lcd_error = lcd_print(temp_string) == 1  ? 1 : 0;
            lcd_error = lcd_send_byte(LCD_DEGREE_SYMBOL, LCD_DATA)  == 1 ? 1 : 0;
            lcd_error = lcd_print("C       ") == 1 ? 1 : 0;

            if (temp_celsius <= FAN_TEMP_SILENT) 
            {
                fan_set_speed(FAN_SPEED_SILENT);
            
                duty_cycle = FAN_SPEED_SILENT;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);
            
                lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN SPEED SILENT    ") == 1 ? 1 : 0;
                lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN OK              ") == 1 ? 1 : 0;

                led_error = led_off(RED) == 1 ? 1 : 0;
            }

            else if (temp_celsius > FAN_TEMP_SILENT && temp_celsius <= FAN_TEMP_NORMAL) 
            {
                fan_set_speed(FAN_SPEED_NORMAL);
            
                duty_cycle = FAN_SPEED_NORMAL;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);

                lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN SPEED NORMAL    ") == 1 ? 1 : 0;
                lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN OK              ") == 1 ? 1 : 0;

                led_error = led_off(RED) == 1 ? 1 : 0;
            }

            else if (temp_celsius > FAN_TEMP_NORMAL && temp_celsius <= FAN_SPEED_PERFORMANCE) 
            {
                fan_set_speed(FAN_SPEED_PERFORMANCE);
            
                duty_cycle = FAN_SPEED_PERFORMANCE;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);

                lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN SPEED PERFORM.  ") == 1 ? 1 : 0;
                lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN OK              ") == 1 ? 1 : 0;

                led_error = led_off(RED) == 1 ? 1 : 0;
            }

            else if (temp_celsius >= FAN_TEMP_CRITICAL)
            {
                fan_set_speed(FAN_SPEED_MAX);
            
                duty_cycle = FAN_SPEED_MAX;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);

                lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN SPEED MAX       ") == 1 ? 1 : 0;
                lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
                lcd_error = lcd_print("FAN OK              ") == 1 ? 1 : 0;

                led_error = led_off(RED) == 1 ? 1 : 0;
            }
        }

        if (fan_status == FAN_ERROR_STALL)
        {
            led_error = led_on(RED) == 1 ? 1 : 0;
            led_error = led_off(GREEN) == 1 ? 1 : 0;
            buzzer_alarm_critical();
        
            lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("FAN ERROR STALL     ") == 1 ? 1 : 0;
            lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("COOLING FAILURE!    ") == 1 ? 1 : 0;
        }

        else if (fan_status == FAN_ERROR_UNDERSPEED) 
        {
            led_error = led_on(RED) == 1 ? 1 : 0;
            led_error = led_off(GREEN) == 1 ? 1 : 0;
            buzzer_alarm_critical();

            lcd_error = lcd_set_cursor(1, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("FAN ERROR UNDERSPEED") == 1 ? 1 : 0;
            lcd_error = lcd_set_cursor(2, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("CHECK THE FAN!      ") == 1 ? 1 : 0;
        }

        if (lcd_error == 1)
        {
            if (raw_temperature != TEMP_SENSOR_ERROR && fan_status == FAN_OK) 
            {
                buzzer_alarm_warning();
            }
        
            led_error = led_on(BLUE) == 1 ? 1 : 0;
            led_error = led_off(GREEN) ? 1 : 0;
        }

        else
        {
            led_error = led_off(BLUE) == 1 ? 1 : 0;
        }

        if (led_error == 1)
        {
            if (raw_temperature != TEMP_SENSOR_ERROR && fan_status == FAN_OK) 
            {
                buzzer_alarm_warning();
            }
        
            lcd_error = lcd_set_cursor(3, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("LEDS ERROR          ") == 1 ? 1 : 0;
        }

        else 
        {
            lcd_error = lcd_set_cursor(3, 0) == 1 ? 1 : 0;
            lcd_error = lcd_print("LEDS OK             ") == 1 ? 1 : 0;
        }

        if (fan_status == FAN_OK && raw_temperature != TEMP_SENSOR_ERROR && lcd_error == 0 && led_error == 0)
        {
            led_on(GREEN);
            led_off(BLUE);
            led_off(YELLOW);
            led_off(RED);
        }
    }
}