#ifndef PERIODIC_H
#define PERIODIC_H

/**
 * All top-level procedures. Can be compiled for SIL or using freertos for stm32.
 */

/**
 * Initialize application modules.
 */
void VC_init(void);

/**
 * The sequence of events that must happen 100 times a second.
 */
void VC_100Hz(void);

/**
 * The sequence of events that must happen 1000 times a second.
 */
void VC_1kHz(void);

#endif // PERIODIC_H