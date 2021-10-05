#include "VehicleState.h"

#include "CAN.h"
#include "FaultManager.h"
#include "MotorController.h"
#include "SoundController.h"

VehicleState_e state;
static bool allow_torque;

void VehicleState_init(void)
{
    state = VehicleState_NOT_READY;
}

static void new_state(VehicleState_e new_s)
{
    state = new_s;
}

void VehicleState_100Hz(void)
{
    // get inputs
    bool mc_ready = MotorController_is_ready();
    bool faulted = FaultManager_is_any_fault_active();

    // determine state
    switch (state)
    {
        case VehicleState_NOT_READY:
            if (mc_ready && !faulted)
            {
                new_state(VehicleState_STARTUP);
            }

            break;
        
        case VehicleState_STARTUP:
            if (!mc_ready)
            {
                new_state(VehicleState_NOT_READY);
            }
            else if (faulted)
            {
                new_state(VehicleState_FAULTED);
            }
            else
            {
                new_state(VehicleState_READY_TO_DRIVE);
            }

            break;

        case VehicleState_READY_TO_DRIVE:
            if (!mc_ready)
            {
                new_state(VehicleState_NOT_READY);
            }
            else if (faulted)
            {
                new_state(VehicleState_FAULTED);
            }
        
            break;

        case VehicleState_FAULTED:
            if (!mc_ready)
            {
                new_state(VehicleState_NOT_READY);
            }
            else if (!faulted)
            {
                new_state(VehicleState_READY_TO_DRIVE);
            }

            break;
    }

    // determine outputs
    bool play_sound = state == VehicleState_STARTUP;
    allow_torque = state == VehicleState_READY_TO_DRIVE;

    // apply outputs
    if (play_sound)
    {
        SoundController_play_sound(Sounds_READY_TO_DRIVE);
    }

    // allow_torque is accessed via getter by other modules

    // Update CAN status message
    can_bus.vc_status.vc_status_vehicle_state = main_bus_vc_status_vc_status_vehicle_state_encode(state);
}

bool VehicleState_allow_torque(void)
{
    return allow_torque;
}