#ifndef HAL_CAN_H
#define HAL_CAN_H
#include <stdint.h>

#include "FaultManager.h"
#include "CAN.h"
#include "semphr.h"

#ifndef VC_SIL
extern SemaphoreHandle_t can_message_transmit_semaphore;
#endif

/**
 * Initializes stm32 for CAN message transmissions, must be called befor a message is sent
 */
void HAL_Can_init(void);



/**
 * Transmits a CAN message over the main bus
 */
Error_t HAL_Can_send_message_main(uint32_t id, int dlc, uint64_t data);

/**
 * Transmits a CAN message over the sensor bus
 */
Error_t HAL_Can_send_message_sensor(uint32_t id, int dlc, uint64_t data);

/**
 * Returns the number of empty transmit mailboxes (max of three)
 */
uint8_t HAL_number_of_empty_mailboxes(void);

/**
 * Initializes a filter for the given 16 bit CAN ids on the main bus
 * Only ids that a filter has been initialized for will be recieved
 * Max 28 filters = 56 ids
 */
void HAL_Can_add_filter_main_standard(uint16_t id1, uint16_t id2);

/**
 * Initializes a filter for the given 32 bit CAN ids on the main bus
 * Only ids that a filter has been initialized for will be recieved
 * Max 8 filters = 16 ids
 */
void HAL_Can_add_filter_main_extended(uint32_t id, uint32_t id2);

/**
 * Initializes a filter for the given 16 bit CAN ids on the main bus
 * Only ids that a filter has been initialized for will be recieved
 * Max 28 filters = 56 ids
 */
void HAL_Can_add_filter_sensor_standard(uint16_t id1, uint16_t id2);

/**
 * Initializes a filter for the given 32 bit CAN ids on the main bus
 * Only ids that a filter has been initialized for will be recieved
 * Max 8 filters = 16 ids
 */
void HAL_Can_add_filter_sensor_extended(uint32_t id1, uint32_t id2);



#endif // HAL_CAN_H