#ifndef VC_CONFIG_H
#define VC_CONFIG_H

#define CAN_TX_QUEUE_LEN 15

/**
 * Faults
 */

// any bits set in this won't get set in the fault vector
#define DISABLE_FAULT_MASK 0

#endif // VC_CONFIG_H