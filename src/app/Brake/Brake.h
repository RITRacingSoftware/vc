#ifndef BRAKE_H
#define BRAKE_H

#include <stdbool.h>

/**
 * Read brake pressure sensor line.
 * Convert brake pressure sensor reading to brake pressure.
 * Return true if pressure is rational, false otherwise.
 */
bool Brake_read_pressure(float* pressure);

#endif // BRAKE_H