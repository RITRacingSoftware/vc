#ifndef BRAKE_H
#define BRAKE_H

#include <stdbool.h>

/**
 * Read if the brake is pressed or not.
 * return true if pressed, false otherwise
 */
bool Brake_is_pressed(float voltage);

float Brake_Get_Voltage(void);


#endif // BRAKE_H