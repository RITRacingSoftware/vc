#include "stm32f0xx_gpio.h"
#include "stm32f0xx.h"

#include "HAL_Dio.h"
#include "common_macros.h"

typedef struct {
    DIOpin_e id;
    GPIO_TypeDef* port;
    GPIO_InitTypeDef init;
} DIOconfig;

DIOconfig configs[DIOpin_NUM] = {
    {DIOpin_SOUND_1, GPIOB, {BIT(4), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SOUND_2, GPIOB, {BIT(5), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SOUND_3, GPIOB, {BIT(6), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SOUND_4, GPIOB, {BIT(7), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SOUND_5, GPIOB, {BIT(8), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SOUND_6, GPIOB, {BIT(9), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SWITCH_1, GPIOA, {BIT(6), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SWITCH_2, GPIOA, {BIT(7), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}}, 
    {DIOpin_RTD_BUTTON, GPIOA, {BIT(15), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_SWITCH_3, GPIOA, {BIT(9), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_BMS_FAULT, GPIOA, {BIT(1), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_IMD_FAULT, GPIOA, {BIT(0), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_BSPD_FAULT, GPIOB, {BIT(0), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_BSPD_SIGNAL_LOST, GPIOC, {BIT(0), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_PRECHARGE, GPIOB, {BIT(1), GPIO_Mode_IN, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
    {DIOpin_STATUS_LED, GPIOB, {BIT(12), GPIO_Mode_OUT, GPIO_Speed_Level_1, GPIO_OType_PP, GPIO_PuPd_NOPULL}},
};

DIOconfig* config_from_id(DIOpin_e id)
{
    for (int i = 0; i < DIOpin_NUM; i++)
    {
        if (configs[i].id == id)
        {
            return &configs[i];
        }
    }
}

void HAL_Dio_init(void)
{
    // enable GPIOA and GPIOB peripherals
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    // initialize each config
    for (int i = 0; i < DIOpin_NUM; i++)
    {
        GPIO_Init(configs[i].port, &configs[i].init);
    }
}

bool HAL_Dio_read(DIOpin_e pin)
{
    DIOconfig* config = config_from_id(pin);
    return GPIO_ReadInputDataBit(config->port, config->init.GPIO_Pin);
}

void HAL_Dio_write(DIOpin_e pin, bool val)
{
    DIOconfig* config = config_from_id(pin);
    
    if (val)
    {
        GPIO_SetBits(config->port, config->init.GPIO_Pin);
    }
    else
    {
        GPIO_ResetBits(config->port, config->init.GPIO_Pin);
    }
}
