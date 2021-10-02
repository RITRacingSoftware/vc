#include "unity.h"

#include "TorqueLimiter.h"

#include "Config.h"

#include "MockVehicleState.h"

void setUp(void)
{
    // hi
}

/**
 * No torque should be allowed when the vehicle state is not ready to drive.
 */
void test_TorqueLimiter_0_torque_when_not_allowed(void)
{
    VehicleState_allow_torque_ExpectAndReturn(false);
    float limited_torque = TorqueLimiter_apply_limit(100);

    char err_msg[100];
    sprintf(err_msg, "Torque was expected to be limited to 0, instead was limited to %f Nm.", limited_torque);
    TEST_ASSERT_MESSAGE(limited_torque == 0, err_msg);
}

/**
 * Under no circumstances should torque above the maximum limit be commanded.
 */
void test_TorqueLimiter_max_torque(void)
{
    VehicleState_allow_torque_ExpectAndReturn(true);
    float limited_torque = TorqueLimiter_apply_limit(MAX_TORQUE_NM + 10);

    char err_msg[100];
    sprintf(err_msg, "Torque was expected to be limited to %.02f Nm, instead was limited to %.02f Nm.", MAX_TORQUE_NM, limited_torque);
    TEST_ASSERT_MESSAGE(limited_torque <= MAX_TORQUE_NM, err_msg);
}

/**
 * Under no circumstances should negative torque be commanded.
 * This could lead to overflows with huge torques commanded.
 */
void test_TorqueLimiter_min_torque(void)
{
    VehicleState_allow_torque_ExpectAndReturn(true);
    float limited_torque = TorqueLimiter_apply_limit(-1);

    char err_msg[100];
    sprintf(err_msg, "Torque was expected to be limited to %.02f Nm, instead was limited to %.02f Nm.", 0.0, limited_torque);
    TEST_ASSERT_MESSAGE(limited_torque == 0, err_msg);
}