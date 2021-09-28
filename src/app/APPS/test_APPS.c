#include <stdio.h>

#include "unity.h"

#include "APPS.h"
#include "config.h"

#include "MockFaultManager.h"

void setUp(void)
{   
    APPS_init();
}

/**
 * APPS should not produce any faults if neither pedal is pressed.
 */
void test_APPS_no_faults_idle(void)
{
    AccelPos_s accel_pos = {0,0};
    float brake_pos = 0;

    for (int i = 0; i < 1000; i++)
    {
        FaultManager_clear_fault_Ignore();
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
        APPS_100Hz(accel_pos, brake_pos);
    }
}

/**
 * APPS should fault if the accelerator pedal sensors read too far apart.
 */
void test_APPS_sensor_disagreement(void)
{
    AccelPos_s accel_pos = {0,0};
    float brake_pos = 0;

    // make sure we start out not faulted
    // If a fault is set, a cmock function that has not been expected will be called and the test will fail.
    for (int i = 0; i < 10; i++)
    {
        FaultManager_clear_fault_Ignore();
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
        APPS_100Hz(accel_pos, brake_pos);
    }

    // now set the pedal sensors too far apart and make sure a fault occurs at the right time
    accel_pos.a = 50;
    accel_pos.b = 61;
    for (int ms = 0; ms < 100; ms += 10)
    {
        FaultManager_clear_fault_Ignore();
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
        APPS_100Hz(accel_pos, brake_pos);
    }

    FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_SENSOR_DISAGREEMENT, false);
    FaultManager_set_fault_active_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
    FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
    APPS_100Hz(accel_pos, brake_pos);

    for (int i = 0; i < 10; i++)
    {
        // make sure the fault stays active
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_SENSOR_DISAGREEMENT, true); // pedal disagreement
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false); // double pedal
        APPS_100Hz(accel_pos, brake_pos);
    }

    // now get rid of the error condition and make sure the fault clears accordingly
    accel_pos.a = 10;
    accel_pos.b = 10;

    // fault should clear in 10 iterations
    for (int ms = 0; ms < 100; ms += 10)
    {
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_SENSOR_DISAGREEMENT, true); // pedal disagreement
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false); // double pedal
        APPS_100Hz(accel_pos, brake_pos);
    }

    FaultManager_clear_fault_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
    FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false); // double pedal
    APPS_100Hz(accel_pos, brake_pos);

}

/**
 * Double pedal faults should be thrown when both pedals are pressed accordingly
 * and cleared when the brake goes below the recovery threshold.
 */
void test_APPS_double_pedal(void)
{   
    AccelPos_s accel_pos = {0,0};
    float brake_pos = 0;

    // make sure we start out not faulted
    // If a fault is set, a cmock function that has not been expected will be called and the test will fail.
    for (int i = 0; i < 10; i++)
    {
        FaultManager_clear_fault_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
        APPS_100Hz(accel_pos, brake_pos);
    }

    // press the throttle, make sure there's still no faults
    accel_pos.a = 5;
    accel_pos.b = 5;
    accel_pos.average = 5;
    brake_pos = 0;

    for (int i = 0; i < 10; i++)
    {
        FaultManager_clear_fault_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
        APPS_100Hz(accel_pos, brake_pos);
    }

    // now press the brake, there should be a fault here
    brake_pos = 26;
    FaultManager_clear_fault_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
    FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, false);
    FaultManager_set_fault_active_Expect(FaultCode_APPS_DOUBLE_PEDAL);
    APPS_100Hz(accel_pos, brake_pos);

    // release the brake part of the way, the fault should hold
    brake_pos = 6;
    for (int i = 0; i < 100; i++)
    {
        FaultManager_clear_fault_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
        FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, true);
        APPS_100Hz(accel_pos, brake_pos);
    }

    // now release the brake all the way, the fault should clear
    brake_pos = 4;
    FaultManager_is_fault_active_ExpectAndReturn(FaultCode_APPS_DOUBLE_PEDAL, true);
    FaultManager_clear_fault_Expect(FaultCode_APPS_SENSOR_DISAGREEMENT);
    FaultManager_clear_fault_Expect(FaultCode_APPS_DOUBLE_PEDAL);
    APPS_100Hz(accel_pos, brake_pos);
}