#include "VcEcu.h"


// no extern "C" here since we're compiling these using g++
#include "VC.h"
#include "CAN.h"
#include "VcCompat.h"
#include "ShutdownMonitor.h"

using namespace ecusim;

VcEcu::VcEcu(void)
{
    // initialize expected hardware states
    VcCompat_init();
    VC_init();

    shutdown_status.vc_shutdown_status_bms_fault = 1;
    shutdown_status.vc_shutdown_status_imd_fault = 1;
    shutdown_status.vc_shutdown_status_bspd_fault = 1;
    shutdown_status.vc_shutdown_status_bspd_signal_lost = 1;
    shutdown_status.vc_shutdown_status_precharge = 1;
}

void VcEcu::tick(void)
{
    static int time_ms = 0;
    
    if (can_data_available)
    {
        CAN_process_recieved_messages();
    }

    if (time_ms % 10 == 0)
    {
        VC_100Hz();
    }

    CAN_send_queued_messages();

    time_ms = (time_ms + 1) % 10;

    CanMessage_s msg;
    while(CanQueue_dequeue(&hardware.outbox, &msg))
    {
        CanMsg msg_out;
        msg_out.id = msg.id;
        msg_out.dlc = msg.dlc;
        *(uint64_t*)msg_out.data = msg.data;
        this->can_outbox.push_back(msg_out);
    }
}

void VcEcu::injectCan(ecusim::CanMsg msg)
{
    CAN_add_message_rx_queue(msg.id, msg.dlc, (uint8_t*)msg.data);
    can_data_available = true;
}

void VcEcu::set(std::string key, float value)
{   
    // analog inputs
    if (key == "accela")
    {
        hardware.aio[AIOpin_ACCEL_A] = value;
    }
    else if (key == "accelb")
    {
        hardware.aio[AIOpin_ACCEL_B] = value;
    }
    else if (key == "brakep")
    {
        hardware.aio[AIOpin_BRAKE_PRESSURE] = value;
    }

    // digital inputs (some of these are outputs in practice)
    if (key == "sound1")
    {
        hardware.dio[DIOpin_SOUND_1] = value == 0.0;
    }
    else if (key == "sound2")
    {
        hardware.dio[DIOpin_SOUND_2] = value == 0.0;
    }
    else if (key == "sound3")
    {
        hardware.dio[DIOpin_SOUND_3] = value == 0.0;
    }
    else if (key == "sound4")
    {
        hardware.dio[DIOpin_SOUND_4] = value == 0.0;
    }
    else if (key == "sound5")
    {
        hardware.dio[DIOpin_SOUND_5] = value == 0.0;
    }
    else if (key == "sound6")
    {
        hardware.dio[DIOpin_SOUND_6] = value == 0.0;
    }
    else if (key == "switch1")
    {
        hardware.dio[DIOpin_SWITCH_1] = value == 0.0;
    }
    else if (key == "switch2")
    {
        hardware.dio[DIOpin_SWITCH_2] = value == 0.0;
    }
    else if (key == "switch3")
    {
        hardware.dio[DIOpin_SWITCH_3] = value == 0.0;
    }
    else if (key == "switch4")
    {
        hardware.dio[DIOpin_SWITCH_4] = value == 0.0;
    }
    else if (key == "led")
    {
        hardware.dio[DIOpin_STATUS_LED] = value == 0.0;
    }

    // shutdown inputs (handled by interrupt on hardware)
    else if (key == "bmsfault")
    {
        this->shutdown_status.vc_shutdown_status_bms_fault = value == 0.0;
        ShutdownMonitor_update(&this->shutdown_status);
    }
    else if (key == "imdfault")
    {
        this->shutdown_status.vc_shutdown_status_imd_fault = value == 0.0;
        ShutdownMonitor_update(&this->shutdown_status);
    }
    else if (key == "bspdfault")
    {
        this->shutdown_status.vc_shutdown_status_bms_fault = value == 0.0;
        ShutdownMonitor_update(&this->shutdown_status);
    }
    else if (key == "bspd_signal_lost")
    {
        this->shutdown_status.vc_shutdown_status_bspd_signal_lost = value == 0.0;
        ShutdownMonitor_update(&this->shutdown_status);
    }
    else if (key == "precharge")
    {
        this->shutdown_status.vc_shutdown_status_precharge = value == 0.0;
        ShutdownMonitor_update(&this->shutdown_status);
    }
}

float VcEcu::get(std::string key)
{
    // analog inputs
    if (key == "accela")
    {
        return hardware.aio[AIOpin_ACCEL_A];
    }
    else if (key == "accelb")
    {
        return hardware.aio[AIOpin_ACCEL_B];
    }
    else if (key == "brakep")
    {
        return hardware.aio[AIOpin_BRAKE_PRESSURE];
    }

    // digital inputs (some of these are outputs in practice)
    else if (key == "sound1") // scons sim
    {
        return hardware.dio[DIOpin_SOUND_1];
    }
    else if (key == "sound2")
    {
        return hardware.dio[DIOpin_SOUND_2];
    }
    else if (key == "sound3")
    {
        return hardware.dio[DIOpin_SOUND_3];
    }
    else if (key == "sound4")
    {
        return hardware.dio[DIOpin_SOUND_4];
    }
    else if (key == "sound5")
    {
        return hardware.dio[DIOpin_SOUND_5];
    }
    else if (key == "sound6")
    {
        return hardware.dio[DIOpin_SOUND_6];
    }
    else if (key == "switch1")
    {
        return hardware.dio[DIOpin_SWITCH_1];
    }
    else if (key == "switch2")
    {
        return hardware.dio[DIOpin_SWITCH_2];
    }
    else if (key == "switch3")
    {
        return hardware.dio[DIOpin_SWITCH_3];
    }
    else if (key == "switch4")
    {
        return hardware.dio[DIOpin_SWITCH_4];
    }
    else if (key == "led")
    {
        return hardware.dio[DIOpin_STATUS_LED];
    }

    // shutdown inputs (handled by interrupt on hardware)
    else if (key == "bmsfault")
    {
        return this->shutdown_status.vc_shutdown_status_bms_fault;
    }
    else if (key == "imdfault")
    {
        return this->shutdown_status.vc_shutdown_status_imd_fault;
    }
    else if (key == "bspdfault")
    {
        return this->shutdown_status.vc_shutdown_status_bspd_fault;
    }
    else if (key == "bspd_signal_lost")
    {
        return this->shutdown_status.vc_shutdown_status_bspd_signal_lost;
    }
    else if (key == "precharge")
    {
        return this->shutdown_status.vc_shutdown_status_precharge;
    }
    
    return -1;
}