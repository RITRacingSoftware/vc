#include "HAL_Can.h"

// Standard library
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Cube drivers
#include "stm32g4xx_hal_conf.h"
#include "stm32g4xx_hal_cortex.h"
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_gpio_ex.h"
#include "stm32g4xx_hal_rcc.h"

// FreeRTOS
#include "semphr.h"
#include "queue.h"

// Application code
#include "stm32_main.h"
#include "CAN.h"


#define CAN_MAIN_PINS (GPIO_PIN_12 | GPIO_PIN_13)
#define CAN_SENSOR_PINS (GPIO_PIN_11 | GPIO_PIN_12)

static uint8_t num_filters = 0;

static FDCAN_HandleTypeDef can_main;
static FDCAN_HandleTypeDef can_sensor;


// Pipe interrupts back to cube code
void FDCAN2_IT0_IRQHandler(void) {
    HAL_FDCAN_IRQHandler(&can_main);
}
void FDCAN1_IT0_IRQHandler(void) {
    HAL_FDCAN_IRQHandler(&can_main);
}

static void main_bus_rx_handler(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        FDCAN_RxHeaderTypeDef header;
        uint8_t data[8];

        // Retrieve Rx messages from RX FIFO0
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, data) != HAL_OK)
        {
            hardfault_handler_routine();
        }

        CAN_add_message_main_rx_queue(header.Identifier, header.DataLength, data);
    }
}

static void sensor_bus_rx_handler(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        FDCAN_RxHeaderTypeDef header;
        uint8_t data[8];

        // Retrieve Rx messages from RX FIFO0
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, data) != HAL_OK)
        {
            hardfault_handler_routine();
        }

        CAN_add_message_sensor_rx_queue(header.Identifier, header.DataLength, data);
    }
}

// Must initialize gpio first to read charger line
void HAL_Can_init(void)
{
    // Initialize pins
    GPIO_InitTypeDef gpio1 = {CAN_MAIN_PINS, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF9_FDCAN2};
    HAL_GPIO_Init(GPIOB, &gpio1);
    GPIO_InitTypeDef gpio2 = {CAN_SENSOR_PINS, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF9_FDCAN1};
    HAL_GPIO_Init(GPIOA, &gpio2);

    // Initialize clocks
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        hardfault_handler_routine();
    }
    __HAL_RCC_FDCAN_CLK_ENABLE();

    // Initialize CAN interfaces
    can_main.Instance = FDCAN2;
    can_main.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    can_main.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    can_main.Init.Mode = FDCAN_MODE_NORMAL;
    can_main.Init.AutoRetransmission = DISABLE;
    can_main.Init.TransmitPause = DISABLE;
    can_main.Init.ProtocolException = DISABLE;
    can_main.Init.NominalPrescaler = 8;
    can_main.Init.NominalSyncJumpWidth = 1;
    can_main.Init.NominalTimeSeg1 = 12;
    can_main.Init.NominalTimeSeg2 = 2;
    can_main.Init.DataPrescaler = 1;
    can_main.Init.DataSyncJumpWidth = 1;
    can_main.Init.DataTimeSeg1 = 1;
    can_main.Init.DataTimeSeg2 = 1;
    can_main.Init.StdFiltersNbr = 0;
    can_main.Init.ExtFiltersNbr = 0;
    can_main.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    if (HAL_FDCAN_Init(&can_main) != HAL_OK)
    {
        hardfault_handler_routine();
    }

    can_sensor.Instance = FDCAN1;
    can_sensor.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    can_sensor.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    can_sensor.Init.Mode = FDCAN_MODE_NORMAL;
    can_sensor.Init.AutoRetransmission = DISABLE;
    can_sensor.Init.TransmitPause = DISABLE;
    can_sensor.Init.ProtocolException = DISABLE;
    can_sensor.Init.NominalPrescaler = 8;
    can_sensor.Init.NominalSyncJumpWidth = 1;
    can_sensor.Init.NominalTimeSeg1 = 12;
    can_sensor.Init.NominalTimeSeg2 = 2;
    can_sensor.Init.DataPrescaler = 1;
    can_sensor.Init.DataSyncJumpWidth = 1;
    can_sensor.Init.DataTimeSeg1 = 1;
    can_sensor.Init.DataTimeSeg2 = 1;
    can_sensor.Init.StdFiltersNbr = 0;
    can_sensor.Init.ExtFiltersNbr = 0;
    can_sensor.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    if (HAL_FDCAN_Init(&can_sensor) != HAL_OK)
    {
        hardfault_handler_routine();
    }

    // Allow all standard frames
    FDCAN_FilterTypeDef filter;
    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = 0;
    filter.FilterType = FDCAN_FILTER_RANGE;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = 0x000;
    filter.FilterID2 = 0x7FF;
    if (HAL_FDCAN_ConfigFilter(&can_main, &filter) != HAL_OK)
    {
        hardfault_handler_routine();
    }
    if (HAL_FDCAN_ConfigFilter(&can_sensor, &filter) != HAL_OK)
    {
        hardfault_handler_routine();
    }

    // Set up RX interrupts
    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 5, 0); // Main bus has slightly higher priority than sensor bus
    HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

    // Register callbacks
    if (HAL_FDCAN_RegisterRxFifo0Callback(&can_main, main_bus_rx_handler) != HAL_OK) {
        hardfault_handler_routine();
    }
    if (HAL_FDCAN_RegisterRxFifo0Callback(&can_sensor, sensor_bus_rx_handler) != HAL_OK) {
        hardfault_handler_routine();
    }


    // Start can interfaces
    if (HAL_FDCAN_Start(&can_main) != HAL_OK)
    {
        hardfault_handler_routine();
    }
    if (HAL_FDCAN_Start(&can_sensor) != HAL_OK)
    {
        hardfault_handler_routine();
    }

    // Send new messages to registered callback
    if (HAL_FDCAN_ActivateNotification(&can_main, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
        hardfault_handler_routine();
    }
    if (HAL_FDCAN_ActivateNotification(&can_sensor, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
        hardfault_handler_routine();
    }
}

Error_t HAL_Can_send_message_main(uint32_t id, int dlc, uint64_t data)
{
    FDCAN_TxHeaderTypeDef header = {0};
    header.Identifier = id;
    header.IdType = FDCAN_STANDARD_ID;
    header.TxFrameType = FDCAN_DATA_FRAME;
    header.DataLength = dlc;
    header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    header.BitRateSwitch = FDCAN_BRS_OFF;
    header.FDFormat = FDCAN_CLASSIC_CAN;
    header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    header.MessageMarker = 0;

    HAL_StatusTypeDef err =  HAL_FDCAN_AddMessageToTxFifoQ(&can_main, &header, (uint8_t*) &data);
}

Error_t HAL_Can_send_message_sensor(uint32_t id, int dlc, uint64_t data)
{
    FDCAN_TxHeaderTypeDef header = {0};
    header.Identifier = id;
    header.IdType = FDCAN_STANDARD_ID;
    header.TxFrameType = FDCAN_DATA_FRAME;
    header.DataLength = dlc;
    header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    header.BitRateSwitch = FDCAN_BRS_OFF;
    header.FDFormat = FDCAN_CLASSIC_CAN;
    header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    header.MessageMarker = 0;

    HAL_StatusTypeDef err =  HAL_FDCAN_AddMessageToTxFifoQ(&can_sensor, &header, (uint8_t*) &data);
}









void HAL_Can_init_id_filter_32bit(uint32_t id)
{

}

void HAL_Can_init_id_filter_16bit(uint16_t id1, uint16_t id2, uint16_t id3, uint16_t id4)
{
    
}



uint8_t HAL_number_of_empty_mailboxes(void)
{
    
}
