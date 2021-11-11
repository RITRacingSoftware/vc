#ifndef HAL_AIO
#define HAL_AIO

#include <stdint.h>

typedef enum {
    AIOpin_BRAKE_PRESSURE,
    AIOpin_ACCEL_A,
    AIOpin_ACCEL_B,
    AIOpin_NUM // Automatically is the value of the number of pins in the enum
} AIOpin_e;

/**
 * Initialize ADC peripheral and other relevant peripherals for synchronous 12 bit reads.
 */
void HAL_Aio_init(void);

/**
 * Read the current voltage at the provided adc pin and report it using 12 bit granularity.
 * pin - [in] provided adc pin
 * return 0-4095
 */
uint16_t HAL_Aio_read(AIOpin_e pin);

#endif // HAL_AIO