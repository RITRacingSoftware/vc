#include "unity.h"

#include "VehicleState.h"

#include "MockFaultManager.h"
#include "MockMotorController.h"
#include "MockSoundController.h"

void setUp(void)
{
    VehicleState_init();
}

/**
 * Torque shouldnt be allowed until there's no faults and the motor controller is ready.
 */
void test_VehicleState_disallow_torque_until_ready(void)
{
    // make sure no torque is allowed from the get go
    for (int i = 0; i < 100; i++)
    {
        FaultManager_is_any_fault_active_ExpectAndReturn(false);
        MotorController_is_ready_ExpectAndReturn(false);
        VehicleState_100Hz();
        TEST_ASSERT(!VehicleState_allow_torque());
    }

    FaultManager_is_any_fault_active_ExpectAndReturn(false);
    MotorController_is_ready_ExpectAndReturn(true);
    // should play sound this iteration
    SoundController_play_sound_Expect(Sounds_READY_TO_DRIVE);
    VehicleState_100Hz();

    // go through startup process, make sure torque gets commanded after
    for (int i = 0; i < 10; i++)
    {
        FaultManager_is_any_fault_active_ExpectAndReturn(false);
        MotorController_is_ready_ExpectAndReturn(true);
        VehicleState_100Hz();
    }

    TEST_ASSERT(VehicleState_allow_torque());
}

/**
 * Torque shouldnt be allowed if the system faults during a drive.
 */
void test_VehicleState_disallow_torque_when_faulted(void)
{
    // get into ready state
    FaultManager_is_any_fault_active_ExpectAndReturn(false);
    MotorController_is_ready_ExpectAndReturn(true);
    // should play sound this iteration
    SoundController_play_sound_Expect(Sounds_READY_TO_DRIVE);
    VehicleState_100Hz();

    // go through startup process, make sure torque gets commanded after
    for (int i = 0; i < 10; i++)
    {
        FaultManager_is_any_fault_active_ExpectAndReturn(false);
        MotorController_is_ready_ExpectAndReturn(true);
        VehicleState_100Hz();
    }

    TEST_ASSERT(VehicleState_allow_torque());

    // now fault
    FaultManager_is_any_fault_active_ExpectAndReturn(true);
    MotorController_is_ready_ExpectAndReturn(true);
    VehicleState_100Hz();

    // expect no torque allowed
    TEST_ASSERT(!VehicleState_allow_torque());
}

/**
 * Torque shouldnt be allowed if the MC becomes not ready during a drive.
 */
void test_VehicleState_disallow_torque_when_mc_not_ready(void)
{
    // get into ready state
    FaultManager_is_any_fault_active_ExpectAndReturn(false);
    MotorController_is_ready_ExpectAndReturn(true);
    // should play sound this iteration
    SoundController_play_sound_Expect(Sounds_READY_TO_DRIVE);
    VehicleState_100Hz();

    // go through startup process, make sure torque gets commanded after
    for (int i = 0; i < 10; i++)
    {
        FaultManager_is_any_fault_active_ExpectAndReturn(false);
        MotorController_is_ready_ExpectAndReturn(true);
        VehicleState_100Hz();
    }

    TEST_ASSERT(VehicleState_allow_torque());

    // now fault
    FaultManager_is_any_fault_active_ExpectAndReturn(false);
    MotorController_is_ready_ExpectAndReturn(false);
    VehicleState_100Hz();

    // expect no torque allowed
    TEST_ASSERT(!VehicleState_allow_torque());
}