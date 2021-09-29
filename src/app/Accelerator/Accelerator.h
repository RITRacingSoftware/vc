#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include <stdbool.h>

/**
 * Convert throttle position sensor voltage readings to throttle positions.
 */

typedef struct {
    float a;
    float b;
    float average;
} AccelPos_s;

/**
 * Convert throttle position sensor voltage readings to positions.
 * Average them.
 * Fault on irrationality.
 * Return true if rational, false otherwise.
 */
bool Accelerator_read_positions(AccelPos_s*);

#endif // ACCELERATOR_H