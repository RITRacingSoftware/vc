#include <stdio.h>

#include "unity.h"

#include "Brake.h"
#include "MockCAN.h"

#include "MockFaultManager.h"
#include "MockHAL_Aio.h"

CAN_BUS can_bus;

void setUp (void)
{
    // lol
}

/**
 * Make sure the brake sensor adc values are properly converted to brake pressures.
 */ 
void test_Brake_convert(void)
{
    // Doing the math ahead of time to see what value should be returned.
    float psi = 5000.0;
    float sensor_output_v = .5 + (((psi-50.0) / 7950.0) * 4.0);
    uint16_t adc_reading = (sensor_output_v/5.0) * 4095.0;

    float converted_psi;
    
    FaultManager_clear_fault_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    TEST_ASSERT_MESSAGE(Brake_read_pressure(&converted_psi), "Brake pressure irrational when in rational range.");

    float lower_limit = 4999.0;
    float upper_limit = 5001.0;

    char err_msg[100];
    sprintf(err_msg, "The following not true: %.02fpsi < %.02fpsi < %.02fpsi", lower_limit, converted_psi, upper_limit);

    TEST_ASSERT_MESSAGE((converted_psi > 4999.0) && (converted_psi < 5001.0), err_msg);
}

/**
 * Irrationality faults should be detected based on sensor voltage.
 */
void test_Brake_irrational(void)
{
    float sensor_output_v = .45;
    uint16_t adc_reading = (sensor_output_v/5.0) * 4095.0;
    float converted_psi;
    FaultManager_set_fault_active_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    Brake_read_pressure(&converted_psi);

    sensor_output_v = 4.55;
    adc_reading = (sensor_output_v/5.0) * 4095.0;
    converted_psi;
    FaultManager_set_fault_active_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    Brake_read_pressure(&converted_psi);
}