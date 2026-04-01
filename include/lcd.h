#ifndef LCD_H
#define LCD_H


#include <stdint.h>
#include "board.h"


// TWI STATES
#define TWI_START         0x08   // START condition transmitted
#define TWI_MT_SLA_ACK    0x18   // Address sent, ACK received
#define TWI_MT_SLA_NACK   0x20   // Address sent, NACK received
#define TWI_MT_DATA_ACK   0x28   // Data sent, ACK received
#define TWI_MT_DATA_NACK  0x30   // Data sent, NACK received


// TWI CONFIGURATION
#define TWI_FREQUENCY  100000UL                                                  // 100kHz
#define TWI_PRESCALER  1                                                         // Prescaler
#define TWBR_VALUE     ((CPU_FREQ / TWI_FREQUENCY - 16) / (2 * TWI_PRESCALER))   // TWBR value


// PCF8574 ADDRESS
#define PCF8574_ADDR  0x27


// LCD CONTROLS
#define LCD_RS  0x01   // P0
#define LCD_RW  0x02   // P1, always LOW (write-only mode)
#define LCD_EN  0x04   // P2
#define LCD_BL  0x08   // P3 (backlight)


// RS
#define LCD_COMMAND  0
#define LCD_DATA     1


// LCD COMMANDS
#define LCD_CLEAR_DISPLAY  0x01
#define LCD_RETURN_HOME    0x02
#define LCD_MODE_4_BIT     0x02
#define LCD_WAKE_UP        0x03
#define LCD_ENTRY_MODE     0x06
#define LCD_DISPLAY_ON     0x0C
#define LCD_FUNCTION_SET   0x28


// SET CURSOR
#define LCD_LINE_1    0x80
#define LCD_LINE_2    0xC0
#define LCD_LINE_3    0x94
#define LCD_LINE_4    0xD4
#define LCD_COLUMN_1  0x00


// SPECIAL CHARACTERS
#define LCD_DEGREE_SYMBOL  0xDF


// PUBLIC FUNCTIONS
/*
   Initializes the TWI peripheral and the LCD display.
   Must be called once at startup before any other LCD function.
   Returns 0 on success, 1 on communication error.
*/
uint8_t lcd_init(void);


/*
   Moves the cursor to the specified row (0-3) and column (0-19).
   Returns 0 on success, 1 on invalid row or communication error.
*/
uint8_t lcd_set_cursor(uint8_t row, uint8_t column);


/*
   Sends a full byte to the LCD as two consecutive nibbles (high then low).
   Returns 0 on success, 1 on communication error.
*/
uint8_t lcd_send_byte(uint8_t data, uint8_t rs_mode);


/*
   Prints a null-terminated string at the current cursor position.
   Returns 0 on success, 1 on communication error.
*/
uint8_t lcd_print(const char *string);


#endif