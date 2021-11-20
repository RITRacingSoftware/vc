#include <string.h>

// FreeRTOS stuff
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// drivers
#include "HAL_Aio.h"
#include "HAL_Dio.h"
#include "HAL_Uart.h"
#include "HAL_Can.h"
#include "HAL_Clock.h"

// application code
#include "VC.h"
#include "CAN.h"

#define SEPHAMORE_WAIT 0

#define TASK_100Hz_NAME "task_100Hz"
#define TASK_100Hz_PRIORITY (tskIDLE_PRIORITY + 1)
#define TASK_100Hz_PERIOD_MS (1)
#define TASK_100Hz_STACK_SIZE_B (1000)

SemaphoreHandle_t can_message_recieved_semaphore;
SemaphoreHandle_t can_message_transmit_semaphore;

void task_100Hz(void *pvParameters)
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();
    for (;;)
    {
        VC_100Hz();
        CAN_send_queued_messages();
        vTaskDelayUntil(&next_wake_time, TASK_100Hz_PERIOD_MS);
    }
}

#define TASK_CAN_RX_NAME "task_CAN_RX"
#define TASK_CAN_RX_PRIORITY (tskIDLE_PRIORITY + 4)
#define TASK_CAN_RX_PERIOD_MS (1)
#define TASK_CAN_RX_STACK_SIZE_B (500) 

void task_can_rx(void *pvParameters)
{
    (void) pvParameters;
    // TickType_t next_wake_time = xTaskGetTickCount();
    for (;;)
    {
        if(xSemaphoreTake(can_message_recieved_semaphore, portMAX_DELAY) == pdTRUE)
        {
            CAN_process_recieved_messages();
        }
    }
}

#define TASK_CAN_TX_NAME "task_CAN_TX"
#define TASK_CAN_TX_PRIORITY (tskIDLE_PRIORITY + 4)
#define TASK_CAN_TX_PERIOD_MS (1)
#define TASK_CAN_TX_STACK_SIZE_B (500) 

void task_can_tx(void *pvParameters)
{
    (void) pvParameters;
    // TickType_t next_wake_time = xTaskGetTickCount();
    for (;;)
    {
        if(xSemaphoreTake(can_message_transmit_semaphore, portMAX_DELAY) == pdTRUE)
        {
            CAN_send_queued_messages();
        }
    }
}

void hardfault_handler_routine(void)
{
    // kill torque
    // messages will stop sending but just in case
    MotorController_set_torque(0);

    TaskHandle_t current_task = xTaskGetCurrentTaskHandle();
    char* task_name = pcTaskGetName(current_task);

    int task_id = 0;
    // need to add an else if here every time a task is added
    // the id should line up with the DBC enum definition (bottom of main_bus.dbc)
    if (strcmp(task_name, TASK_100Hz_NAME) == 0)
    {
        task_id = 1;
    }

    uint8_t data[8];
    can_bus.vc_hard_fault_indicator.vc_hard_fault_indicator_task = task_id;
    main_bus_vc_hard_fault_indicator_pack(data, &can_bus.vc_hard_fault_indicator, 8);
    HAL_Can_send_message(MAIN_BUS_VC_HARD_FAULT_INDICATOR_FRAME_ID, 8, *((uint64_t*)data)); 
}

int main(void)
{
    can_message_recieved_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(can_message_recieved_semaphore);
    xSemaphoreTake(can_message_recieved_semaphore, SEPHAMORE_WAIT);
    can_message_transmit_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(can_message_transmit_semaphore);
    xSemaphoreTake(can_message_transmit_semaphore, SEPHAMORE_WAIT);

    // initialize all drivers
    HAL_Clock_init();
    HAL_Uart_init();
    HAL_Can_init();
    HAL_Aio_init();
    HAL_Dio_init();

    // initialize all application modules
    VC_init();
    
    // initialize tasks
    xTaskCreate(task_100Hz, 
        TASK_100Hz_NAME, 
        TASK_100Hz_STACK_SIZE_B,
        NULL,
        TASK_100Hz_PRIORITY,
        NULL);

    xTaskCreate(task_can_rx, 
        TASK_CAN_RX_NAME, 
        TASK_CAN_RX_STACK_SIZE_B,
        NULL,
        TASK_CAN_RX_PRIORITY,
        NULL);

    xTaskCreate(task_can_tx, 
        TASK_CAN_TX_NAME, 
        TASK_CAN_TX_STACK_SIZE_B,
        NULL,
        TASK_CAN_TX_PRIORITY,
        NULL);
   
    // hand control over to FreeRTOS
    vTaskStartScheduler();

    // we should not get here ever
    hardfault_handler_routine();
    for (;;);
}