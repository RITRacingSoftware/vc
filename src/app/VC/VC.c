#include "VC.h"

#include <stdio.h>

#include "Accelerator.h"
#include "APPS.h"
#include "Brake.h"
#include "CAN.h"
#include "FaultManager.h"
#include "HeartBeatLed.h"
#include "MotorController.h"
#include "ShutdownMonitor.h"
#include "SoundController.h"
#include "Switches.h"
#include "TorqueConverter.h"
#include "TorqueLimiter.h"
#include "VehicleState.h"

#include "main_bus.h"
#include "HAL_Dio.h"

// #define VC_DEBUG

static float commanded_torque = 0;
static float limited_torque = 0;

void VC_init(void)
{
    // initialize all app modules (app modules not initialized here dont have an init)
    CAN_init(); // should be up front
    APPS_init();
    FaultManager_init();
    HeartBeatLed_init();
    MotorController_init();
    ShutdownMonitor_init();
    SoundController_init();
    Switches_init();
    VehicleState_init();
}

void VC_100Hz(void)
{
    // read the new pedal inputs, perform basic rationality checks on values read
    AccelPos_s accel_pos;
    Accelerator_read_positions(&accel_pos);

    float brake_on = Brake_is_pressed();

    CAN_send_message(MAIN_BUS_VC_PEDAL_INPUTS_FRAME_ID);
    CAN_send_message(MAIN_BUS_VC_PEDAL_INPUTS_RAW_FRAME_ID);

    // perform dynamic rationality checks on pedal inputs
    APPS_100Hz(&accel_pos, brake_on);

    // keep up to date with the state of the motor controller. Enable it if necessary.
    MotorController_100Hz();

    // read new dashboard switch inputs
    Switches_100Hz();

    // Stop sound triggers that are done triggering
    SoundController_100Hz();

    // figure out new vehicle state based on changes this iteration
    VehicleState_100Hz(commanded_torque);

    // calculate the torque to request based on the accelerator pedal input
    commanded_torque = TorqueConverter_pos_to_torque(accel_pos.average);

    can_bus.vc_pedal_inputs.vc_pedal_inputs_torque_requested = main_bus_vc_pedal_inputs_vc_pedal_inputs_torque_requested_encode(commanded_torque);

    // limit torque to max torque, or 0 if the system is not ready or faulted
    limited_torque = TorqueLimiter_apply_limit(commanded_torque);

#ifdef VC_DEBUG
    printf("Position: %f, Commanded: %f, Limited: %f\r\n", accel_pos.average, commanded_torque, limited_torque);
#endif

    // command the safety-checked torque to the motor controller
    MotorController_set_torque(limited_torque);

    // send periodic status messages
    CAN_send_message(MAIN_BUS_VC_FAULT_VECTOR_FRAME_ID);
    CAN_send_message(MAIN_BUS_VC_STATUS_FRAME_ID);

    // Blink heartbeat led
    HeartBeatLed_100Hz();
}

void VC_1kHz(void)
{
    static struct main_bus_vc_shutdown_status_t status;
    status.vc_shutdown_status_bms_fault = HAL_Dio_read(DIOpin_BMS_FAULT);
    status.vc_shutdown_status_imd_fault = HAL_Dio_read(DIOpin_IMD_FAULT);
    status.vc_shutdown_status_bspd_fault = HAL_Dio_read(DIOpin_BSPD_FAULT);
    status.vc_shutdown_status_bspd_signal_lost = HAL_Dio_read(DIOpin_BSPD_SIGNAL_LOST);
    status.vc_shutdown_status_precharge = HAL_Dio_read(DIOpin_PRECHARGE);
    
    ShutdownMonitor_update(&status);

    CAN_send_message(MAIN_BUS_VC_SHUTDOWN_STATUS_FRAME_ID);
}