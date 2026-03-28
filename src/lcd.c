#include <avr/io.h>
#include <avr/delay.h>
#include "lcd.h"
#include "system_timer.h"




volatile uint8_t twi_busy;         // 1 = transmission in progress, 0 = idle
volatile uint8_t twi_error;        // 0 = NO ERROR, 1 = ERROR
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

      case TWI_MT_SLA_NACK:
      case TWI_MT_DATA_NACK:
         twi_error = 1;
         TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
         twi_busy = 0;
      break;

      default:
         twi_error = 1;
         TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
         twi_busy = 0;
      break;
   }
}




void twi_init (void)
{
   TWBR = TWBR_VALUE;                  // Set SCL frequency to 100kHz
   TWSR = (0 << TWPS1) | (0 << TWPS0); // Set prescaler to 1
   TWCR = (1 << TWEN) | (1 << TWIE);   // Enable TWI and interrupts
}




static uint8_t twi_send (uint8_t data)
{
   uint32_t start = get_millis();

   while (twi_busy == 1)
   {
      if (get_millis() - start >= 10)
      {
         twi_error = 1;
         return 1; // Errore
      }
   }
   
   twi_buffer[0] = data;  // Load byte into buffer
   twi_buffer_len = 1;    // Always 1 byte
   twi_buffer_index = 0;  // Reset buffer index
   twi_error = 0;         // Reset error flag
   twi_busy = 1;          // Set bus as busy
   TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE); // Trigger START condition

   return 0; // Success
}




static uint8_t lcd_send_nibble (uint8_t nibble, uint8_t rs_mode)
{
   uint8_t data;

   data = nibble << 4 | (rs_mode) | (LCD_EN) | (LCD_BL);
   if (twi_send (data) != 0) return 1;

   data = nibble << 4 | (rs_mode) | (LCD_BL);
   if (twi_send (data) != 0) return 1;

   return 0;
}




static uint8_t lcd_send_byte (uint8_t data, uint8_t rs_mode)
{
   if (lcd_send_nibble (data >> 4, rs_mode) != 0) return 1;
   if (lcd_send_nibble (data, rs_mode) != 0) return 1;

   return 0;
}




uint8_t lcd_init (void)
{
   /* Step 1: Power-On Delay.
      Wait for the power supply (VCC) to stabilize. The datasheet requires 
      at least 15ms after VCC rises to 4.5V. A 20ms delay provides a robust 
      safety margin for the hardware to settle.
   */
   _delay_ms (20);

   /* Step 2: Signal Initialization.
      Force both RS and E pins LOW. This guarantees that the controller 
      interprets the upcoming wake-up nibbles as commands (RS=0) and 
      prevents any accidental data latching before the sequence begins.
   */

   if (lcd_send_nibble (LCD_WAKE_UP, LCD_COMMAND) != 0) return 1;
   _delay_ms (5); // Required wait > 4.1ms

   if (lcd_send_nibble (LCD_WAKE_UP, LCD_COMMAND) != 0) return 1;
   _delay_us (100); // Required wait > 100us

   if (lcd_send_nibble (LCD_WAKE_UP, LCD_COMMAND) != 0) return 1;

   /* Step 4: 4-Bit Mode Activation.
      Send the command (0x02) to switch the data interface from 8-bit 
      to 4-bit mode. From this exact point onward, the controller expects 
      all instructions and data to be sent as two consecutive nibbles.
   */
   if (lcd_send_nibble (LCD_MODE_4_BIT, LCD_COMMAND) != 0) return 1;
   _delay_us (50);

   /* Step 5: Final Display Configuration.
      Now using the full 8-bit split transmission (lcd_send_byte), we configure 
      the display parameters: 2 lines with a 5x8 font, turn the display ON, 
      clear the memory, and set the cursor to automatically move right.
   */
   if (lcd_send_byte (LCD_FUNCTION_SET, LCD_COMMAND) != 0) return 1;

   if (lcd_send_byte (LCD_DISPLAY_ON, LCD_COMMAND) != 0) return 1;

   /* The Clear Display command takes significantly longer to execute 
      (~1.52ms) than standard commands. A 2ms delay ensures it completes 
      before the Entry Mode command is sent.
   */
   if (lcd_send_byte (LCD_CLEAR_DISPLAY, LCD_COMMAND) != 0) return 1;
   _delay_ms (2);

   if (lcd_send_byte (LCD_ENTRY_MODE, LCD_COMMAND) != 0) return 1;

   return 0;
}




uint8_t lcd_set_cursor (uint8_t row, uint8_t column)
{
   uint8_t destination_address;
   
   /* Step 1: Calculate the DDRAM Address.
      According to the datasheet, the internal Display Data RAM (DDRAM) 
      address for the first line starts at 0x00, 
      while the second line starts at 0x40. 
      To instruct the controller to move the cursor, the "Set DDRAM Address" 
      command requires the most significant bit (DB7) to be set to 1.
      Therefore, the base command to set the cursor on Line 1 is 0x80, 
      and for Line 2 is 0xC0, for Line 3 is 0x94 and for Line 4 is 0xD4. 
      The final destination address is calculated by adding 
      the horizontal column offset to this base value (e.g., 0x80 + column).
   */
   switch (row)
   {
      case 0:
      destination_address = LCD_LINE_1 + column;
      break;

      case 1:
      destination_address = LCD_LINE_2 + column;
      break;

      case 2:
      destination_address = LCD_LINE_3 + column;
      break;

      case 3:
      destination_address = LCD_LINE_4 + column;
      break;

      default:
      return 1;
   }

   /* Step 2: Send the Position Command.
      The combined base address and horizontal column offset are sent 
      as a single instruction byte in LCD_COMMAND mode (RS = 0) to place 
      the cursor at the exact memory location.
   */
   if (lcd_send_byte(destination_address, LCD_COMMAND) != 0) return 1;

   return 0;
}




uint8_t lcd_print (const char *string)
{
   uint8_t i = 0;

   /* Step 1: String Traversal.
      In C, strings are arrays of characters ending with a null 
      terminator ('\0'). This loop iterates through the memory 
      until the terminator is reached, ensuring compatibility 
      with strings of any length (up to the display's memory limit).
   */
   while (string[i] != '\0') 
   {
      /* Step 2: Data Transmission.
         Each character is sent as a DATA byte (RS = 1). 
         The TC1602A-01T controller (Page 15) uses a built-in 
         Character Generator ROM (CGROM) to map ASCII-like 
         values to 5x8 pixel matrices.
      */
      if (lcd_send_byte(string[i], LCD_DATA) != 0) return 1;

      /* Step 3: Auto-Increment Logic.
         The controller is configured (via LCD_ENTRY_MODE) to 
         automatically increment the DDRAM address after each 
         write operation, moving the cursor to the next 
         position without manual intervention.
      */
      i++;
   }

   return 0;
}