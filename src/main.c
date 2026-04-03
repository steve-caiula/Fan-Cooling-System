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
    char temp_string[12];
    uint16_t rpm;
    fan_status_t fan_status = FAN_OK;
    uint8_t duty_cycle;
    uint8_t lcd_error;
    uint8_t lcd_init_error = 0;
    uint8_t led_error = 0;

    system_timer_init();
    fan_init();
    buzzer_init();
    led_init();

    lcd_init_error = lcd_init();

    sei();

    while (get_millis() < 1000); 

    while (1) 
    {   
        lcd_error = lcd_init_error;
        
        raw_temperature = get_raw_temperature();
        temp_celsius = convert_to_celsius(raw_temperature);

        if (raw_temperature == TEMP_SENSOR_ERROR) 
        {
            fan_set_speed(FAN_SPEED_MAX);
            
            duty_cycle = FAN_SPEED_MAX;
            rpm = fan_get_rpm();
            fan_status = fan_get_status(rpm, duty_cycle);
            
            lcd_error |= lcd_set_cursor(0, 0);
            lcd_error |= lcd_print("TEMP. SENSOR ERROR  ");
            lcd_error |= lcd_set_cursor(1, 0);
            lcd_error |= lcd_print("FAN SPEED MAX       ");
            lcd_error |= lcd_set_cursor(2, 0);
            lcd_error |= lcd_print("FAN OK              ");

            led_error |= led_off(RED);
            led_error |= led_on(YELLOW);
            led_error |= led_off(GREEN);
        }

        else 
        {
            lcd_error |= lcd_set_cursor(0, 0);
            lcd_error |= lcd_print("Temp: ");
            dtostrf(temp_celsius, 4, 2, temp_string);
            lcd_error |= lcd_print(temp_string);
            lcd_error |= lcd_send_byte(LCD_DEGREE_SYMBOL, LCD_DATA);
            lcd_error |= lcd_print("C       ") ;

            if (temp_celsius <= FAN_TEMP_SILENT) 
            {
                fan_set_speed(FAN_SPEED_SILENT);
            
                duty_cycle = FAN_SPEED_SILENT;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);
            
                lcd_error |= lcd_set_cursor(1, 0);
                lcd_error |= lcd_print("FAN SPEED SILENT    ");
                lcd_error |= lcd_set_cursor(2, 0);
                lcd_error |= lcd_print("FAN OK              ");

                led_error |= led_off(RED);
            }

            else if (temp_celsius > FAN_TEMP_SILENT && temp_celsius <= FAN_TEMP_NORMAL) 
            {
                fan_set_speed(FAN_SPEED_NORMAL);
            
                duty_cycle = FAN_SPEED_NORMAL;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);

                lcd_error |= lcd_set_cursor(1, 0);
                lcd_error |= lcd_print("FAN SPEED NORMAL    ");
                lcd_error |= lcd_set_cursor(2, 0);
                lcd_error |= lcd_print("FAN OK              ");

                led_error |= led_off(RED);
            }

            else if (temp_celsius > FAN_TEMP_NORMAL && temp_celsius <= FAN_TEMP_PERFORMANCE) 
            {
                fan_set_speed(FAN_SPEED_PERFORMANCE);
            
                duty_cycle = FAN_SPEED_PERFORMANCE;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);

                lcd_error |= lcd_set_cursor(1, 0);
                lcd_error |= lcd_print("FAN SPEED PERFORM.  ");
                lcd_error |= lcd_set_cursor(2, 0);
                lcd_error |= lcd_print("FAN OK              ");

                led_error |= led_off(RED);
            }

            else if (temp_celsius >= FAN_TEMP_CRITICAL)
            {
                fan_set_speed(FAN_SPEED_MAX);
            
                duty_cycle = FAN_SPEED_MAX;
                rpm = fan_get_rpm();
                fan_status = fan_get_status(rpm, duty_cycle);

                lcd_error |= lcd_set_cursor(1, 0);
                lcd_error |= lcd_print("FAN SPEED MAX       ");
                lcd_error |= lcd_set_cursor(2, 0);
                lcd_error |= lcd_print("FAN OK              ");

                led_error |= led_off(RED);
            }
        }

        if (fan_status == FAN_ERROR_STALL)
        {
            led_error |= led_on(RED);
            led_error |= led_off(GREEN);
            buzzer_alarm_critical();
        
            lcd_error |= lcd_set_cursor(1, 0);
            lcd_error |= lcd_print("FAN ERROR STALL     ");
            lcd_error |= lcd_set_cursor(2, 0);
            lcd_error |= lcd_print("COOLING FAILURE!    ");
        }

        else if (fan_status == FAN_ERROR_UNDERSPEED) 
        {
            led_error |= led_on(RED);
            led_error |= led_off(GREEN);
            buzzer_alarm_critical();

            lcd_error |= lcd_set_cursor(1, 0);
            lcd_error |= lcd_print("FAN ERROR UNDERSPEED");
            lcd_error |= lcd_set_cursor(2, 0);
            lcd_error |= lcd_print("CHECK THE FAN!      ");
        }

        if (lcd_error == 1)
        {
            if (raw_temperature != TEMP_SENSOR_ERROR && fan_status == FAN_OK) 
            {
                buzzer_alarm_warning();
            }
        
            led_error |= led_on(BLUE);
            led_error |= led_off(GREEN);
        }

        else
        {
            led_error |= led_off(BLUE);
        }

        if (led_error == 1)
        {
            if (raw_temperature != TEMP_SENSOR_ERROR && fan_status == FAN_OK) 
            {
                buzzer_alarm_warning();
            }
        
            lcd_error |= lcd_set_cursor(3, 0);
            lcd_error |= lcd_print("LEDS ERROR          ");
        }

        else 
        {
            lcd_error |= lcd_set_cursor(3, 0);
            lcd_error |= lcd_print("LEDS OK             ");
        }

        if (fan_status == FAN_OK && raw_temperature != TEMP_SENSOR_ERROR && lcd_error == 0 && led_error == 0)
        {
            led_error |= led_on(GREEN);
            led_error |= led_off(BLUE);
            led_error |= led_off(YELLOW);
            led_error |= led_off(RED);
        }
    }
}