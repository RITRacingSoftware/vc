#include "CAN.h"
#include "FaultManager.h"
#include "HAL_Dio.h"

typedef enum {
    SystemStatus_NOMINAL,
    SystemStatus_FAULTED,
    SystemStatus_CAN_ERROR,
} SystemStatus_e;

static SystemStatus_e status;


static void led_on(void)
{
    HAL_Dio_write(DIOpin_STATUS_LED, true);
}

static void led_off(void)
{
    HAL_Dio_write(DIOpin_STATUS_LED, false);
}

static void pattern_nominal(unsigned int count_ms)
{
    unsigned int mod_ms = count_ms % 2000;

    // blink once every 2 seconds
    if (mod_ms < 200)
    {
        led_on();
    }
    else
    {
        led_off();
    }
}

static void pattern_faulted(unsigned int count_ms)
{
    unsigned int mod_ms = count_ms % 2000;

    // steady 1Hz blink

    if (mod_ms < 1000)
    {
        led_on();
    }
    else 
    {
        led_off();
    }
}

static void pattern_can_error(unsigned int count_ms)
{
    unsigned int mod_ms = count_ms % 500;

    // fast blink, 4 Hz

    if (mod_ms < 250)
    {
        led_on();
    }
    else
    {
        led_off();
    }
}

void HeartBeatLed_init(void)
{
    status = SystemStatus_NOMINAL;
}

void HeartBeatLed_100Hz(void)
{
    static unsigned int count_ms = 0;

    // determine system status
    if (CAN_get_tx_error() || CAN_get_rx_error())
    {
        status = SystemStatus_CAN_ERROR;
    }
    else if (FaultManager_is_any_fault_active())
    {
        status = SystemStatus_FAULTED;
    }
    else
    {
        status = SystemStatus_NOMINAL;
    }

    // run pattern function for led blinking
    switch (status)
    {
        case SystemStatus_NOMINAL:
            pattern_nominal(count_ms);
            break;
        
        case SystemStatus_FAULTED:
            pattern_faulted(count_ms);
            break;

        case SystemStatus_CAN_ERROR:
            pattern_can_error(count_ms);
            break;
    }
}
