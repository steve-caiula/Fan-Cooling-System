#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "lcd.h"

volatile uint8_t twi_data;         // Data to transmit
volatile uint8_t twi_busy;         // 1 = transmission in progress, 0 = idle
volatile uint8_t twi_buffer[16];   // Data buffer to transmit
volatile uint8_t twi_buffer_len;   // Number of bytes to transmit
volatile uint8_t twi_buffer_index; // Current buffer index

ISR (TWI_vect)
{
    switch (TWSR & 0xF8) // Status bit mask
    {
        case TWI_START: // START sent
            TWDR = PCF8574_ADDR << 1;
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
            break;

        case TWI_MT_SLA_ACK: // Address sent, ACK received
            TWDR = twi_buffer[twi_buffer_index];
            twi_buffer_index++;
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
            break;

        case TWI_MT_DATA_ACK: // Data sent, ACK received
            if (twi_buffer_index < twi_buffer_len) 
            {
                TWDR = twi_buffer[twi_buffer_index];
                twi_buffer_index++;
                TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
            }

            else 
            {
                TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
                twi_busy = 0;
            }
            break;
    }
}

void twi_init (void)
{
    TWBR = TWBR_VALUE;                  // Set SCL frequency to 100kHz
    TWSR = (0 << TWPS1) | (0 << TWPS0); // Set prescaler to 1
    TWCR = (1 << TWEN) | (1 << TWIE);   // Enable TWI and interrupts
}

void twi_send (uint8_t *data, uint8_t length)
{
    while (twi_busy == 1); // Wait until bus is free

    for (uint8_t i = 0; i < length; i++) // Copy *data on twi_buffer
    {
        twi_buffer[i] = data[i];
    }

    twi_buffer_len = length; // Set buffer length
    twi_buffer_index = 0;    // Set twi_buffer_index to 0 to start transmission from 1st char

    twi_busy = 1; // Set bus as busy

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE); // Trigger START condition
}