#include "MotorController.h"

#include "CAN.h"
#include "Config.h"

typedef struct {
    bool mc_messages_seen;
    bool mc_enabled;
    bool mc_state_ready;
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
    CAN_begin_counting_id(MAIN_BUS_M170_INTERNAL_STATES_FRAME_ID);
    last_checkin_ms = MC_CAN_TIMEOUT_MS + 1;
    last_mc_msg_count = 0;
    can_bus.mc_command.inverter_enable = 1;
    can_bus.mc_command.direction_command = 1; // go forward
    can_bus.mc_command.torque_command = 0;
    can_bus.mc_command.torque_limit_command = main_bus_m192_command_message_torque_limit_command_encode(ABSOLUTE_MAX_TORQUE_N);
    can_bus.mc_command.inverter_discharge = 1; // enable discharge
    can_bus.mc_command.speed_command = 0; // just in case
    can_bus.mc_command.speed_mode_enable = 0; // no speed mode
}

static void new_state(MCstate_e new)
{
    state = new;
    state_counter_ms = 0;
}

/**
 * Run the state machine.
 * Waits until the motor controller powers on, then sends the command message each iteration.
 */
void MotorController_100Hz(void)
{
    // calculate the time since last receiving a motor control message
    int new_count = CAN_get_count_for_id(MAIN_BUS_M170_INTERNAL_STATES_FRAME_ID);
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
        int mc_state = main_bus_m170_internal_states_d1_vsm_state_decode(can_bus.mc_state.d1_vsm_state);
        printf("VSM STATE: %d\r\n", mc_state);
        inputs.mc_state_ready = mc_state == 5 || mc_state == 6;
    }
    else
    {
        inputs.mc_messages_seen = false;
        inputs.mc_enabled = true;
        inputs.mc_state_ready = false;
    }

    // determine any transitions that must happen
    switch (state)
    {
        case MCstate_DISCONNECTED:
            printf("MC state: DISCONNECTED\r\n");
            if (inputs.mc_messages_seen)
            {
                state = MCstate_DISABLED_UNLOCKING;
            }
            break;

        
        
        case MCstate_DISABLED_UNLOCKING:
            printf("MC state: DISABLED_UNLOCKING\r\n");
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else
            {
                // we only stay in DISABLED_UNLOCKING for one iteration to flip the enabled bit low for one command message
                state = MCstate_DISABLED;
            }
            break;

        case MCstate_DISABLED:
            printf("MC state: DISABLED\r\n");
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else if (inputs.mc_enabled)
            {
                state = MCstate_ENABLED;
            }
            else if (state_counter_ms > UNLOCK_ATTEMPT_TIMEOUT_MS)
            {
                state = MCstate_DISABLED_UNLOCKING;
            }
            break;

        case MCstate_ENABLED:
        printf("MC state: ENABLED\r\n");
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
        printf("MC state: READY\r\n");
            if (!inputs.mc_messages_seen)
            {
                state = MCstate_DISCONNECTED;
            }
            else if (!inputs.mc_enabled)
            {
                state = MCstate_DISABLED;
            }
            break;
    }

    // now determine outputs
    outputs.mc_ready = state == MCstate_READY;
    outputs.attempt_unlock = state == MCstate_DISABLED_UNLOCKING;

    // now apply outputs
    if (outputs.attempt_unlock)
    {
        // attempt to unlock the motor controller
        
        can_bus.mc_command.inverter_enable = 0;
    }
    else
    {
        // otherwise hold it enabled
        can_bus.mc_command.inverter_enable = 1;
    }

    // increment state counter
    state_counter_ms += 10;

    // outputs.mc_ready is used by other modules

    // send command message
    CAN_send_message(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
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