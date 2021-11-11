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

    float brake_pressure;
    Brake_read_pressure(&brake_pressure);

    CAN_send_message(MAIN_BUS_VC_PEDAL_INPUTS_FRAME_ID);

    // perform dynamic rationality checks on pedal inputs
    APPS_100Hz(&accel_pos, brake_pressure);

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

    // send all queued CAN messages
    CAN_send_queued_messages();

    // Blink heartbeat led
    HeartBeatLed_100Hz();
}