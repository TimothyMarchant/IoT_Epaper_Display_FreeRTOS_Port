/* 
 * File:   main.c
 * Author: Timothy Marchant
 *
 * Created on March 31, 2025, 12:47 AM
 * The purpose of this project was to port my already existing bare metal project to an RTOS. 2800 bytes of ram is used for the FreeRTOS heap.
 * I didn't bother with the sleep functions for the idle task, as I view it as unnecessary and that there was very little to gain from it.
 * The compiler being used is XC32 with O2 optimizations.  O0 (debug) can barely fit on the chip.
 * For an actual product a bigger microcontroller probably is necessary, but I digress.
 */

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include "GPIO.h"
#include "EIC_Methods.h"
#include "SPI_Methods.h"
#include "UART_Methods.h"
#include "EPaper_Methods.h"
void (* screenmethod)(void);
void updatescreen(void);
void UART_task(void*);
void SPI_task(void*);
//semaphore declarations
SemaphoreHandle_t EICReady;
SemaphoreHandle_t SPIFinished;
SemaphoreHandle_t TXready;
SemaphoreHandle_t RXready;
SemaphoreHandle_t UARTFinished;
SemaphoreHandle_t SPIready;
//queue declarations
QueueHandle_t SPI_Queue;
QueueHandle_t UART_Transmit_Queue;
//generic UART receiver queue
QueueHandle_t UART_Receive_Queue;
//task declarations.  Main task is omitted as it shouldn't be referenced by anything.
TaskHandle_t SPITask;
TaskHandle_t UARTTask;
//callback functions for determining which function to call after wakeup.

void EIC0_Callback(void) {
    screenmethod = &testscreen;
    BaseType_t higherprioritytask = pdFALSE;
    xSemaphoreGiveFromISR(EICReady, &higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}

void EIC1_Callback(void) {
    screenmethod = &updatescreen;
    BaseType_t higherprioritytask = pdFALSE;
    xSemaphoreGiveFromISR(EICReady, &higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}

void SetupHardware(void) {
    Init_IO();
    Init_Epaper_IO();
    Init_EIC(EIC0 | EIC1, 0);
    Set_EIC0(RISE);
    Set_EIC1(RISE);
    Enable_EIC();
    InitSPI(2);
    InitUART();
    EnableUART();
    EnableSPI();
}

void maintask(void * pvParameters) {
    while (1) {
        xSemaphoreTake(EICReady, portMAX_DELAY);
        screenmethod();
    }
}

int main(void) {
    SetupHardware();
    //create all binary semaphores
    EICReady = xSemaphoreCreateBinary();
    SPIFinished = xSemaphoreCreateBinary();
    SPIready = xSemaphoreCreateBinary();
    TXready = xSemaphoreCreateBinary();
    RXready = xSemaphoreCreateBinary();
    UARTFinished = xSemaphoreCreateBinary();
    //create byte queues
    UART_Transmit_Queue = xQueueCreate(25, sizeof (unsigned char));
    UART_Receive_Queue = xQueueCreate(20, sizeof (unsigned char));
    SPI_Queue = xQueueCreate(5, sizeof (unsigned char));
    //size of tasks are not optimized, they just are their size to insure that they work.
    xTaskCreate(UART_task, "UART task", 75, NULL, 3, &UARTTask);
    xTaskCreate(SPI_task, "SPI task", 50, NULL, 3, &SPITask);
    xTaskCreate(maintask, "Main task", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    //should never be reached
    while (1) {
    }
    return (EXIT_SUCCESS);
}

