#ifndef CAN_QUEUE_H
#define CAN_QUEUE_H

#include "CAN.h"
#include "Config.h"

typedef struct {
    CanMessage_s msgs[CAN_QUEUE_NUM_MSGS];
    unsigned int next_in; // next index to put message
    unsigned int next_read; // next index to read message from
    unsigned int count; // how many messages current in queue
} CanQueue_s;

/**
 * Initialize internal pointers and data structures.
 * queue [in] - CanQueue to initialize;
 */
void CanQueue_init(CanQueue_s* queue);

/**
 * Add a CAN message to the queue if there's space.
 * queue [in] - the queue
 * msg [in] - the message
 * return true if message was added, false if there was no space left
 */
bool CanQueue_enqueue(CanQueue_s* queue, CanMessage_s* msg);

/**
 * Get the next in line CAN message (FIFO) if there are any.
 * queue [in] - queue
 * msg [out] - destination for read message
 * return true if there was a message to read, false if the queue was empty
 */
bool CanQueue_dequeue(CanQueue_s* queue, CanMessage_s* msg);

#endif // CAN_QUEUE_H