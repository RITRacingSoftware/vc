#include "VcCompat.h"
#include "Config.h"

VcHardware_s hardware;

void VcCompat_init(void)
{
    // Analog I/O initialization:
    // zero accelerator input
    hardware.aio[AIOpin_ACCEL_A] =  APS_A_MIN_RATIONAL_V;
    hardware.aio[AIOpin_ACCEL_B] = APS_B_MIN_RATIONAL_V;
    // zero brake press
    hardware.aio[AIOpin_BRAKE_PRESSURE] = BPS_MIN_V;
    
    // Digital I/O initialization:
    // start all sounds not triggered
    hardware.dio[DIOpin_SOUND_0] = 
    hardware.dio[DIOpin_SOUND_1] = 
    hardware.dio[DIOpin_SOUND_2] = 
    hardware.dio[DIOpin_SOUND_3] =
    hardware.dio[DIOpin_SOUND_4] =
    hardware.dio[DIOpin_SOUND_5] =
    hardware.dio[DIOpin_SOUND_6] = true;
    
    // start status LED low
    hardware.dio[DIOpin_STATUS_LED] = false;

    // start all switches low
    hardware.dio[DIOpin_SWITCH_0] =
    hardware.dio[DIOpin_SWITCH_1] =
    hardware.dio[DIOpin_SWITCH_2] =
    hardware.dio[DIOpin_SWITCH_3] = false;

    CanQueue_init(&hardware.outbox);
}