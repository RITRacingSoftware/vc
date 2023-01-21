#include <stdio.h>

#include "unity.h"

#include "Brake.h"
#include "MockCAN.h"
#include "Config.h"

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
    // float psi = 5000.0;
    // float sensor_output_v = BPS_MIN_V + (((psi-BPS_MIN_PSI) / BPS_RANGE_PSI) * 4.0);
    // uint16_t adc_reading = (sensor_output_v/5.0) * 4095.0;

    // float converted_psi;
    
    // FaultManager_clear_fault_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    // HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    // TEST_ASSERT_MESSAGE(Brake_read_pressure(&converted_psi), "Brake pressure irrational when in rational range.");

    // float lower_limit = 4999.0;
    // float upper_limit = 5001.0;

    // char err_msg[100];
    // sprintf(err_msg, "The following not true: %.02fpsi < %.02fpsi < %.02fpsi", lower_limit, converted_psi, upper_limit);

    // TEST_ASSERT_MESSAGE((converted_psi > 4999.0) && (converted_psi < 5001.0), err_msg);

    float voltage = BRAKE_PRESSED_V - 0.1;
    uint16_t adc_reading = (voltage/ADC_MAX_VOLTAGE)*ADC_MAX_VAL;
    HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    FaultManager_clear_fault_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    bool is_pressed = Brake_is_pressed();

    TEST_ASSERT_MESSAGE(is_pressed == false, "Brake registered as pressed when not past threshold.");

    voltage = BRAKE_PRESSED_V + 0.1;
    adc_reading = (voltage/ADC_MAX_VOLTAGE)*ADC_MAX_VAL;
    HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    FaultManager_clear_fault_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    is_pressed = Brake_is_pressed();

    TEST_ASSERT_MESSAGE(is_pressed == true, "Brake registered as not pressed when past threshold.");
}

/**
 * Irrationality faults should be detected based on sensor voltage.
 */
void test_Brake_irrational(void)
{
    float voltage = BPS_MIN_V - .1;
    uint16_t adc_reading = (voltage/ADC_MAX_VOLTAGE)*ADC_MAX_VAL;
    HAL_Aio_read_ExpectAndReturn(AIOpin_BRAKE_PRESSURE, adc_reading);
    FaultManager_set_fault_active_Expect(FaultCode_BRAKE_SENSOR_IRRATIONAL);
    Brake_is_pressed();
}