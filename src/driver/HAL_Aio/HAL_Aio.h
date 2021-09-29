#ifndef HAL_AIO
#define HAL_AIO

#include <stdint.h>

typedef enum {
    AIOpin_BRAKE_PRESSURE,
    AIOpin_ACCEL_A,
    AIOpin_ACCEL_B
} AIOpin_e;

void HAL_Aio_init(void);
uint16_t HAL_Aio_read(AIOpin_e);

#endif // HAL_AIO