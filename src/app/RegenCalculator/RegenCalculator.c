#include "RegenCalculator.h"

#include <stdint.h>

#include "Config.h"

#include "HAL_Dio.h"

#include "CAN.h"
#include "main_bus.h"

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

// Calculated using GNU units to prevent precision loss
// > units "1/minute * 11/40 * 16in * pi" "km/hour"
const static double RPM_TO_KMPH = 0.021066264;

static bool first_loop = false;
static bool regen_button_last = true; // Starts as true, so require a false-true cycle to enable
static float max_voltage_at_regen_enable;
static bool motor_speed_acceptable = false;

float RegenCalculator_calculate_regen()
{
    // Only regen if switch flipped
    bool regen_button_pressed = HAL_Dio_read(DIOpin_REGEN_BUTTON);

    first_loop = false;

    if(regen_button_pressed && !regen_button_last) {
        first_loop = true;
    }

    regen_button_last = regen_button_pressed;

    if(!regen_button_pressed) {
        return 0;
    }


    /**
     * Motor speed checks
    */
   
    // Note: Regen has 2 minimum speeds, one at which regen is disabled while slowing down, and one at which regen is reenabled while speeding up
    // This prevents a nasty oscillation at low speeds due to slack in the drive system

    // Estimate wheel speed from motor speed
    // Negative, because motor spins backwards
    double wheel_speed = -can_bus.motor_pos.d2_motor_speed * RPM_TO_KMPH;
    
    if(!motor_speed_acceptable){
        // If motor speed just exceeded activation speed, enable regen
        if(wheel_speed > REGEN_ACTIVATE_SPEED) {
            motor_speed_acceptable = true;
        }
    }
    else {
        // If motor speed just dropped below minimum, disable regen
        if(wheel_speed < REGEN_MIN_SPEED) {
            motor_speed_acceptable = false;
        }
    }

    // If motor is too slow, no regen
    if(!motor_speed_acceptable) {
        return 0;
    }


    /**
     * Battery status checks
    */

    // When button is initially pressed, save highest cell voltage
    // Only checking when enabling regen prevents an oscillation when accumulator SOC is near maximum
    // TODO: We may want to remove this, since it only rarely applies and adds confusing logic for the driver
    if (!regen_button_last && regen_button_pressed) {
        float max_cell_voltage = highest_cell(&can_bus.bms_voltages);
        max_voltage_at_regen_enable = max_cell_voltage;
    }
    regen_button_last = regen_button_pressed;

    // Don't regen if highest cell is near-full
    if(max_voltage_at_regen_enable > REGEN_MAX_VOLTS) {
        return 0;
    }


    /**
     * Calculate regen amount
    */

    // Get front brake pressure in PSI
    //lower bound 50-80 upper ~2000
    double front_brake_pressure = main_bus_c70_brake_pressures_brake_pressure_front_decode(can_bus.brake_pressures.brake_pressure_front);

    /* If criteria are met, subtract configured torque from commanded
	float regen_configured_torque = can_bus.regen_config.regen_torque * 0.1;
    float regen_requested = should_regen ? regen_configured_torque : 0.0;
    */

    //THe Max torque Limit to ensure the cells charge
    float upper_torque_limit = 40;
    //The lowest pressure required for regen 50 might be placeholder?
    float lower_pressure_limit = 50;
    //the highest threshold of pressure for the car to regen 2000 might be placeholder?
    float upper_pressure_limit = 2000;
    //the convertion rate for brake pressure to torque
    float torque_psi_convert = upper_torque_limit/(upper_pressure_limit-lower_pressure_limit);
    //the torque sent back for regen
    float regen_requested = 0.0;

    //Check if the break pressure is above the regen limit
    if(front_brake_pressure > lower_pressure_limit ){
        if(front_brake_pressure > upper_pressure_limit){
            regen_requested = upper_torque_limit;
        }
        else{
            regen_requested = (front_brake_pressure - lower_pressure_limit) * torque_psi_convert;
        }
    }

    return regen_requested;
}
