#ifndef BUZZER_H
#define BUZZER_H

#include "pin_definitions.h"

#define BUZZER_FREQ_HZ        2000   // Tone frequency
#define BUZZER_BLINK_ON_MS    200    // Time sound on
#define BUZZER_BLINK_OFF_MS   200    // Time sound off

/*
  CTC Mode frequency formula: OCR2A = (F_CPU / (2 * prescaler * target_freq)) - 1
  Prescaler 64 is chosen because it keeps OCR2A within the 8-bit range (0-255).
  At 2000Hz: OCR2A = (16000000 / (2 * 64 * 2000)) - 1 = 61
*/
#define BUZZER_OCR2A_VALUE ((CPU_FREQ / (2UL * 64UL * BUZZER_FREQ_HZ)) - 1)

void buzzer_init (void);
void buzzer_alarm_sensor (void);
void buzzer_alarm_lcd (void);
 
#endif