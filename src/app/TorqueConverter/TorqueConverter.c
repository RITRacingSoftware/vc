#include "common_macros.h"

#include "TorqueConverter.h"

#include "Config.h"

/**
 * The transfer function from accelerator position to torque.
 * This function should only do this math and return, no other logic.
 */
float pos_to_t(float acc_pos)
{
    float normalized_pos = MAX(acc_pos - ACC_LOWER_DEADZONE_PERCENT, 0); // normalized pedal position wrt lower deadzone


    // range of pedal travel where torque is proportional to travel
    // float increasing_range = 100.0 - (ACC_LOWER_DEADZONE_PERCENT + ACC_UPPER_DEADZONE_PERCENT);
    float portion_of_max = MIN(normalized_pos / (100.0-ACC_UPPER_DEADZONE_PERCENT), 100);

    float torque = (portion_of_max) * MAX_TORQUE_NM;

    return torque;
}

/**
 * Reverse braking issues
 * returns 
*/
float break_to_t(float brake_v)
{
    // check if the speed of the car is below the minimum value
    float motor_speed = can_bus.mc_state.motor_position_status.d2_motor_speed;
    if(motor_speed * RPM_TO_KM_PER_HOUR <= MIN_REGEN_SPEED)
    {
        return 0;
    }
    float normalized_pos = MAX(brake_v - BPS_REGEN_LOWER_DEADZONE_V, 0); // normalized pedal position wrt lower deadzone

    float portion_of_max = MIN(normalized_pos / (BPS_REGEN_MAX_V), 100);

    float torque = -(portion_of_max * MAX_TORQUE_NM);

    return torque;
}


float TorqueConverter_pos_to_torque(float pos, float brake_v)
{
    
    // don't command negative torque or torque above max torque
    float commanded_torque = pos_to_t(pos);
    if(commanded_torque == 0)
    {
        break_to_t(brake_v);
    }
}
