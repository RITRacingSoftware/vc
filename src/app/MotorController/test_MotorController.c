#include "unity.h"

#include "MotorController.h"

#include "MockCAN.h"

CAN_BUS can_bus;

void setUp(void)
{
    CAN_begin_counting_id_Ignore();
    MotorController_init();
}

// helper to avoid using this long function name each time
float get_commanded_torque(void)
{
    main_bus_m192_command_message_torque_command_decode(can_bus.mc_command.torque_command);
}

void test_MotorController_ready_sequence(void)
{
    // simulate a pedal press that is looking to apply torque too early
    MotorController_set_torque(100);

    // motor controller begins not ready
    can_bus.mc_state.d1_vsm_state = 2;

    // vc shouldnt command torque before MC status messages are seen.
    // Enable bit should be low to unlock MC once it starts up.
    for (int i = 0; i < 100; i++)
    {
        // no MC status messages received
        CAN_get_count_for_id_ExpectAnyArgsAndReturn(0);
        // should start out sending command messages so that the MC doesn't immediately time out on power on
        CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
        MotorController_100Hz();
        TEST_ASSERT_MESSAGE(get_commanded_torque() == 0, "vc should start with no torque commanded");   
    }

    // a transition should be seen into the DISABLED state upon receiving a MC status message
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(1);
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    MotorController_100Hz();
    // torque should still be 0
    TEST_ASSERT_MESSAGE(get_commanded_torque() == 0, "vc should start with no torque commanded");   
    // enable bit should flip low to unlock MC
    TEST_ASSERT_MESSAGE(can_bus.mc_command.inverter_enable == 0, "VC should flip enable bit low upon receiving a message from the MC.");

    // now run again, the enable bit should flip high to enable MC
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(1);
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    MotorController_100Hz();

    // torque should still be 0
    TEST_ASSERT_MESSAGE(get_commanded_torque() == 0, "vc should start with no torque commanded");   
    // enable bit should flip low to unlock MC
    TEST_ASSERT_MESSAGE(can_bus.mc_command.inverter_enable == 1, "VC should flip enable bit high to enable MC after unlocking it.");

    // now simulate a MC status message that indicates the MC has been enabled
    // note that it is still not in the ready state at this point
    CAN_get_count_for_id_ExpectAnyArgsAndReturn(2);
    can_bus.mc_state.d6_inverter_enable_state = 1;
    CAN_send_message_Expect(MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID);
    MotorController_100Hz();

    TEST_ASSERT_MESSAGE(get_commanded_torque() == 0, "vc should not request torque before MC is ready");
    TEST_ASSERT_MESSAGE(can_bus.mc_command.inverter_enable == 1, "Enabled bit should be held high now");
}