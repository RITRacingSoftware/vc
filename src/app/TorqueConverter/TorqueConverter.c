#include "TorqueConverter.h"

#include "Config.h"

/**
 * The transfer function from accelerator position to torque.
 * This function should only do this math and return, no other logic.
 */
float pos_to_t(float pos)
{
    // basic linear function for now
    (pos / 100.0) * MAX_TORQUE_NM;
}

float TorqueConverter_pos_to_torque(float pos)
{
    return pos_to_t(pos);
}