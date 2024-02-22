#include "HAL_Dio.h"

#include "common_macros.h"

#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_rcc.h"

typedef struct {
    DIOpin_e id;
    GPIO_TypeDef* port;
    GPIO_InitTypeDef init;
} DIOconfig;

DIOconfig configs[DIOpin_NUM] = {
    // Inputs
    {DIOpin_RTD_BUTTON,        GPIOA, {GPIO_PIN_15, GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SWITCH_1,          GPIOA, {GPIO_PIN_6,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SWITCH_2,          GPIOA, {GPIO_PIN_7,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SWITCH_3,          GPIOA, {GPIO_PIN_9,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},

    {DIOpin_BMS_FAULT,         GPIOA, {GPIO_PIN_1,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_IMD_FAULT,         GPIOA, {GPIO_PIN_0,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_BSPD_FAULT,        GPIOB, {GPIO_PIN_0,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_BSPD_SIGNAL_LOST,  GPIOC, {GPIO_PIN_0,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_PRECHARGE,         GPIOB, {GPIO_PIN_1,  GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},

    // Outputs
    {DIOpin_STATUS_LED,        GPIOB, {GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},

    {DIOpin_SOUND_1,           GPIOB, {GPIO_PIN_4,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SOUND_2,           GPIOB, {GPIO_PIN_5,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SOUND_3,           GPIOB, {GPIO_PIN_6,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SOUND_4,           GPIOB, {GPIO_PIN_7,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SOUND_5,           GPIOB, {GPIO_PIN_8,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
    {DIOpin_SOUND_6,           GPIOB, {GPIO_PIN_9,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}},
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
    // enable GPIOA and GPIOB peripheral clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // initialize each config
    for (int i = 0; i < DIOpin_NUM; i++)
    {
        HAL_GPIO_Init(configs[i].port, &configs[i].init);
    }
}

bool HAL_Dio_read(DIOpin_e pin)
{
    DIOconfig* config = config_from_id(pin);
    return HAL_GPIO_ReadPin(config->port, config->init.Pin);
}

void HAL_Dio_write(DIOpin_e pin, bool val)
{
    DIOconfig* config = config_from_id(pin);
    HAL_GPIO_WritePin(config->port, config->init.Pin, val);
}
