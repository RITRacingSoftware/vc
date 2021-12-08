#include "stm32f0xx_gpio.h"
#include "stm32f0xx.h"

#include "HAL_Dio.h"
#include "common_macros.h"

static GPIO_TypeDef* port_for_pin(DIOpin_e pin)
{
    GPIO_TypeDef* port;

    switch (pin)
    {
        case DIOpin_SOUND_0:
        case DIOpin_SOUND_1:
        case DIOpin_SOUND_2:
        case DIOpin_SOUND_3:
        case DIOpin_SOUND_4:
        case DIOpin_SOUND_5:
        case DIOpin_SOUND_6:
        case DIOpin_STATUS_LED:
            port = GPIOB;
            break;
        
        default:
            port = GPIOA; 
            break;
    }

    return port;
}

static uint16_t mask_for_pin(DIOpin_e pin)
{
    uint16_t mask;
    switch (pin)
    {
        case DIOpin_SOUND_0:
            mask = BIT(2);
            break;
        
        case DIOpin_SOUND_1:
            mask = BIT(4);
            break;
        
        case DIOpin_SOUND_2:
            mask = BIT(5);
            break;
        
        case DIOpin_SOUND_3:
            mask = BIT(6);
            break;

        case DIOpin_SOUND_4:
            mask = BIT(7);
            break;

        case DIOpin_SOUND_5:
            mask = BIT(8);
            break;

        case DIOpin_SOUND_6:
            mask = BIT(9);
            break;

        case DIOpin_STATUS_LED:
            mask = BIT(12);
            break;
        
        case DIOpin_SWITCH_0:
            mask = BIT(6);
            break;
        
        case DIOpin_SWITCH_1:
            mask = BIT(7);
            break;
        
        case DIOpin_SWITCH_2:
            mask = BIT(8);
            break;
        
        case DIOpin_SWITCH_3:
            mask = BIT(9);
            break;
    }

    return mask;
}

void HAL_Dio_init(void)
{
    // enable GPIOA and GPIOB peripherals
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    // initialize pins

    // start with port A pins
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_6 |  // switch 0
        GPIO_Pin_7 | // switch 1
        GPIO_Pin_8 | // switch 2
        GPIO_Pin_9; // switch 3
    gpio_init.GPIO_Mode = GPIO_Mode_IN;
    gpio_init.GPIO_Speed = GPIO_Speed_Level_1;
    gpio_init.GPIO_OType = GPIO_OType_PP; // shouldnt matter
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL; // TODO - figure out what this should be
    GPIO_Init(GPIOA, &gpio_init);

    // gpio_init.GPIO_Pin = mask_for_pin(DIOpin_STATUS_LED); // status led
    // gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    // gpio_init.GPIO_Speed = GPIO_Speed_Level_1;
    // gpio_init.GPIO_OType = GPIO_OType_PP; // shouldnt matter
    // gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL; // TODO - figure out what this should be
    // GPIO_Init(GPIOA, &gpio_init);

    // now do port B pins
    gpio_init.GPIO_Pin = GPIO_Pin_2 |  // sound 0
        GPIO_Pin_4 | // sound 1
        GPIO_Pin_5 | // sound 2
        GPIO_Pin_6 | // sound 3
        GPIO_Pin_7 | // sound 4
        GPIO_Pin_8 | // sound 5
        GPIO_Pin_9 | // sound 6
        GPIO_Pin_12; // status LED
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_Speed = GPIO_Speed_Level_1;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL; // shouldnt matter
    GPIO_Init(GPIOB, &gpio_init);
}

bool HAL_Dio_read(DIOpin_e pin)
{
    GPIO_TypeDef* gpio = port_for_pin(pin);
    uint16_t pin_mask = mask_for_pin(pin);

    return GPIO_ReadInputDataBit(gpio, pin_mask);
}

void HAL_Dio_write(DIOpin_e pin, bool val)
{
    GPIO_TypeDef* gpio = port_for_pin(pin);
    uint16_t mask = mask_for_pin(pin);
    if (val)
    {
        GPIO_SetBits(gpio, mask);
    }
    else
    {
        GPIO_ResetBits(gpio, mask);
    }
}
