#include "HAL_Aio.h"
#include "VcCompat.h"

void HAL_Aio_init(void)
{
    // nothing required!
}

uint16_t HAL_Aio_read(AIOpin_e pin)
{
    return (hardware.aio[pin] / ADC_MAX_VAL) * ADC_MAX_V;
}