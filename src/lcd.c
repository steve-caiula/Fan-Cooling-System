#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "pin_definitions.h"

volatile uint8_t twi_status;       // Updated by ISR
volatile uint8_t twi_data;         // Data to transmit
volatile uint8_t twi_busy;         // 1 = transmission in progress, 0 = idle
volatile uint8_t twi_buffer[16];   // data buffer to transmit
volatile uint8_t twi_buffer_len;   // number of bytes to transmit
volatile uint8_t twi_buffer_index; // current buffer index

ISR(TWI_vect)
{
    switch (TWSR & 0xF8) // Status bit mask
    {
        case TWI_START: // START sent
            // Send PCF8574 address
            break;

        case TWI_MT_SLA_ACK: // Address sent, ACK received
            // Send data
            break;

        case TWI_MT_DATA_ACK: // Data sent, ACK received
            // Send STOP or next data
            break;
    }
}