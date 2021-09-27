#include "APPS.h"
#include "config.h"
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

static AppsState_e state = AppsState_SENSORS_AGREE;
static unsigned int state_counter_ms = 0;

static void new_state(AppsState_e new)
{
    state = new;
    state_counter_ms = 0;
}

static void state_machine(SmInputs_s* inputs, SmOutputs_s* outputs)
{
    switch (state)
    {
        case AppsState_SENSORS_AGREE:
            if (!inputs->accel_sensors_agree)
            {
                new_state(AppsState_SENSORS_DISAGREE);
            }
            break;

        case AppsState_SENSORS_DISAGREE:
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
            if (inputs->accel_sensors_agree)
            {
                new_state(AppsState_SENSORS_AGREE_FAULTED);
            }
            break;
        
        case AppsState_SENSORS_AGREE_FAULTED:
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

// Check if the pedals are within a certain percentage of each other
// Return true if they are, false otherwise
bool accel_pos_agree(AccelPos_s pos)
{
    float diff = pos.a - pos.b;

    return fabs(pos.a - diff)*100.0 < APPS_PEDAL_DISAGREEMENT_PERCENTAGE;
}

void pedal_disagreement_check(AccelPos_s accel_pos)
{
    SmInputs_s inputs = {accel_pos_agree(accel_pos)};
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
        FaultManager_set_fault_active(FaultCode_APPS_SENSOR_DISAGREEMENT);
    }
    else
    {
        FaultManager_clear_fault(FaultCode_APPS_SENSOR_DISAGREEMENT);
    }
}

void double_pedal_check(float average_accel_pos, float brake_pos)
{
    if (FaultManager_is_fault_active(FaultCode_APPS_DOUBLE_PEDAL))
    {
        if (brake_pos < DOUBLE_PEDAL_BRAKE_POS_RECOVERY_THRESHOLD)
        {
            FaultManager_clear_fault(FaultCode_APPS_DOUBLE_PEDAL);
        }
    }
    else
    {
        if ((average_accel_pos > 0) && (brake_pos > DOUBLE_PEDAL_BRAKE_POS_THRESHOLD))
        {
            FaultManager_set_fault_active(FaultCode_APPS_DOUBLE_PEDAL);
        }
    }
}


void APPS_100Hz(AccelPos_s accel_pos, float brake_pos)
{
    pedal_disagreement_check(accel_pos);
    double_pedal_check(accel_pos.average, brake_pos);
}