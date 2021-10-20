#include <stdio.h>

#include "HAL_Dio.h"
#include "VcCompat.h"


bool HAL_Dio_read(DIOpin_e pin)
{
    return hardware.dio[pin];
}

void HAL_Dio_write(DIOpin_e pin, bool val)
{
    if (pin == DIOpin_STATUS_LED)
    {
        hardware.dio[pin] = val;
    }
}