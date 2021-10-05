#include "CanQueue.h"

void CanQueue_init(CanQueue_s* queue)
{
    queue->next_in = 0;
    queue->next_read = 0;
    queue->count = 0;
}

bool CanQueue_enqueue(CanQueue_s* queue, CanMessage_s* msg)
{
    if (queue->count < CAN_QUEUE_NUM_MSGS)
    {
        queue->msgs[queue->next_in] = *msg;
        queue->next_in = (queue->next_in + 1) % CAN_QUEUE_NUM_MSGS;
        queue->count++;
        return true;
    }
    else
    {
        return false;
    }
}

bool CanQueue_dequeue(CanQueue_s* queue, CanMessage_s* msg)
{
    if (queue->count > 0)
    {
        *msg = queue->msgs[queue->next_read];
        queue->next_read = (queue->next_read + 1) % CAN_QUEUE_NUM_MSGS;
        queue->count--;
        return true;
    }
    else
    {
        return false;
    }
}