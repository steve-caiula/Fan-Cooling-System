/*
   Fan driver for Arctic P12 PRO 4-pin PWM fan.
   Controls fan speed via Timer 1 PWM on OC1A (D9) at 25kHz.
   Measures RPM via INT0 tachometer input on D2.
   Detects stall and underspeed conditions via fan_get_status().
*/


#include <avr/io.h>
#include "fan.h"
#include "system_timer.h"


volatile uint16_t pulse_count = 0;   // Tachometer pulse counter, incremented by INT0 ISR


ISR(INT0_vect)
{
    pulse_count++;   // Increment tachometer pulse counter on falling edge
}


void fan_init(void)
{
    DDRB |= (1 << FAN_PWM);   // Set OC1A as output for PWM signal

    TCCR1A = (1 << COM1A1) | (1 << WGM11);                // Non-inverting Fast PWM, WGM bits 1:0
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);   // Fast PWM mode 14, prescaler 1
    ICR1 = FAN_ICR1_VALUE;                                // Set TOP for 25kHz frequency

    DDRD &= ~(1 << FAN_TACHO);   // Set tachometer pin as input

    EICRA = (1 << ISC01);   // Trigger INT0 on falling edge
    EIMSK = (1 << INT0);    // Enable INT0 interrupt
}


void fan_set_speed(uint8_t duty_cycle)
{
    OCR1A = (ICR1 * duty_cycle) / 100;   // Set duty cycle: OCR1A / ICR1 = duty cycle percentage
}


uint16_t fan_get_rpm(void)
{
    static uint32_t last_sample_time;   // Timestamp of last RPM calculation
    static uint16_t last_pulse_count;   // Pulse count at last sample
    static uint16_t rpm;                // Last calculated RPM value
    uint16_t elapsed_pulse;             // Pulses counted in current interval
    uint32_t current_time = get_millis ();

    if ((current_time - last_sample_time) >= 1000)   // Sample every 1 second
    {
        cli();
        elapsed_pulse = pulse_count - last_pulse_count;   // Count pulses in interval
        last_pulse_count = pulse_count;                   // Save current count
        sei();

        rpm = (elapsed_pulse / 2) * 60;   // 2 pulses per revolution, convert to RPM
        last_sample_time = current_time;  // Reset timestamp
    }

    return rpm;
}


fan_status_t fan_get_status(uint16_t rpm, uint8_t duty_cycle)
{
    if (rpm == 0 && duty_cycle > 0)   // Fan not spinning despite active PWM
    {
        return FAN_ERROR_STALL;
    }

    else if (rpm < (duty_cycle * FAN_RPM_PER_DUTY * FAN_SPEED_TOLERANCE / 100))   // RPM below expected threshold
    {
        return FAN_ERROR_UNDERSPEED;
    }

    else 
    {
        return FAN_OK;
    }
}