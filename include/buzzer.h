#ifndef BUZZER_H
#define BUZZER_H

#include "board.h"
#include "config.h"

/*
  CTC Mode frequency formula: OCR2A = (F_CPU / (2 * prescaler * target_freq)) - 1
  Prescaler 64 is chosen because it keeps OCR2A within the 8-bit range (0-255).
  At 2000Hz: OCR2A = (16000000 / (2 * 64 * 2000)) - 1 = 61
*/
#define BUZZER_OCR2A_VALUE  ((CPU_FREQ / (2UL * 64UL * BUZZER_FREQ_HZ)) - 1)

// PUBLIC FUNCTIONS
void buzzer_init(void);
void buzzer_stop(void);


/*
  Activates a continuous tone on sensor fault or fan fault detection.
  The tone is started only once, subsequent calls are no-ops
  until the system is reset.
*/
void buzzer_alarm_critical(void);


/*
  Generates an intermittent tone for LCD fault signaling.
  Alternates between on and off states based on BUZZER_BLINK_ON_MS
  and BUZZER_BLINK_OFF_MS intervals. Non-blocking, must be called
  repeatedly from the main loop.
*/
void buzzer_alarm_warning(void);
 
#endif