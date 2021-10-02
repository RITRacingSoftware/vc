#include "Accelerator.h"
#include "CAN.h"
#include "Config.h"
#include "FaultManager.h"
#include "HAL_Aio.h"

bool Accelerator_read_positions(AccelPos_s* accel_pos)
{
    float voltage_a = (HAL_Aio_read(AIOpin_ACCEL_A) / ADC_MAX_VAL) * ADC_MAX_V;
    float voltage_b = (HAL_Aio_read(AIOpin_ACCEL_B) / ADC_MAX_VAL) * ADC_MAX_V;

    accel_pos->a = ((voltage_a -APS_A_MIN_RATIONAL_V) / APS_A_RANGE_V) * 100.0;
    accel_pos->b = ((voltage_b -APS_B_MIN_RATIONAL_V) / APS_B_RANGE_V) * 100.0;

    accel_pos->average = (accel_pos->a + accel_pos->b) / 2.0;
    
    // check for irrationality
    // this gon flicker
    if ((voltage_a < APS_A_MIN_RATIONAL_V) || (voltage_a > APS_A_MAX_RATIONAL_V)
        || (voltage_b < APS_B_MIN_RATIONAL_V) || (voltage_b > APS_B_MAX_RATIONAL_V))
    {
        FaultManager_set_fault_active(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL);
    }
    else
    {
        FaultManager_clear_fault(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL);
    }

    // update CAN message
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_a = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_a_encode(accel_pos->a);
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_b = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_b_encode(accel_pos->b);
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_avg = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_avg_encode(accel_pos->average);
}