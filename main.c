/* 
 * File:   main.c
 * Author: Timothy Marchant
 *
 * Created on March 31, 2025, 12:47 AM
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
#include "CPUconfig.h"
/*
 * 
 */
void (* functptr)(void);
void updatescreen(void);
void UART_task(void*);
void SPI_task(void*);
SemaphoreHandle_t semaphore;
SemaphoreHandle_t SPIFinished;
SemaphoreHandle_t TXready;
SemaphoreHandle_t RXready;
SemaphoreHandle_t UARTFinished;
SemaphoreHandle_t SPIready;
QueueHandle_t SPI_Queue;
QueueHandle_t UART_Transmit_Queue;
//generic UART receiver queue
QueueHandle_t UART_Receive_Queue;
TaskHandle_t SPITask;
TaskHandle_t UARTTask;
void EIC0_Callback(void){
    functptr=&testscreen;
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(semaphore,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}
void EIC1_Callback(void){
    functptr=&updatescreen;
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(semaphore,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}
void SetupHardware(void){
    //configCPUspeed(Sixteen_MHz);
    Init_IO();
    Init_Epaper_IO();
    Init_EIC(EIC0|EIC1,0);
    Set_EIC0(RISE);
    Set_EIC1(RISE);
    Enable_EIC();
    InitSPI(2);
    InitUART();
    EnableUART();
    EnableSPI();
}
void maintask(void * pvParameters){
    //BaseType_t freesize=
    while (1){
        xSemaphoreTake(semaphore,portMAX_DELAY);
        functptr();
    }
}
int main(void) {
    SetupHardware();
    semaphore=xSemaphoreCreateBinary();
    SPIFinished=xSemaphoreCreateBinary();
    SPIready=xSemaphoreCreateBinary();
    TXready=xSemaphoreCreateBinary();
    RXready=xSemaphoreCreateBinary();
    UARTFinished=xSemaphoreCreateBinary();
    //create byte queue
    UART_Transmit_Queue=xQueueCreate(25,sizeof(unsigned char));
    UART_Receive_Queue=xQueueCreate(20,sizeof(unsigned char));
    SPI_Queue=xQueueCreate(5,sizeof(unsigned char));
    xTaskCreate(UART_task,"UART task",75,NULL,3,&UARTTask);
    xTaskCreate(SPI_task,"SPI task",50,NULL,3,&SPITask);
    xTaskCreate(maintask,"Main task",128,NULL,2,NULL);
    vTaskStartScheduler();
    //should never be reached
    while (1){
    }
    return (EXIT_SUCCESS);
}

