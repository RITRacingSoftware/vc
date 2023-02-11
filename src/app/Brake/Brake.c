#include "Brake.h"

#include <stdio.h>

#include "CAN.h"
#include "common_macros.h"
#include "Config.h"
#include "FaultManager.h"
#include "HAL_Aio.h"

// #define BRAKE_DEBUG

bool Brake_is_pressed(float voltage)
{
    // this gon flicker
    if (FLOAT_LT(voltage, BPS_MIN_V, VOLTAGE_TOL))
    {
        // FaultManager_set_fault_active(FaultCode_BRAKE_SENSOR_IRRATIONAL); //TODO: RE-enable
    }
    else
    {
        FaultManager_clear_fault(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    }

    return FLOAT_GT(voltage, BRAKE_PRESSED_V, VOLTAGE_TOL);
}

//Gives brake voltage
float Brake_Get_Voltage(void)
{
    // read the analog input line
    uint16_t adc_val = HAL_Aio_read(AIOpin_BRAKE_PRESSURE);
    
    // convert the analog input line to sensor voltage
    // This is different than the adc input (3v3) but is scaled down to 3v3 so this should work
    float voltage = ((float) adc_val / ADC_MAX_VAL) * BPS_MAX_V;

#ifdef BRAKE_DEBUG
    printf("voltage: %f\r\n", adc_val, voltage);
#endif
    // update CAN message
    can_bus.vc_pedal_inputs_raw.vc_pedal_inputs_raw_brake_voltage = main_bus_vc_pedal_inputs_raw_vc_pedal_inputs_raw_brake_voltage_encode(voltage);

    return voltage
}