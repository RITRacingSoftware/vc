#include <math.h>

#include "APPS.h"
#include "CAN.h"
#include "Config.h"
#include "FaultManager.h"


typedef struct {
    bool accel_sensors_agree;
    bool accelerator_rational;
} DisagreementSmInputs_s;

typedef struct {
    bool pedal_disagreement_fault;
} DisagreementSmOutputs_s;

typedef struct {
    bool both_pedals_pressed;
    float average_accel_pos;
} DoublePedalSmInputs_s;

typedef struct {
    bool double_pedal_fault;
} DoublePedalSmOutputs_s;

typedef enum {
    AppsDisagreementState_SENSORS_AGREE,
    AppsDisagreementState_SENSORS_DISAGREE,
    AppsDisagreementState_SENSORS_AGREE_FAULTED,
    AppsDisagreementState_SENSORS_DISAGREE_FAULTED,
} AppsDisagreementState_e;

typedef enum {
    AppsDoublePedalState_NOT_ACTIVE,
    AppsDoublePedaltate_DOUBLE_PEDAL,
    AppsDoublePedalState_DOUBLE_PEDAL_FAULTED,
} AppsDoublePedalState_e;

static AppsDisagreementState_e disagreement_state;
static unsigned int disagreement_state_counter_ms;

static AppsDoublePedalState_e double_pedal_state;
static unsigned int double_pedal_state_counter_ms;

static void new_disagreement_state(AppsDisagreementState_e new_s)
{
    disagreement_state = new_s;
    disagreement_state_counter_ms = 0;
}

static void new_double_pedal_state(AppsDoublePedalState_e new_s)
{
    double_pedal_state = new_s;
    double_pedal_state_counter_ms = 0;
}

static void disagreement_state_machine(DisagreementSmInputs_s* inputs, DisagreementSmOutputs_s* outputs)
{
    switch (disagreement_state)
    {
        case AppsDisagreementState_SENSORS_AGREE:
            // printf("APPS_STATE: AGREE\r\n");
            if (!inputs->accel_sensors_agree || !inputs->accelerator_rational)
            {
                new_disagreement_state(AppsDisagreementState_SENSORS_DISAGREE);
            }
            break;

        case AppsDisagreementState_SENSORS_DISAGREE:
            // printf("APPS_STATE: DISAGREE\r\n");
            if (inputs->accel_sensors_agree && inputs->accelerator_rational)
            {
                new_disagreement_state(AppsDisagreementState_SENSORS_AGREE);
            }
            else if (disagreement_state_counter_ms >= APPS_PEDAL_DISAGREEMENT_TIMEOUT_MS)
            {
                new_disagreement_state(AppsDisagreementState_SENSORS_DISAGREE_FAULTED);
            }
            break;
        
        case AppsDisagreementState_SENSORS_DISAGREE_FAULTED:
            // printf("APPS_STATE: DISAGREE_FAULTED\r\n");
            if (inputs->accel_sensors_agree && inputs->accelerator_rational)
            {
                new_disagreement_state(AppsDisagreementState_SENSORS_AGREE_FAULTED);
            }
            break;
        
        case AppsDisagreementState_SENSORS_AGREE_FAULTED:
            // printf("APPS_STATE: AGREE_FAULTED\r\n");
            if (!inputs->accel_sensors_agree || !inputs->accelerator_rational)
            {
                new_disagreement_state(AppsDisagreementState_SENSORS_DISAGREE_FAULTED);
            }
            else if (disagreement_state_counter_ms >= APPS_PEDAL_DISAGREEMENT_RECOVERY_MS)
            {
                new_disagreement_state(AppsDisagreementState_SENSORS_AGREE);
            }
            break;
    }
}

static void double_pedal_state_machine(DoublePedalSmInputs_s* inputs, DoublePedalSmOutputs_s* outputs)
{
    switch (double_pedal_state)
    {
        case AppsDoublePedalState_NOT_ACTIVE:
            if (inputs->both_pedals_pressed)
            {
                new_double_pedal_state(AppsDoublePedaltate_DOUBLE_PEDAL);
            }
            break;

        case AppsDoublePedaltate_DOUBLE_PEDAL:
            // printf("APPS_STATE: DISAGREE\r\n");
            if (!inputs->both_pedals_pressed)
            {
                new_double_pedal_state(AppsDoublePedalState_NOT_ACTIVE);
            }
            else if (double_pedal_state_counter_ms >= APPS_DOUBLE_PEDAL_TIMEOUT_MS)
            {
                new_double_pedal_state(AppsDoublePedalState_DOUBLE_PEDAL_FAULTED);
            }
            break;
        
        case AppsDoublePedalState_DOUBLE_PEDAL_FAULTED:
            // printf("APPS_STATE: DISAGREE_FAULTED\r\n");
            if (inputs->average_accel_pos <= DOUBLE_PEDAL_APS_RECOVERY_THRESHOLD)
            {
                new_double_pedal_state(AppsDoublePedalState_NOT_ACTIVE);
            }
            break;
    }
}

void APPS_init(void)
{
    disagreement_state = AppsDisagreementState_SENSORS_AGREE;
    disagreement_state_counter_ms = 0;
}

// Check if the pedals are within a certain percentage of each other
// Return true if they are, false otherwise
bool accel_pos_agree(AccelPos_s* pos)
{   
    float err = fabs(pos->a - pos->b);
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_err = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_err_encode(err);
    return err < APPS_PEDAL_DISAGREEMENT_PERCENTAGE;
}

void pedal_disagreement_check(AccelPos_s* accel_pos, bool is_accelerator_rational)
{
    DisagreementSmInputs_s disagreement_inputs = {accel_pos_agree(accel_pos), is_accelerator_rational}; 
    DisagreementSmOutputs_s disagreement_outputs;

    disagreement_state_machine(&disagreement_inputs, &disagreement_outputs);

    // increment state counter
    disagreement_state_counter_ms += 10;

    // determine outputs
    switch (disagreement_state)
    {
        case AppsDisagreementState_SENSORS_AGREE:
        case AppsDisagreementState_SENSORS_DISAGREE:
            disagreement_outputs.pedal_disagreement_fault = false;
            break;
        case AppsDisagreementState_SENSORS_AGREE_FAULTED:
        case AppsDisagreementState_SENSORS_DISAGREE_FAULTED:
            disagreement_outputs.pedal_disagreement_fault = true;
            break;
    }

    // apply outputs
    if (disagreement_outputs.pedal_disagreement_fault)
    {
        if (!FaultManager_is_fault_active(FaultCode_APPS_SENSOR_DISAGREEMENT))
        {
            // we should not continuously set faults to avoid spamming alert message
            FaultManager_set_fault_active(FaultCode_APPS_SENSOR_DISAGREEMENT);
        }
    }
    else
    {
        FaultManager_clear_fault(FaultCode_APPS_SENSOR_DISAGREEMENT);
    }
}

void double_pedal_check(float average_accel_pos, bool brake_on)
{
    DoublePedalSmInputs_s double_pedal_inputs = {(brake_on && (average_accel_pos > DOUBLE_PEDAL_APS_THRESHOLD)), average_accel_pos}; 
    DoublePedalSmOutputs_s double_pedal_outputs;

    double_pedal_state_machine(&double_pedal_inputs, &double_pedal_outputs);

    // increment state counter
    double_pedal_state_counter_ms += 10;

    // determine outputs
    switch (double_pedal_state)
    {
        case AppsDoublePedalState_NOT_ACTIVE:
        case AppsDoublePedaltate_DOUBLE_PEDAL:
            double_pedal_outputs.double_pedal_fault = false;
            break;
        case AppsDoublePedalState_DOUBLE_PEDAL_FAULTED:
            double_pedal_outputs.double_pedal_fault = true;
            break;
    }

    // apply outputs
    if (double_pedal_outputs.double_pedal_fault)
    {
        if (!FaultManager_is_fault_active(FaultCode_APPS_DOUBLE_PEDAL))
        {
            // we should not continuously set faults to avoid spamming alert message
            FaultManager_set_fault_active(FaultCode_APPS_DOUBLE_PEDAL);
        }
    }
    else
    {
        FaultManager_clear_fault(FaultCode_APPS_DOUBLE_PEDAL);
    }
}


void APPS_100Hz(AccelPos_s* accel_pos, bool brake_on, bool is_accelerator_rational)
{
    pedal_disagreement_check(accel_pos, is_accelerator_rational);
    double_pedal_check(accel_pos->average, brake_on);
}