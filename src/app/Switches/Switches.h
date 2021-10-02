#ifndef SWITCHES_H
#define SWITCHES_H

/**
 * Dashboard switch input reading and associated application of settings.
 */

/**
 * Set up memory structures that determine what each switch does.
 */
void Switches_init(void);

/**
 * Evaluate dashboard switch inputs and apply their settings.
 */
void Switches_100Hz(void);

#endif // SWITCHES_H