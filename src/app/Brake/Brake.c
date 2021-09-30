#include "Brake.h"
#include "Config.h"
#include "FaultManager.h"
#include "HAL_Aio.h"

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
    
    // return psi
    *pressure = psi;

    // Now check for irrationality
    // We can do this based on voltage since there's a smaller range than the 

    // this gon flicker
    if ((voltage < BPS_MIN_V) || (voltage > (BPS_MIN_V + BPS_RANGE_V)))
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