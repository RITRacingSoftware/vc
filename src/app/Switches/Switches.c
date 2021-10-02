#include <stdbool.h>
#include <string.h>

#include "CAN.h"
#include "Switches.h"
#include "HAL_Dio.h"

#define NUM_SWITCHES 4

bool switch_states[NUM_SWITCHES];

static void edge_action(int switch_num, bool rising_edge)
{
    // empty now, don't have any use for switches yet.
    switch (switch_num)
    {
        case 0:
            if (rising_edge)
            {

            }
            else
            {

            }
            break;

        default:
            break;
    }
}

void Switches_init(void)
{
    for (int i = 0; i < NUM_SWITCHES; i++) switch_states[i] = 0;
}

void Switches_100Hz(void)
{
    // read new switch inputs
    bool new_switch_states[NUM_SWITCHES];
    new_switch_states[0] = HAL_Dio_read(DIOpin_SWITCH_0);
    new_switch_states[1] = HAL_Dio_read(DIOpin_SWITCH_0);
    new_switch_states[2] = HAL_Dio_read(DIOpin_SWITCH_0);  
    new_switch_states[3] = HAL_Dio_read(DIOpin_SWITCH_0);

    // detect any switch edges and act on them
    for (int i = 0; i < NUM_SWITCHES; i++)
    {
        if (new_switch_states[i] != switch_states[i])
        {
            bool rising_edge = new_switch_states[i] == 1;
            edge_action(i, rising_edge);
            switch_states[i] = new_switch_states[i];
        }
    }

    // update CAN message bits and send them
    can_bus.vc_dash_inputs.vc_dash_inputs_switch0 = switch_states[0];
    can_bus.vc_dash_inputs.vc_dash_inputs_switch1 = switch_states[1];
    can_bus.vc_dash_inputs.vc_dash_inputs_switch2 = switch_states[2];
    can_bus.vc_dash_inputs.vc_dash_inputs_switch3 = switch_states[3]; 

    CAN_send_message(MAIN_BUS_VC_DASH_INPUTS_FRAME_ID);
}