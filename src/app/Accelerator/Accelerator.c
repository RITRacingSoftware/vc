#include "Accelerator.h"
#include "CAN.h"
#include "common_macros.h"
#include "Config.h"
#include "FaultManager.h"
#include "HAL_Aio.h"
#include <stdio.h>

// #define ACCEL_DEBUG

bool Accelerator_read_positions(AccelPos_s* accel_pos)
{
    #ifdef ACCEL_DEBUG
    printf("ADC: %d %d\r\n", HAL_Aio_read(AIOpin_ACCEL_A), HAL_Aio_read(AIOpin_ACCEL_B));
    #endif
    uint16_t adc_a = HAL_Aio_read(AIOpin_ACCEL_A);
    uint16_t adc_b = HAL_Aio_read(AIOpin_ACCEL_B);

    float voltage_a = ((float) adc_a / ADC_MAX_VAL) * ADC_MAX_VOLTAGE;
    float voltage_b = ((float) adc_b / ADC_MAX_VAL) * ADC_MAX_VOLTAGE;

    can_bus.vc_pedal_inputs_raw.vc_pedal_inputs_raw_accel_voltage_a = main_bus_vc_pedal_inputs_raw_vc_pedal_inputs_raw_accel_voltage_a_encode(voltage_a);
    can_bus.vc_pedal_inputs_raw.vc_pedal_inputs_raw_accel_voltage_b = main_bus_vc_pedal_inputs_raw_vc_pedal_inputs_raw_accel_voltage_b_encode(voltage_b);

    #ifdef ACCEL_DEBUG
    printf("VOLTAGE: %.02f %.02f\r\n", voltage_a, voltage_b);
    #endif 
    accel_pos->a = ((voltage_a -APS_A_MIN_RATIONAL_V) / APS_A_RANGE_V) * 100.0;
    accel_pos->b = ((voltage_b -APS_B_MIN_RATIONAL_V) / APS_B_RANGE_V) * 100.0;

    // limit positions to from 0-100
    accel_pos->a = SAT(accel_pos->a, 0, 100);
    accel_pos->b = SAT(accel_pos->b, 0, 100);

    accel_pos->average = (accel_pos->a + accel_pos->b) / 2.0;
    
    // check for irrationality
    // this gon flicker
    
    // if (FLOAT_LT(voltage_a, APS_A_MIN_RATIONAL_V, VOLTAGE_TOL) || FLOAT_GT(voltage_a, APS_A_MAX_RATIONAL_V, VOLTAGE_TOL)
    //     || FLOAT_LT(voltage_b, APS_B_MIN_RATIONAL_V, VOLTAGE_TOL) || FLOAT_GT(voltage_b, APS_B_MAX_RATIONAL_V, VOLTAGE_TOL))
    // {
    //     #ifdef ACCEL_DEBUG
    //     printf("Setting irrational fault.\r\n");
    //     #endif
    //     FaultManager_set_fault_active(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL);
    // }
    // else
    // {
    //     FaultManager_clear_fault(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL);
    // }
    #ifdef ACCEL_DEBUG
    printf("ACCEL: %.02f %.02f\r\n", accel_pos->a, accel_pos->b);
    #endif
    // update CAN message
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_a = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_a_encode(accel_pos->a);
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_b = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_b_encode(accel_pos->b);
    can_bus.vc_pedal_inputs.vc_pedal_inputs_accel_position_avg = main_bus_vc_pedal_inputs_vc_pedal_inputs_accel_position_avg_encode(accel_pos->average);
}