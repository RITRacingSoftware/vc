#ifndef HEART_BEAT_LED_H
#define HEART_BEAT_LED_H

/**
 * Blink an LED based off of the status of the system.
 * 
 * CAN tx or rx error = fast steady 4Hz toggling
 * Faulted - slow 1Hz steady toggling
 * Nominal (no faults or CAN errors) - quick blink every 2 seconds
 */

/**
 * Set up initial system status.
 */
void HeartBeatLed_init(void);

/**
 * Determine system status and set the blinking pattern accordingly.
 */
void HeartBeatLed_100Hz(void);

#endif // HEART_BEAT_LED_H