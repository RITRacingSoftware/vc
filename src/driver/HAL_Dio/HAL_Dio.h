#ifndef HAL_DIO_H
#define HAL_DIO_H

#include <stdbool.h>

typedef enum {
    DIOpin_SOUND_1,
    DIOpin_SOUND_2,
    DIOpin_SOUND_3,
    DIOpin_SOUND_4,
    DIOpin_SOUND_5,
    DIOpin_SOUND_6,
    DIOpin_SWITCH_1,
    DIOpin_SWITCH_2,
    DIOpin_SWITCH_3,
    DIOpin_SWITCH_4,
    DIOpin_BMS_FAULT,
    DIOpin_IMD_FAULT,
    DIOpin_BSPD_FAULT,
    DIOpin_BSPD_SIGNAL_LOST,
    DIOpin_PRECHARGE,
    // DIOpin_IMD1_LATCH,
    // DIOpin_IMD2_INPUT,
    // DIOpin_IMD2_LATCH,
    DIOpin_STATUS_LED,
    DIOpin_NUM
} DIOpin_e;

/**
 * Set up GPIO pins used for digital and analog I/O.
 */
void HAL_Dio_init(void);

/**
 * Read the binary state of a digital input pin.
 * pin [in] - pin used for digital input
 */
bool HAL_Dio_read(DIOpin_e pin);

/**
 * Write a binary state to a digital output pin.
 * pin [in] - pin used for digital output to write to
 * val [in] - true for logic high, false for logic low
 */
void HAL_Dio_write(DIOpin_e pin, bool val);

#endif // HAL_DIO_H