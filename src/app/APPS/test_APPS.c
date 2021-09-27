#include <stdio.h>

#include "unity.h"

#include "APPS.h"
#include "config.h"

#include "MockFaultManager.h"

void setUp(void)
{
    // lol
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
        FaultManager_is_fault_active_IgnoreAndReturn(false);
        APPS_100Hz(accel_pos, brake_pos);
    }
}

/**
 * APPS should fault if the accelerator pedal sensors read too far apart.
 */

