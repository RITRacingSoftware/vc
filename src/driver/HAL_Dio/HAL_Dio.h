#ifndef HAL_DIO_H
#define HAL_DIO_H

#include <stdbool.h>

typedef enum {
    DIOpin_SOUND_0,
} DIOpin_e;

bool HAL_Dio_read(DIOpin_e pin);
bool HAL_Dio_write(DIOpin_e pin, bool val);

#endif // HAL_DIO_H