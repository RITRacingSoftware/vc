#ifndef REGEN_CALCULATOR_H
#define REGEN_CALCULATOR_H

/**
 * Based on vehicle state, calculate if and how much regenerative braking is desired
 * Returns a positive torque value in Nm indicating the regen torque which should be applied
 */
float RegenCalculator_calculate_regen();


#endif // REGEN_CALCULATOR_H