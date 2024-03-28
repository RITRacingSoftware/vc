#include "HAL_Aio.h"

#include "Config.h"
#include "stm32_main.h"

#include "stm32g4xx_hal_dma.h" // Needed due to an include bug in stm32g4xx_hal_adc.h
#include "stm32g4xx_hal_adc.h"
#include "stm32g4xx_hal_adc_ex.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_rcc.h"

static ADC_HandleTypeDef adc1;
static ADC_HandleTypeDef adc2;

void HAL_Aio_init(void)
{
    // configure needed GPIO pins
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // initialize ADC1 peripheral
    adc1.Instance = ADC1;
    adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    adc1.Init.Resolution = ADC_RESOLUTION_12B;
    adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc1.Init.GainCompensation = 0;
    adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc1.Init.LowPowerAutoWait = DISABLE;
    adc1.Init.ContinuousConvMode = DISABLE;
    adc1.Init.NbrOfConversion = 1;
    adc1.Init.DiscontinuousConvMode = DISABLE;
    adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc1.Init.DMAContinuousRequests = DISABLE;
    adc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    adc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&adc1) != HAL_OK) {
        hardfault_handler_routine();
    }

    // initialize ADC2 peripheral
    adc2.Instance = ADC2;
    adc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    adc2.Init.Resolution = ADC_RESOLUTION_12B;
    adc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc2.Init.GainCompensation = 0;
    adc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc2.Init.LowPowerAutoWait = DISABLE;
    adc2.Init.ContinuousConvMode = DISABLE;
    adc2.Init.NbrOfConversion = 1;
    adc2.Init.DiscontinuousConvMode = DISABLE;
    adc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc2.Init.DMAContinuousRequests = DISABLE;
    adc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    adc2.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&adc2) != HAL_OK) {
        hardfault_handler_routine();
    }

    if (HAL_ADCEx_Calibration_Start(&adc1, ADC_SINGLE_ENDED) != HAL_OK) {
        hardfault_handler_routine();
    }

    if (HAL_ADCEx_Calibration_Start(&adc2, ADC_SINGLE_ENDED) != HAL_OK) {
        hardfault_handler_routine();
    }
}

uint16_t HAL_Aio_read(AIOpin_e pin)
{
    // Initialize channel
    ADC_HandleTypeDef adcx;
    ADC_ChannelConfTypeDef sConfig;

    switch (pin) {
        case AIOpin_ACCEL_A:
            adcx = adc2;
            sConfig.Channel = ADC_CHANNEL_17;
            break;
        case AIOpin_ACCEL_B:
            adcx = adc1;
            sConfig.Channel = ADC_CHANNEL_4;
            break;
        case AIOpin_BRAKE_PRESSURE:
            adcx = adc1;
            sConfig.Channel = ADC_CHANNEL_3;
            break;
        default:
            hardfault_handler_routine();
    }

    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&adcx, &sConfig) != HAL_OK)
    {
        hardfault_handler_routine();
    }

    // Perform reading
    HAL_ADC_Start(&adcx);
    HAL_ADC_PollForConversion(&adcx, HAL_MAX_DELAY);
    uint16_t val = HAL_ADC_GetValue(&adcx);
    HAL_ADC_Stop(&adcx);
    return val;
}