/*
   System configuration parameters.
   Modify these values to adapt the system to your specific use case.
   Default values are calibrated for laboratory testing with a soldering iron.
*/

#ifndef CONFIG_H
#define CONFIG_H


// BUZZER
#define BUZZER_FREQ_HZ         2000   // Tone frequency
#define BUZZER_BLINK_ON_MS     200    // Time sound on
#define BUZZER_BLINK_OFF_MS    200    // Time sound off


// FAN
#define FAN_TEMP_SILENT        25     // Below 25°C
#define FAN_TEMP_NORMAL        30     // 25°C - 30°C
#define FAN_TEMP_PERFORMANCE   35     // 30°C - 35°C
#define FAN_TEMP_CRITICAL      36     // Above 35°C -> safe mode

#define FAN_SPEED_SILENT       30     // Minimum guaranteed speed
#define FAN_SPEED_NORMAL       50     // 25°C - 30°C
#define FAN_SPEED_PERFORMANCE  75     // 30°C - 35°C
#define FAN_SPEED_MAX          100    // Safe mode


#endif