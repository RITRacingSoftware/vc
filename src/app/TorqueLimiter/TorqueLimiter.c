#include "TorqueLimiter.h"

#include "common_macros.h"
#include "Config.h"
#include "VehicleState.h"

float TorqueLimiter_apply_limit(float commanded_torque)
{
    if (VehicleState_allow_torque())
    {
        // saturate to absolute max
        return SAT(commanded_torque, MIN_TORQUE_NM, MAX_TORQUE_NM);
    }
    else
    {
        return 0;
    }
}
