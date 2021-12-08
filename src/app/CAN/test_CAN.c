#include "unity.h"

#include "CAN.h"

void setUp(void)
{
    CAN_init();
}


void test_CAN_counting(void)
{
    int id = 0x69;

    // begin counting id
    CAN_begin_counting_id(id);

    // verify we begin at 0
    TEST_ASSERT_MESSAGE(CAN_get_count_for_id(id) == 0, "Count did not start at 0.");

    for (int i = 0; i < 101; i++)
    {
        CanMessage_s msg;
        msg.id = id;
        CAN_add_message_rx_queue(msg.id, msg.dlc, (uint8_t*)&msg.data);
        CAN_process_recieved_messages();
    }

    TEST_ASSERT_MESSAGE(CAN_get_count_for_id(id) == 101, "Count did not increment appropriately.");

    // now receive a bunch of other messages and see if it messes up the counting
    for (int i = 0; i < 101; i++)
    {
        CanMessage_s msg;
        msg.id = i;
        if (i != id)
        {
            CAN_add_message_rx_queue(msg.id, msg.dlc, (uint8_t*)&msg.data);
            CAN_process_recieved_messages();
        }
        msg.id = id;
        CAN_add_message_rx_queue(msg.id, msg.dlc, (uint8_t*)&msg.data);
        CAN_process_recieved_messages();
    }
    TEST_ASSERT_MESSAGE(CAN_get_count_for_id(id) == 202, "Count did not increment appropriately when amidst other messages.");
}

// no need tbh