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
#include "Task_Names.h"
void (* screenmethod)(void);
void SetScreenMethod(volatile void (*Method)(void));
void updatescreen(void);
//static global variables
static volatile _Bool UsingDisplay=0;
static volatile _Bool UsingESP=0;
//semaphore declarations
SemaphoreHandle_t Epaper_start;
SemaphoreHandle_t Epaper_INIT_finished;
SemaphoreHandle_t Epaper_Finished;
SemaphoreHandle_t BusyLOW;
SemaphoreHandle_t ESP_Image_Received;
SemaphoreHandle_t ESP_Ready;
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
//TaskHandle_t SPITask;
TaskHandle_t UARTTask;
TaskHandle_t EpaperTask;
TaskHandle_t ESPTask;
//callback functions for determining which function to call after wakeup.
static void SetEpaperBool(void){
    UsingDisplay=1;
}
static void SetESPBool(void){
    UsingESP=1;
}
static void ClearBools(void){
    UsingESP=0;
    UsingDisplay=0;
}
void EIC0_Callback(void) {
    SetScreenMethod(ClearScreen);
    BaseType_t higherprioritytask = pdFALSE;
    xSemaphoreGiveFromISR(EICReady, &higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
    SetEpaperBool();
}

void EIC1_Callback(void) {
    SetScreenMethod(updatescreen);
    BaseType_t higherprioritytask = pdFALSE;
    xSemaphoreGiveFromISR(EICReady, &higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
    SetEpaperBool();
    SetESPBool();
}

void SetupHardware(void) {
    Init_IO();
    Init_Epaper_IO();
    Init_EIC(EIC0 | EIC1 | EIC2, 0);
    Set_EIC0(RISE);
    Set_EIC1(RISE);
    Set_EIC2(FALL);
    Enable_EIC();
    InitSPI(2);
    InitUART();
    EnableUART();
    EnableSPI();
}
//used for waking up other tasks.
void maintask(void * pvParameters) {
    while (1) {
        //suspend until further notice.
        xSemaphoreTake(EICReady, portMAX_DELAY);
        Clear_EIC_INT(EIC0|EIC1);
        Set_EIC_INT(EIC2);
        if (UsingDisplay){
            
            vTaskResume(EpaperTask);
        }
        if (UsingESP){
            vTaskResume(ESPTask);
        }
        //suspend until Epaper transfer is complete OR at least 30 seconds has passed.
        xSemaphoreTake(Epaper_Finished,portMAX_DELAY);
        ClearBools();
        Set_EIC_INT(EIC0|EIC1);
        Clear_EIC_INT(EIC2);
    }
}
void CreateSemaphores(void){
    //create all binary semaphores
    EICReady = xSemaphoreCreateBinary();
    SPIFinished = xSemaphoreCreateBinary();
    SPIready = xSemaphoreCreateBinary();
    TXready = xSemaphoreCreateBinary();
    RXready = xSemaphoreCreateBinary();
    UARTFinished = xSemaphoreCreateBinary();
    ESP_Image_Received=xSemaphoreCreateBinary();
    Epaper_INIT_finished=xSemaphoreCreateBinary();
    Epaper_Finished=xSemaphoreCreateBinary();
    BusyLOW=xSemaphoreCreateBinary();
}
int main(void) {
    SetupHardware();
    CreateSemaphores();
    //create byte queues
    UART_Transmit_Queue = xQueueCreate(25, sizeof (unsigned char));
    UART_Receive_Queue = xQueueCreate(20, sizeof (unsigned char));
    //SPI_Queue = xQueueCreate(5, sizeof (unsigned char)); not used
    //size of tasks are not optimized, they just are their size to insure that they work.
    xTaskCreate(UART_task, "UART task", 100, NULL, 4, &UARTTask);
    xTaskCreate(ESP_Task,"ESP task",100,NULL,1,&ESPTask);
    xTaskCreate(Epaper_Task,"Epaper task",100,NULL,1,&EpaperTask);
    //xTaskCreate(SPI_task, "SPI task", 50, NULL, 3, &SPITask); not used
    xTaskCreate(maintask, "Main task", 100, NULL, 2, NULL);
    vTaskStartScheduler();
    //should never be reached
    while(1){
        
    }
    return (EXIT_SUCCESS);
}

