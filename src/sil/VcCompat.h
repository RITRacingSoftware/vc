#ifndef VC_COMPAT_H
#define VC_COMPAT_H
#include "SILconfig.h"

#include "HAL_Aio.h"
#include "HAL_Dio.h"
#include "CAN.h"
#include "CanQueue.h"

/**
 * Static memory simulating hardware states.
 * ADC readings and digital input/output states are stored here.
 */

typedef struct {
    // analog and digital input pin state arrays are indexed using enum values
    float aio[AIOpin_NUM];
    bool dio[DIOpin_NUM];
    CanQueue_s outbox;
} VcHardware_s;

extern VcHardware_s hardware;

void VcCompat_init(void);

#endif // VC_COMPAT_H