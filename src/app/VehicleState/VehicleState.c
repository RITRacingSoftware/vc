#include "VehicleState.h"

#include <stdio.h>

#include "CAN.h"
#include "FaultManager.h"
#include "MotorController.h"
#include "SoundController.h"

// #define VEHICLE_STATE_DEBUG

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

void VehicleState_100Hz(float torque)
{
    // get inputs
    bool mc_ready = MotorController_is_ready();
    bool faulted = FaultManager_is_any_fault_active();
    bool torque_requested = torque > 0.0; // this input keeps the VC from instantaneously jumping from 0 requested torque to a high number
    bool pumps_running = can_bus.pbx_status.pbx_pumps_on;

    // determine state
    switch (state)
    {
        case VehicleState_NOT_READY:
        #ifdef VEHICLE_STATE_DEBUG
            printf("VEHICLE STATE: NOT READY\r\n");
        #endif
            if (mc_ready && !faulted && !torque_requested && pumps_running)
            {
                new_state(VehicleState_STARTUP);
            }

            break;
        
        case VehicleState_STARTUP:
        #ifdef VEHICLE_STATE_DEBUG
            printf("VEHICLE STATE: STARTUP\r\n");
        #endif
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
        #ifdef VEHICLE_STATE_DEBUG
            printf("VEHICLE STATE: READY_TO_DRIVE\r\n");
        #endif
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
        #ifdef VEHICLE_STATE_DEBUG
            printf("VEHICLE STATE: FAULTED\r\n");
        #endif
            if (!mc_ready)
            {
                new_state(VehicleState_NOT_READY);
            }
            else if (!faulted && !torque_requested)
            {
                new_state(VehicleState_READY_TO_DRIVE);
            }

            break;
    }

    // determine outputs
    bool play_sound = state == VehicleState_STARTUP;
    allow_torque = state == VehicleState_READY_TO_DRIVE;

    // apply outputs, which is now a CAN message that the PBX will read and power the speaker for the sound
    if (play_sound)
    {
        SoundController_play_sound(Sounds_READY_TO_DRIVE);
    }

    // allow_torque is accessed via getter by other modules

    // Update CAN status message
    can_bus.vc_status.vc_status_vehicle_state = formula_main_dbc_vc_status_vc_status_vehicle_state_encode(state);
}

bool VehicleState_allow_torque(void)
{
    return allow_torque;
}
