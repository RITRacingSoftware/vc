#include "common_macros.h"

#include "TorqueConverter.h"

#include "Config.h"

/**
 * The transfer function from accelerator position to torque.
 * This function should only do this math and return, no other logic.
 */
float pos_to_t(float pos)
{
    float normalized_pos = MAX(pos - ACC_LOWER_DEADZONE_PERCENT, 0); // normalized pedal position wrt lower deadzone


    // range of pedal travel where torque is proportional to travel
    // float increasing_range = 100.0 - (ACC_LOWER_DEADZONE_PERCENT + ACC_UPPER_DEADZONE_PERCENT);
    float portion_of_max = MIN(normalized_pos / (100.0-ACC_UPPER_DEADZONE_PERCENT), 100);

    float torque = (portion_of_max) * MAX_TORQUE_NM;

    return torque;
}

float TorqueConverter_pos_to_torque(float pos)
{
    // don't command negative torque or torque above max torque
    return pos_to_t(pos);
}