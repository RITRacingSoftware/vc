#include <stdio.h>
#include "MotorController.h"

#include "Brake.h"
#include "CAN.h"
#include "Config.h"
#include "HAL_Dio.h"

// #define MC_DEBUG

typedef struct {
    bool mc_messages_seen;
    bool mc_enabled;
    bool mc_state_ready;
    bool brake_pressed;
    bool rtd_button_pressed;
} SMinputs_s;

typedef struct {
    bool attempt_unlock;
    bool mc_ready;
} SMoutputs_s;

static MCstate_e state;
static SMinputs_s inputs;
static SMoutputs_s outputs;
static unsigned int state_counter_ms;
static float commanded_torque;
static int last_checkin_ms;
static int last_mc_msg_count;

void MotorController_init(void)
{
    state = MCstate_DISCONNECTED;
    state_counter_ms = 0;
    commanded_torque = 0;
    CAN_begin_counting_id(FORMULA_MAIN_DBC_MCU_INTERNAL_STATES_FRAME_ID);
    last_checkin_ms = MC_CAN_TIMEOUT_MS + 1;
    last_mc_msg_count = 0;
    can_bus.mc_command.inverter_enable = 1;
    can_bus.mc_command.direction_command = 0; // go forward
    can_bus.mc_command.torque_command = 0;
    can_bus.mc_command.torque_limit_command = formula_main_dbc_mcu_command_message_torque_limit_command_encode(ABSOLUTE_MAX_TORQUE_NM);
    can_bus.mc_command.inverter_discharge = 1; // enable discharge
    can_bus.mc_command.speed_command = 0; // just in case
    can_bus.mc_command.speed_mode_enable = 0; // no speed mode
}

static void new_state(MCstate_e new_s)
{
    state = new_s;
    state_counter_ms = 0;
}

/**
 * Run the state machine.
 * Waits until the motor controller powers on, then sends the command message each iteration.
 */
void MotorController_100Hz(void)
{
    // calculate the time since last receiving a motor control message
    int new_count = CAN_get_count_for_id(FORMULA_MAIN_DBC_MCU_INTERNAL_STATES_FRAME_ID);
    if (new_count == last_mc_msg_count)
    {
        last_checkin_ms += 10;
    }
    else
    {
        last_checkin_ms = 0;
    }
    last_mc_msg_count = new_count;

    // determine inputs
    if (last_checkin_ms < MC_CAN_TIMEOUT_MS)
    {
        inputs.mc_messages_seen = true;
        inputs.mc_enabled = can_bus.mc_state.d6_inverter_enable_state;
        // states 5 and 6 are ready and running, respectively
        int mc_state = formula_main_dbc_mcu_internal_states_d1_vsm_state_decode(can_bus.mc_state.d1_vsm_state);
        inputs.mc_state_ready = mc_state == 5 || mc_state == 6;
    }
    else
    {
        inputs.mc_messages_seen = false;
        inputs.mc_enabled = true;
        inputs.mc_state_ready = false;
    }

    inputs.brake_pressed = Brake_is_pressed();
    inputs.rtd_button_pressed = HAL_Dio_read(DIOpin_RTD_BUTTON);
    can_bus.vc_dash_inputs.vc_dash_inputs_rt_dbutton = inputs.rtd_button_pressed;

    // determine any transitions that must happen
    switch (state)
    {
        case MCstate_DISCONNECTED:
        #ifdef MC_DEBUG
            printf("MC state: DISCONNECTED\r\n");
        #endif
            if (inputs.mc_messages_seen)
            {
                state = MCstate_DISABLED;
            }
            break;

        case MCstate_DISABLED:
        #ifdef MC_DEBUG
            printf("MC state: DISABLED\r\n");
        #endif
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else if (inputs.brake_pressed && inputs.rtd_button_pressed)
            {
                state = MCstate_DISABLED_UNLOCKING;
            }
            break;

        case MCstate_DISABLED_UNLOCKING:
        #ifdef MC_DEBUG
            printf("MC state: DISABLED_UNLOCKING\r\n");
        #endif
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else if (!inputs.brake_pressed || !inputs.rtd_button_pressed)
            {
                state = MCstate_DISABLED;
            }
            else if (inputs.mc_enabled)
            {
                state = MCstate_ENABLED;
            }
            break;

        case MCstate_ENABLED:
        #ifdef MC_DEBUG
            printf("MC state: ENABLED\r\n");
        #endif
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else if (!inputs.mc_enabled)
            {
                state = MCstate_DISABLED;
            }
            else if (inputs.mc_state_ready)
            {
                state = MCstate_READY;
            }
            break;

        case MCstate_READY:
        #ifdef MC_DEBUG
            printf("MC state: READY\r\n");
        #endif
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else if (!inputs.mc_state_ready)
            {
                state = MCstate_ENABLED;
            }
            else if (!inputs.mc_enabled)
            {
                state = MCstate_DISABLED;
            }
            break;
    }

    // now determine outputs
    outputs.mc_ready = state == MCstate_READY;
    outputs.attempt_unlock = (state == MCstate_DISABLED_UNLOCKING) || (state == MCstate_ENABLED) || (state == MCstate_READY);

    // now apply outputs
    if (outputs.attempt_unlock)
    {
        can_bus.mc_command.inverter_enable = 1;
    }
    else
    {
        can_bus.mc_command.inverter_enable = 0;
    }

    // increment state counter
    state_counter_ms += 10;

    // outputs.mc_ready is used by other modules

    // update command message with any torque
    can_bus.mc_command.torque_command =  formula_main_dbc_mcu_command_message_torque_command_encode(commanded_torque);

    // send command message
    CAN_send_message(FORMULA_MAIN_DBC_MCU_COMMAND_MESSAGE_FRAME_ID);

    // update status CAN message
    can_bus.vc_status.vc_status_mc_state = formula_main_dbc_vc_status_vc_status_mc_state_encode(state);
}

/**
 * Set the torque to be commanded to the motor controller.
 * This will not be commanded if the motor controller is not ready.
 */
void MotorController_set_torque(float torque)
{
    commanded_torque = torque;
}

bool MotorController_is_ready(void)
{
    return outputs.mc_ready;
}

MCstate_e MotorController_get_state(void)
{
    return state;
}
