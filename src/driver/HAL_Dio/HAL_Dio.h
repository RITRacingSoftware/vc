#ifndef HAL_DIO_H
#define HAL_DIO_H

#include <stdbool.h>

typedef enum {
    DIOpin_SOUND_0,
    DIOpin_SOUND_1,
    DIOpin_SOUND_2,
    DIOpin_SOUND_3,
    DIOpin_SOUND_4,
    DIOpin_SOUND_5,
    DIOpin_SOUND_6,
    DIOpin_STATUS_LED,
} DIOpin_e;

bool HAL_Dio_read(DIOpin_e pin);
bool HAL_Dio_write(DIOpin_e pin, bool val);

#endif // HAL_DIO_H