#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"
#include "HAL_Aio.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "Config.h"

void HAL_Aio_init(void)
{
    // enable ADC and GPIO peripherals
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // configure the GPIO peripheral to allow use of the ADC for the 
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // initialize ADC peripheral
    ADC_DeInit(ADC1);
    ADC_InitTypeDef adc_init;
    ADC_StructInit(&adc_init);

    // the default settings are exactly what we want
    ADC_Init(ADC1, &adc_init);

    // calibrate adc
    ADC_GetCalibrationFactor(ADC1);

     // Enable the ADC peripheral
    ADC_Cmd(ADC1, ENABLE);  
}

uint16_t HAL_Aio_read(AIOpin_e pin)
{
    // Note: Channels 0-15 are adc pins, while 16-18 are internal values (like proc temp, etc.)

    // determine which channel to read
    unsigned int channel;
    switch (pin)
    {
        case AIOpin_ACCEL_A:
            channel = ADC_CHSELR_CHSEL3;
            break;
        
        case AIOpin_ACCEL_B:
            channel = ADC_CHSELR_CHSEL4;
            break;
        
        case AIOpin_BRAKE_PRESSURE:
            channel = ADC_CHSELR_CHSEL2;
            break;

        default:
            return 0;
    }

    // select channel
    ADC1->CHSELR |= channel;

    // start conversion
    ADC_StartOfConversion(ADC1);

    // wait for conversion to end (this is what makes this read synchronous)
    while(!(ADC1->ISR & ADC_ISR_EOC));

    // read and return the value the adc read
    return ADC_GetConversionValue(ADC1);
}