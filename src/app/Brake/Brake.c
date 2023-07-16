#include "Brake.h"

#include <stdio.h>

#include "CAN.h"
#include "common_macros.h"
#include "Config.h"
#include "FaultManager.h"
#include "HAL_Aio.h"

// #define BRAKE_DEBUG

bool Brake_is_pressed(void)
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
    can_bus.vc_pedal_inputs_raw.vc_pedal_inputs_raw_brake_voltage = formula_main_dbc_vc_pedal_inputs_raw_vc_pedal_inputs_raw_brake_voltage_encode(voltage);
    
    // this gon flicker
    // Only set a fualt if the brake is irrational for a number of samples in a row
    uint8_t irrational_count = 0;
    if (FLOAT_LT(voltage, BPS_MIN_V, VOLTAGE_TOL))
    {
        irrational_count += 1;
    }
    else if(FLOAT_GT(voltage, BPS_IRRATIONAL_V, VOLTAGE_TOL))
    {
        irrational_count += 1;
    }
    else
    {
        irrational_count = 0;
    }

    // Set fault if irrational, clear it if not
    if(irrational_count > MAX_BRAKE_IRRATIONAL_COUNT)
    {
        if(!FaultManager_is_fault_active(FaultCode_BRAKE_SENSOR_IRRATIONAL))
        {
            FaultManager_set_fault_active(FaultCode_BRAKE_SENSOR_IRRATIONAL);
        }
    }
    else
    {
        FaultManager_clear_fault(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    }

    return FLOAT_GT(voltage, BRAKE_PRESSED_V, VOLTAGE_TOL);
}