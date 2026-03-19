#ifndef LCD_H
#define LCD_H

// TWI STATES
#define TWI_START           0x08
#define TWI_REP_START       0x10
#define TWI_MT_SLA_ACK      0x18  // Address sent, ACK received
#define TWI_MT_SLA_NACK     0x20  // Address sent, NACK received
#define TWI_MT_DATA_ACK     0x28  // Data sent, ACK received
#define TWI_MT_DATA_NACK    0x30  // Data sent, NACK received

// TWI CONFIGURATION
#define F_CPU         16000000UL                                         // 16MHz crystal
#define TWI_FREQUENCY 100000UL                                           // 100kHz
#define TWI_PRESCALER 1                                                  // Prescaler
#define TWBR_VALUE    (F_CPU / TWI_FREQUENCY - 16) / (2 * TWI_PRESCALER) // TWBR value

// PCF8574 ADDRESS
#define PCF8574_ADDR        0x27

// LCD CONTROLS
#define LCD_RS              0x01  // P0
#define LCD_RW              0x02  // P1
#define LCD_EN              0x04  // P2
#define LCD_BL              0x08  // P3 (backlight)

#endif