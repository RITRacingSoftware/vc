#include <string.h>
#include "unity.h"
#include "Config.h"

#include "common_macros.h"

#include "MotorController.h"
#include "Config.h"

#include "MockHAL_Dio.h"
#include "MockCAN.h"
#include "MockBrake.h"

CAN_BUS can_bus;

void setUp(void)
{
    CAN_begin_counting_id_Ignore();
    MotorController_init();
    memset(&can_bus, 0, sizeof(CAN_BUS));
}

// helper to avoid using this long function name each time
float get_commanded_torque(void)
{
    return main_bus_m192_command_message_torque_command_decode(can_bus.mc_command.torque_command);
}

void test_MotorController_ready_sequence(void)
{
    // motor controller begins not ready
    can_bus.mc_state.d1_vsm_state = main_bus_m170_internal_states_d1_vsm_state_encode(2.0);

    // vc shouldnt command torque before MC status messages are seen.
    // Enable bit should be low to unlock MC once it starts up.
    for (int i = 0; i < 100; i++)
    {
        // no MC status messages received
        CAN_get_count_for_id_ExpectAnyArgsAndReturn(0);
        // should start out sending command messages so that the MC doesn't immediately time out on power on
        CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
        Brake_is_pressed_ExpectAndReturn(false); // dont press brake
        HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, false); // dont press start button
        MotorController_100Hz();
        TEST_ASSERT_MESSAGE(!MotorController_is_ready(), "MC is not ready at startup");   
    }

    // a transition should be seen into the DISABLED state upon receiving a MC status message
    
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(1);
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    Brake_is_pressed_ExpectAndReturn(true); // press brake
    HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
    MotorController_100Hz();
    
    // mc still not ready yet
    TEST_ASSERT(!MotorController_is_ready());   
    // enable bit should flip low to unlock MC
    TEST_ASSERT_MESSAGE(can_bus.mc_command.inverter_enable == 0, "VC should flip enable bit low upon receiving a message from the MC.");

    // run one more iteration
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(1);
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    Brake_is_pressed_ExpectAndReturn(true); // press brake
    HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
    MotorController_100Hz();

    // now run again, the enable bit should flip high to enable MC
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(1);
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    Brake_is_pressed_ExpectAndReturn(true); // press brake
    HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
    MotorController_100Hz();

    // mc should still not be deemed ready
    TEST_ASSERT(!MotorController_is_ready());   
    // enable bit should flip low to unlock MC
    TEST_ASSERT_MESSAGE(can_bus.mc_command.inverter_enable == 1, "VC should flip enable bit high to enable MC after unlocking it.");

    // now simulate a MC status message that indicates the MC has been enabled
    // note that it is still not in the ready state at this point
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(2);
    can_bus.mc_state.d6_inverter_enable_state = 1;
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    Brake_is_pressed_ExpectAndReturn(true); // press brake
    HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
    MotorController_100Hz();

    TEST_ASSERT(!MotorController_is_ready());
    TEST_ASSERT_MESSAGE(can_bus.mc_command.inverter_enable == 1, "Enabled bit should be held high now");

    // run for a bit while MC is enabled but not in the ready state and ensure no torque is requested yet
    int can_count = 2;
    for (int i = 0; i < 100; i++)
    {
        if (i % 10)
        {
            // send a CAN message so the VC knows the MC is still powered on
            can_count++;       
        }

        CAN_get_count_for_id_ExpectAnyArgsAndReturn(can_count);
        CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
        Brake_is_pressed_ExpectAndReturn(true); // press brake
    HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
        MotorController_100Hz();

        TEST_ASSERT(!MotorController_is_ready());
    }

    // indicate the motor controller is ready. Make sure the vc starts requesting torque afterwards.
    can_bus.mc_state.d1_vsm_state = main_bus_m170_internal_states_d1_vsm_state_encode(5); // 5 is VSM_READY
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(can_count);
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    Brake_is_pressed_ExpectAndReturn(true); // press brake
    HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
    MotorController_100Hz();

    TEST_ASSERT(MotorController_is_ready());
}

/**
 * The VC should stop commanding torque if the motor controller goes mia.
 */
void test_MotorController_disconnect(void)
{
    // setup the motor controller ready condition
    can_bus.mc_state.d1_vsm_state = main_bus_m170_internal_states_d1_vsm_state_encode(5); // 5 is VSM_READY
    can_bus.mc_state.d6_inverter_enable_state = 1; // inverter enabled

    int can_count = 0;
    float commanded_torque = 100;
    MotorController_set_torque(commanded_torque);
    for (int i = 0; i < 10; i++)
    {
        CAN_get_count_for_id_ExpectAnyArgsAndReturn(can_count++);
        CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
        Brake_is_pressed_ExpectAndReturn(true); // press brake
        HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
        MotorController_100Hz();
    }

    TEST_ASSERT_MESSAGE(MotorController_is_ready(), "Motor Controller was never ready");

    // now stop sending MC status CAN messages (keep can_count steady)
    for (int i = 0; i < MC_CAN_TIMEOUT_MS/10 + 10; i++)
    {
        CAN_get_count_for_id_ExpectAnyArgsAndReturn(can_count);
        CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
        Brake_is_pressed_ExpectAndReturn(true); // press brake
        HAL_Dio_read_ExpectAndReturn(DIOpin_RTD_BUTTON, true); // press start button
        MotorController_100Hz();
    }

    TEST_ASSERT_MESSAGE(!MotorController_is_ready(), "VC kept MC status as ready after MC CAN timeout.");
}