/*
   Main control loop for a temperature-based fan cooling system.
   Reads temperature from a DS18B20 sensor and adjusts fan speed
   accordingly via PWM. Monitors fan RPM, LCD and LED status,
   and triggers alarms via buzzer and LEDs on fault detection.

   System parameters (temperature thresholds and fan speeds) are
   defined in config.h. Default values are calibrated for laboratory
   testing with a soldering iron.
   
   Hardware: Arduino UNO R3 (ATmega328P)
   Peripherals: DS18B20, Arctic P12 PRO, NHD-0420H1Z with PCF8574 I2C module, Murata buzzer
*/


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
    int16_t raw_temperature;            // Raw temperature value from temperature sensor
    float temp_celsius;                 // Converted temperature in Celsius
    char temp_string[12];               // Buffer for temperature string conversion
    uint16_t rpm;                       // Fan speed in RPM
    fan_status_t fan_status = FAN_OK;   // Fan operational status, initialized to OK
    uint8_t duty_cycle;                 // Current fan duty cycle (percentage)
    uint8_t lcd_init_error = 0;         // LCD initialization error flag (1 = error)
    uint8_t lcd_error;                  // LCD operation error flag for current cycle
    uint8_t led_error = 0;              // LED operation error flag

    // Peripherals initialization
    system_timer_init();   // Must be called first: provides get_millis() to all drivers
    fan_init();            // Configures Timer 1 (PWM) and INT0 (TACHO)
    buzzer_init();         // Configures buzzer pin as output
    led_init();            // Configures LED pins as output

    sei();   // Enable global interrupts

    lcd_init_error = lcd_init();   // Initializes TWI and LCD, stores result for main loop

    while (get_millis() < 1000);   // Wait for fan RPM first sample to be ready to avoid FAN_ERROR_STALL in first loop

    while (1) 
    {   
        lcd_error = lcd_init_error;   // Reset LCD error flag, preserving init error if present
        
        raw_temperature = get_raw_temperature();              // Read raw temperature from temperature sensor
        temp_celsius = convert_to_celsius(raw_temperature);   // Convert to Celsius

        /*
           SAFE MODE: Sensor fault detected.
           Triggered when the temperature sensor is disconnected or malfunctioning.
           Fan is forced to maximum speed to guarantee cooling.
           LCD: displays sensor error and fan status.
           LED: YELLOW.
           Buzzer: continuous alarm.
        */
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

        /*
           NORMAL OPERATION: Fan speed adjusted based on current temperature.
           - Below 25C:  SILENT mode      (30% duty cycle)
           - 25C to 30C: NORMAL mode      (50% duty cycle)
           - 30C to 35C: PERFORMANCE mode (75% duty cycle)
           - Above 35C:  MAX mode         (100% duty cycle)
           LCD: displays current temperature and fan speed mode.
           LED: RED off. Final state determined by fault checks below.
        */
        else 
        {
            lcd_error |= lcd_set_cursor(0, 0);
            lcd_error |= lcd_print("Temp: ");
            dtostrf(temp_celsius, 4, 2, temp_string);
            lcd_error |= lcd_print(temp_string);
            lcd_error |= lcd_send_byte(LCD_DEGREE_SYMBOL, LCD_DATA);
            lcd_error |= lcd_print("C       ") ;

            led_error |= led_off(YELLOW);

            if (temp_celsius <= FAN_TEMP_SILENT && fan_status == FAN_OK) 
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

            else if (temp_celsius > FAN_TEMP_SILENT && temp_celsius <= FAN_TEMP_NORMAL && fan_status == FAN_OK) 
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

            else if (temp_celsius > FAN_TEMP_NORMAL && temp_celsius <= FAN_TEMP_PERFORMANCE && fan_status == FAN_OK) 
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

            else if (temp_celsius >= FAN_TEMP_CRITICAL && fan_status == FAN_OK)
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

        /*
           FAN STALL: Critical fault. Fan is not spinning despite active PWM signal.
           Cooling cannot be guaranteed. System requires immediate intervention.
           LCD: displays stall error and cooling failure warning.
           LED: RED on.
           Buzzer: continuous alarm.
        */
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

        /*
           FAN UNDERSPEED: Critical fault. Fan is spinning below the required speed.
           Cooling is active but may be insufficient for current temperature.
           System requires inspection.
           LCD: displays underspeed error and inspection warning.
           LED: RED on.
           Buzzer: continuous alarm.
        */
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

        /*
           LCD FAULT: Communication error with the LCD display.
           Triggered when the LCD is disconnected or malfunctioning.
           LED: BLUE on.
           Buzzer: intermittent alarm, only if no critical faults are active
           (sensor error and fan faults have priority).
        */
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

        /*
           LED FAULT: Error detected in LED operation.
           In practice this fault should never occur, as LED functions are
           always called with valid led_color_t enum values.
           Included as a defensive check against future code changes.
           LCD: displays LED error on row 4.
           Buzzer: intermittent alarm, only if no critical faults are active
           (sensor error and fan faults have priority).
        */
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

        /*
           ALL PERIPHERALS OK: No faults detected. GREEN LED on.
        */
        if (fan_status == FAN_OK && raw_temperature != TEMP_SENSOR_ERROR && lcd_error == 0 && led_error == 0)
        {
            led_error |= led_on(GREEN);
            led_error |= led_off(BLUE);
            led_error |= led_off(YELLOW);
            led_error |= led_off(RED);
        }
    }
}