#include "Brake.h"

#include <stdio.h>

#include "CAN.h"
#include "common_macros.h"
#include "Config.h"
#include "FaultManager.h"
#include "HAL_Aio.h"

// #define BRAKE_DEBUG

bool Brake_read_pressure(float* pressure)
{
    // read the analog input line
    uint16_t adc_val = HAL_Aio_read(AIOpin_BRAKE_PRESSURE);
    
    // convert the analog input line to sensor voltage
    // This is different than the adc input (3v3) but is scaled down to 3v3 so this should work
    float voltage = ((float) adc_val / ADC_MAX_VAL) * BPS_MAX_V;
    
    // get rid of offset
    float voltage_normalized = voltage - BPS_MIN_V;
    
    // get percentage of pressure range indicated by the voltage
    float pres_prog = voltage_normalized / BPS_RANGE_V;
    
    // convert to PSI
    float psi = BPS_MIN_PSI + pres_prog * BPS_RANGE_PSI;

    // saturate psi to possible range (but not rational range)
    psi = MAX(psi, 0);
    
    // return psi
    *pressure = psi;

    // Now check for irrationality
    // We can do this based on voltage since there's a smaller range than the 

    #ifdef BRAKE_DEBUG
    printf("adc_reading: %d voltage: %f\r\n", adc_val, voltage);
    printf("Brake Pressure: %f\r\n", *pressure);
    #endif

    // update the CAN message
    can_bus.vc_pedal_inputs.vc_pedal_inputs_brake_pressure = main_bus_vc_pedal_inputs_vc_pedal_inputs_brake_pressure_encode(*pressure);


    // this gon flicker
    if (FLOAT_LT(voltage, BPS_MIN_V, VOLTAGE_TOL) || FLOAT_GT(voltage, (BPS_MIN_V + BPS_RANGE_V), VOLTAGE_TOL))
    {
        FaultManager_set_fault_active(FaultCode_BRAKE_SENSOR_IRRATIONAL);
        return false;
    }
    else
    {
        FaultManager_clear_fault(FaultCode_BRAKE_SENSOR_IRRATIONAL);
        return true;
    }

}