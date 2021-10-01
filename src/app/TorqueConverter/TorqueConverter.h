#ifndef TORQUE_CONVERTER_H
#define TORQUE_CONVERTER_H
#include "Accelerator.h"

/**
 * This module performs the transfer function from accelerator position to commanded torque.
 */

/**
 * Convert from pedal position to commanded torque (Nm).
 * pos - instantaneous accelerator position
 * return corresponding torque in Nm
 */
float TorqueConverter_pos_to_torque(AccelPos_s pos);

#endif // TORQUE_CONVERTER_H