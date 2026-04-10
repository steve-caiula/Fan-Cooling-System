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
#define FAN_RPM_PER_DUTY       18     // 1800 RPM max / 100
#define FAN_SPEED_TOLERANCE    50     // Minimum acceptable RPM percentage


// TEMPERATURE THRESHOLDS
// Compressed range (22-24°C) calibrated for laboratory testing with a soldering iron.
// Widen the range for real-world use cases.
#define FAN_TEMP_SILENT        22     // Below 22°C
#define FAN_TEMP_NORMAL        23     // 22.01°C - 23°C
#define FAN_TEMP_PERFORMANCE   24     // 23.01°C - 24°C (above 24°C -> safe mode)


// FAN SPEED
#define FAN_SPEED_SILENT       30     // Mode SILENT (minimum guaranteed speed)
#define FAN_SPEED_NORMAL       50     // Mode NORMAL
#define FAN_SPEED_PERFORMANCE  75     // Mode PERFORMANCE
#define FAN_SPEED_MAX          100    // Mode MAX or safe mode (temp. sensor error)


#endif