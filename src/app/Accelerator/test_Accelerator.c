#include <stdio.h>
#include "Accelerator.h"
#include "MockHAL_Aio.h"
#include "MockFaultManager.h"
#include "MockCAN.h"

CAN_BUS can_bus;

void setUp(void){ }

/**
 * A calculated adc reading from an expected position should be read from the accelerator module
 * when that adc reading is injected in.
 */
void test_Accelerator_convert(void)
{
    // for the error check to work, these should be whole numbers
    float expected_pos_a = 76.0;
    float expected_pos_b = 44.0;

    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_A, (((expected_pos_a / 100.0) * APS_A_SENSOR_V)/ADC_MAX_VOLTAGE)*ADC_MAX_VAL);
    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_B, (((expected_pos_b / 100.0) * APS_B_SENSOR_V)/ADC_MAX_VOLTAGE)*ADC_MAX_VAL);
    FaultManager_clear_fault_Expect(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL);
    AccelPos_s pos;

    Accelerator_read_positions(&pos);

    char err[100];
    sprintf(err, "Accelerator read %.02f for position A instead of %.02f", pos.a, expected_pos_a);

    TEST_ASSERT_MESSAGE((pos.a > expected_pos_a-1) && (pos.a < expected_pos_a + 1), err);

    sprintf(err, "Accelerator read %.02f for position A instead of %.02f", pos.b, expected_pos_b);

    TEST_ASSERT_MESSAGE((pos.b > expected_pos_b-1) && (pos.b < expected_pos_b + 1), err);

    float expected_avg = (pos.a + pos.b) / 2.0;

    sprintf(err, "Accelerator read %.02f for average position instead of %.02f", pos.average, expected_avg);

    TEST_ASSERT_MESSAGE((pos.average > expected_avg-1) && (pos.average < expected_avg + 1), err);    
}

/**
 * The accelerator should fault upon seeing irrational accelerator position sensor readings.
 */
void test_Accelerator_irrational(void)
{
    float too_low_v = 0;
    float too_high_v =  3.3;
    float okay_v = 1.0;
    AccelPos_s pos;

    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_A, (too_high_v / 3.3) * 4095.0);
    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_B, (okay_v / 3.3) * 4095.0);
    FaultManager_set_fault_active_Expect(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL);
    Accelerator_read_positions(&pos);

    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_A, (okay_v / 3.3) * 4095.0);
    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_B, (too_high_v / 3.3) * 4095.0);
    FaultManager_set_fault_active_Expect(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL); 
    Accelerator_read_positions(&pos);

    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_A, (too_low_v / 3.3) * 4095.0);
    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_B, (okay_v / 3.3) * 4095.0);
    FaultManager_set_fault_active_Expect(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL); 
    Accelerator_read_positions(&pos);

    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_A, (okay_v / 3.3) * 4095.0);
    HAL_Aio_read_ExpectAndReturn(AIOpin_ACCEL_B, (too_low_v / 3.3) * 4095.0);
    FaultManager_set_fault_active_Expect(FaultCode_ACCELERATOR_SENSOR_IRRATIONAL); 
    Accelerator_read_positions(&pos);
}