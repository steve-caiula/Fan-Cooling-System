#ifndef CONFIG_H
#define CONFIG_H

#define BUZZER_FREQ_HZ        2000   // Tone frequency
#define BUZZER_BLINK_ON_MS    200    // Time sound on
#define BUZZER_BLINK_OFF_MS   200    // Time sound off

#define FAN_TEMP_SILENT        25    // Below 25°C
#define FAN_TEMP_NORMAL        30    // 25°C - 30°C
#define FAN_TEMP_PERFORMANCE   35    // 30°C - 35°C
#define FAN_TEMP_CRITICAL      40    // Above 40°C → safe mode

#define FAN_SPEED_SILENT       30    // Minimum guaranteed speed
#define FAN_SPEED_NORMAL       50
#define FAN_SPEED_PERFORMANCE  75
#define FAN_SPEED_MAX          100   // Safe mode

#endif