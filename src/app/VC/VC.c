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

// Calculated using GNU units to prevent precision loss
// > units "1/minute * 11/40 * 16in * pi" "km/hour"
const static double RPM_TO_KMPH = 0.021066264;

static bool regen_button_last;
static float max_voltage_at_regen_enable;

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

static float highest_cell(struct main_bus_bms_voltages_t* voltages) {
    // Yeah, this sucks. This should become an array somehow
    uint16_t highest = 0;
    if (voltages->bms_voltages_cell0 > highest) { highest = voltages->bms_voltages_cell0; }
    if (voltages->bms_voltages_cell1 > highest) { highest = voltages->bms_voltages_cell1; }
    if (voltages->bms_voltages_cell2 > highest) { highest = voltages->bms_voltages_cell2; }
    if (voltages->bms_voltages_cell3 > highest) { highest = voltages->bms_voltages_cell3; }
    if (voltages->bms_voltages_cell4 > highest) { highest = voltages->bms_voltages_cell4; }
    if (voltages->bms_voltages_cell5 > highest) { highest = voltages->bms_voltages_cell5; }
    if (voltages->bms_voltages_cell6 > highest) { highest = voltages->bms_voltages_cell6; }
    if (voltages->bms_voltages_cell7 > highest) { highest = voltages->bms_voltages_cell7; }
    if (voltages->bms_voltages_cell8 > highest) { highest = voltages->bms_voltages_cell8; }
    if (voltages->bms_voltages_cell9 > highest) { highest = voltages->bms_voltages_cell9; }
    if (voltages->bms_voltages_cell10 > highest) { highest = voltages->bms_voltages_cell10; }
    if (voltages->bms_voltages_cell11 > highest) { highest = voltages->bms_voltages_cell11; }
    if (voltages->bms_voltages_cell12 > highest) { highest = voltages->bms_voltages_cell12; }
    if (voltages->bms_voltages_cell13 > highest) { highest = voltages->bms_voltages_cell13; }
    if (voltages->bms_voltages_cell14 > highest) { highest = voltages->bms_voltages_cell14; }
    if (voltages->bms_voltages_cell15 > highest) { highest = voltages->bms_voltages_cell15; }
    if (voltages->bms_voltages_cell16 > highest) { highest = voltages->bms_voltages_cell16; }
    if (voltages->bms_voltages_cell17 > highest) { highest = voltages->bms_voltages_cell17; }
    if (voltages->bms_voltages_cell18 > highest) { highest = voltages->bms_voltages_cell18; }
    if (voltages->bms_voltages_cell19 > highest) { highest = voltages->bms_voltages_cell19; }
    if (voltages->bms_voltages_cell10 > highest) { highest = voltages->bms_voltages_cell20; }
    if (voltages->bms_voltages_cell21 > highest) { highest = voltages->bms_voltages_cell21; }
    if (voltages->bms_voltages_cell22 > highest) { highest = voltages->bms_voltages_cell22; }
    if (voltages->bms_voltages_cell23 > highest) { highest = voltages->bms_voltages_cell23; }
    if (voltages->bms_voltages_cell24 > highest) { highest = voltages->bms_voltages_cell24; }
    if (voltages->bms_voltages_cell25 > highest) { highest = voltages->bms_voltages_cell25; }
    if (voltages->bms_voltages_cell26 > highest) { highest = voltages->bms_voltages_cell26; }
    if (voltages->bms_voltages_cell27 > highest) { highest = voltages->bms_voltages_cell27; }
    if (voltages->bms_voltages_cell28 > highest) { highest = voltages->bms_voltages_cell28; }
    if (voltages->bms_voltages_cell29 > highest) { highest = voltages->bms_voltages_cell29; }
    if (voltages->bms_voltages_cell20 > highest) { highest = voltages->bms_voltages_cell30; }
    if (voltages->bms_voltages_cell31 > highest) { highest = voltages->bms_voltages_cell31; }
    if (voltages->bms_voltages_cell32 > highest) { highest = voltages->bms_voltages_cell32; }
    if (voltages->bms_voltages_cell33 > highest) { highest = voltages->bms_voltages_cell33; }
    if (voltages->bms_voltages_cell34 > highest) { highest = voltages->bms_voltages_cell34; }
    if (voltages->bms_voltages_cell35 > highest) { highest = voltages->bms_voltages_cell35; }
    if (voltages->bms_voltages_cell36 > highest) { highest = voltages->bms_voltages_cell36; }
    if (voltages->bms_voltages_cell37 > highest) { highest = voltages->bms_voltages_cell37; }
    if (voltages->bms_voltages_cell38 > highest) { highest = voltages->bms_voltages_cell38; }
    if (voltages->bms_voltages_cell39 > highest) { highest = voltages->bms_voltages_cell39; }
    if (voltages->bms_voltages_cell40 > highest) { highest = voltages->bms_voltages_cell40; }
    if (voltages->bms_voltages_cell41 > highest) { highest = voltages->bms_voltages_cell41; }
    if (voltages->bms_voltages_cell42 > highest) { highest = voltages->bms_voltages_cell42; }
    if (voltages->bms_voltages_cell43 > highest) { highest = voltages->bms_voltages_cell43; }
    if (voltages->bms_voltages_cell44 > highest) { highest = voltages->bms_voltages_cell44; }
    if (voltages->bms_voltages_cell45 > highest) { highest = voltages->bms_voltages_cell45; }
    if (voltages->bms_voltages_cell46 > highest) { highest = voltages->bms_voltages_cell46; }
    if (voltages->bms_voltages_cell47 > highest) { highest = voltages->bms_voltages_cell47; }
    if (voltages->bms_voltages_cell48 > highest) { highest = voltages->bms_voltages_cell48; }
    if (voltages->bms_voltages_cell49 > highest) { highest = voltages->bms_voltages_cell49; }
    if (voltages->bms_voltages_cell50 > highest) { highest = voltages->bms_voltages_cell50; }
    if (voltages->bms_voltages_cell51 > highest) { highest = voltages->bms_voltages_cell51; }
    if (voltages->bms_voltages_cell52 > highest) { highest = voltages->bms_voltages_cell52; }
    if (voltages->bms_voltages_cell53 > highest) { highest = voltages->bms_voltages_cell53; }
    if (voltages->bms_voltages_cell54 > highest) { highest = voltages->bms_voltages_cell54; }
    if (voltages->bms_voltages_cell55 > highest) { highest = voltages->bms_voltages_cell55; }
    if (voltages->bms_voltages_cell56 > highest) { highest = voltages->bms_voltages_cell56; }
    if (voltages->bms_voltages_cell57 > highest) { highest = voltages->bms_voltages_cell57; }
    if (voltages->bms_voltages_cell58 > highest) { highest = voltages->bms_voltages_cell58; }
    if (voltages->bms_voltages_cell59 > highest) { highest = voltages->bms_voltages_cell59; }
    if (voltages->bms_voltages_cell60 > highest) { highest = voltages->bms_voltages_cell60; }
    if (voltages->bms_voltages_cell61 > highest) { highest = voltages->bms_voltages_cell61; }
    if (voltages->bms_voltages_cell62 > highest) { highest = voltages->bms_voltages_cell62; }
    if (voltages->bms_voltages_cell63 > highest) { highest = voltages->bms_voltages_cell63; }
    if (voltages->bms_voltages_cell64 > highest) { highest = voltages->bms_voltages_cell64; }
    if (voltages->bms_voltages_cell65 > highest) { highest = voltages->bms_voltages_cell65; }
    if (voltages->bms_voltages_cell66 > highest) { highest = voltages->bms_voltages_cell66; }
    if (voltages->bms_voltages_cell67 > highest) { highest = voltages->bms_voltages_cell67; }
    if (voltages->bms_voltages_cell68 > highest) { highest = voltages->bms_voltages_cell68; }
    if (voltages->bms_voltages_cell69 > highest) { highest = voltages->bms_voltages_cell69; }
    if (voltages->bms_voltages_cell70 > highest) { highest = voltages->bms_voltages_cell70; }
    if (voltages->bms_voltages_cell71 > highest) { highest = voltages->bms_voltages_cell71; }
    if (voltages->bms_voltages_cell72 > highest) { highest = voltages->bms_voltages_cell72; }
    if (voltages->bms_voltages_cell73 > highest) { highest = voltages->bms_voltages_cell73; }
    if (voltages->bms_voltages_cell74 > highest) { highest = voltages->bms_voltages_cell74; }
    if (voltages->bms_voltages_cell75 > highest) { highest = voltages->bms_voltages_cell75; }
    if (voltages->bms_voltages_cell76 > highest) { highest = voltages->bms_voltages_cell76; }
    if (voltages->bms_voltages_cell77 > highest) { highest = voltages->bms_voltages_cell77; }
    if (voltages->bms_voltages_cell78 > highest) { highest = voltages->bms_voltages_cell78; }
    if (voltages->bms_voltages_cell79 > highest) { highest = voltages->bms_voltages_cell79; }
    if (voltages->bms_voltages_cell80 > highest) { highest = voltages->bms_voltages_cell80; }
    if (voltages->bms_voltages_cell81 > highest) { highest = voltages->bms_voltages_cell81; }
    if (voltages->bms_voltages_cell82 > highest) { highest = voltages->bms_voltages_cell82; }
    if (voltages->bms_voltages_cell83 > highest) { highest = voltages->bms_voltages_cell83; }
    if (voltages->bms_voltages_cell84 > highest) { highest = voltages->bms_voltages_cell84; }
    if (voltages->bms_voltages_cell85 > highest) { highest = voltages->bms_voltages_cell85; }
    if (voltages->bms_voltages_cell86 > highest) { highest = voltages->bms_voltages_cell86; }
    if (voltages->bms_voltages_cell87 > highest) { highest = voltages->bms_voltages_cell87; }
    if (voltages->bms_voltages_cell88 > highest) { highest = voltages->bms_voltages_cell88; }
    if (voltages->bms_voltages_cell89 > highest) { highest = voltages->bms_voltages_cell89; }

    return highest * 0.01;
}

void VC_100Hz(void)
{
    // read the new pedal inputs, perform basic rationality checks on values read
    AccelPos_s accel_pos;
    bool is_accelerator_rational = Accelerator_read_positions(&accel_pos);

    float brake_on = Brake_is_pressed();

    CAN_send_message(MAIN_BUS_VC_PEDAL_INPUTS_FRAME_ID);
    CAN_send_message(MAIN_BUS_VC_PEDAL_INPUTS_RAW_FRAME_ID);

    // perform dynamic rationality checks on pedal inputs
    APPS_100Hz(&accel_pos, brake_on, is_accelerator_rational);

    // keep up to date with the state of the motor controller. Enable it if necessary.
    MotorController_100Hz();

    // read new dashboard switch inputs
    Switches_100Hz();

    // Stop sound triggers that are done triggering
    SoundController_100Hz();
    CAN_send_message(MAIN_BUS_VC_RTDS_REQUEST_FRAME_ID);

    // figure out new vehicle state based on changes this iteration
    VehicleState_100Hz(commanded_torque);

    // calculate the torque to request based on the accelerator pedal input
    commanded_torque = TorqueConverter_pos_to_torque(accel_pos.average);

    can_bus.vc_pedal_inputs.vc_pedal_inputs_torque_requested = main_bus_vc_pedal_inputs_vc_pedal_inputs_torque_requested_encode(commanded_torque);

    // Only regen if button pressed
    bool regen_button_pressed = HAL_Dio_read(DIOpin_REGEN_BUTTON);

    // Don't regen below 5 km/h
    double wheel_speed = -can_bus.motor_pos.d2_motor_speed * RPM_TO_KMPH;
    bool wheel_speed_high = wheel_speed > 5.0;

    // When button is initially pressed, save highest cell voltage
    if (!regen_button_last && regen_button_pressed) {
        float max_cell_voltage = highest_cell(&can_bus.bms_voltages);
        max_voltage_at_regen_enable = max_cell_voltage;
    }
    regen_button_last = regen_button_pressed;

    // Don't regen if highest cell voltage > 4.1
    // Read voltage from when button was pressed to prevent nasty oscillations
    bool cells_low_enough = max_voltage_at_regen_enable <= 4.1;

    // Require all 3 criteria to regen
    bool should_regen = regen_button_pressed && wheel_speed_high && cells_low_enough;

    // If criteria are met, subtract configured torque from commanded
	float regen_configured_torque = can_bus.regen_config.regen_torque * 0.1;
    float regen_requested = should_regen ? regen_configured_torque : 0.0;
    float torque_minus_regen = commanded_torque - regen_requested;

    // limit torque to max torque, or 0 if the system is not ready or faulted
    limited_torque = TorqueLimiter_apply_limit(torque_minus_regen);

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

    // CAN_send_message(MAIN_BUS_VC_SHUTDOWN_STATUS_FRAME_ID);
}
