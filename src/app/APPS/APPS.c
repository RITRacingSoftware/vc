#include <math.h>

#include "APPS.h"
#include "CAN.h"
#include "Config.h"
#include "FaultManager.h"


typedef struct {
    bool accel_sensors_agree;
} SmInputs_s;

typedef struct {
    bool pedal_disagreement_fault;
} SmOutputs_s;

typedef enum {
    AppsState_SENSORS_AGREE,
    AppsState_SENSORS_DISAGREE,
    AppsState_SENSORS_AGREE_FAULTED,
    AppsState_SENSORS_DISAGREE_FAULTED,
} AppsState_e;

static AppsState_e state;
static unsigned int state_counter_ms;

static void new_state(AppsState_e new_s)
{
    state = new_s;
    state_counter_ms = 0;
}

static void state_machine(SmInputs_s* inputs, SmOutputs_s* outputs)
{
    switch (state)
    {
        case AppsState_SENSORS_AGREE:
            // printf("APPS_STATE: AGREE\r\n");
            if (!inputs->accel_sensors_agree)
            {
                new_state(AppsState_SENSORS_DISAGREE);
            }
            break;

        case AppsState_SENSORS_DISAGREE:
            // printf("APPS_STATE: DISAGREE\r\n");
            if (inputs->accel_sensors_agree)
            {
                new_state(AppsState_SENSORS_AGREE);
            }
            else if (state_counter_ms >= APPS_PEDAL_DISAGREEMENT_TIMEOUT_MS)
            {
                new_state(AppsState_SENSORS_DISAGREE_FAULTED);
            }
            break;
        
        case AppsState_SENSORS_DISAGREE_FAULTED:
            // printf("APPS_STATE: DISAGREE_FAULTED\r\n");
            if (inputs->accel_sensors_agree)
            {
                new_state(AppsState_SENSORS_AGREE_FAULTED);
            }
            break;
        
        case AppsState_SENSORS_AGREE_FAULTED:
            // printf("APPS_STATE: AGREE_FAULTED\r\n");
            if (!inputs->accel_sensors_agree)
            {
                new_state(AppsState_SENSORS_DISAGREE_FAULTED);
            }
            else if (state_counter_ms >= APPS_PEDAL_DISAGREEMENT_RECOVERY_MS)
            {
                new_state(AppsState_SENSORS_AGREE);
            }
            break;
    }
}

void APPS_init(void)
{
    state = AppsState_SENSORS_AGREE;
    state_counter_ms = 0;
}

// Check if the pedals are within a certain percentage of each other
// Return true if they are, false otherwise
bool accel_pos_agree(AccelPos_s* pos)
{   
    float err = fabs(pos->a - pos->b);
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_err = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_err_encode(err);
    return err < APPS_PEDAL_DISAGREEMENT_PERCENTAGE;
}

void pedal_disagreement_check(AccelPos_s* accel_pos)
{
    SmInputs_s inputs = {true};//accel_pos_agree(accel_pos)}; TEMP
    SmOutputs_s outputs;

    state_machine(&inputs, &outputs);

    // increment state counter
    state_counter_ms += 10;

    // determine outputs
    switch (state)
    {
        case AppsState_SENSORS_AGREE:
        case AppsState_SENSORS_DISAGREE:
            outputs.pedal_disagreement_fault = false;
            break;
        case AppsState_SENSORS_AGREE_FAULTED:
        case AppsState_SENSORS_DISAGREE_FAULTED:
            outputs.pedal_disagreement_fault = true;
            break;
    }

    // apply outputs
    if (outputs.pedal_disagreement_fault)
    {
        if (!FaultManager_is_fault_active(FaultCode_APPS_SENSOR_DISAGREEMENT))
        {
            // we should not continuously set faults to avoid spamming alert messaTEMP
        }
    }
    else
    {
        FaultManager_clear_fault(FaultCode_APPS_SENSOR_DISAGREEMENT);
    }
}

void double_pedal_check(float average_accel_pos, bool brake_on)
{
    if (FaultManager_is_fault_active(FaultCode_APPS_DOUBLE_PEDAL))
    {
        if (average_accel_pos < DOUBLE_PEDAL_APS_RECOVERY_THRESHOLD)
        {
            FaultManager_clear_fault(FaultCode_APPS_DOUBLE_PEDAL);
        }
    }
    else
    {
        if (brake_on && (average_accel_pos > DOUBLE_PEDAL_APS_THRESHOLD))
        {
            FaultManager_set_fault_active(FaultCode_APPS_DOUBLE_PEDAL);
        }
    }
}


void APPS_100Hz(AccelPos_s* accel_pos, bool brake_on)
{
    pedal_disagreement_check(accel_pos);
    double_pedal_check(accel_pos->average, brake_on);
}