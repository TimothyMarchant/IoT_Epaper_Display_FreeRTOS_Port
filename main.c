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
/*
 * 
 */
void (* functptr)(void);
void Init_Epaper_IO(void);
void testscreen(void);
void updatescreen(void);
SemaphoreHandle_t semaphore;
void EIC0_Callback(void){
    functptr=&testscreen;
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(semaphore,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}
void EIC1_Callback(void){
    functptr=&testscreen;
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(semaphore,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}
void SetupHardware(void){
    Init_IO();
    Init_Epaper_IO();
    Init_EIC(EIC0,0);
    Set_EIC0(RISE);
    Set_EIC1(RISE);
    Enable_EIC();
    InitSPI(2);
    InitUART();
    EnableSPI();
}
void maintask(void * pvParameters){
    while (1){
        xSemaphoreTake(semaphore,portMAX_DELAY);
        functptr();
    }
}
int main(void) {
    SetupHardware();
    semaphore=xSemaphoreCreateBinary();
    xTaskCreate(maintask,"Main task",128,NULL,1,NULL);
    vTaskStartScheduler();
    while (1){
        pinwrite(PORT_PA03,LOW);
    }
    return (EXIT_SUCCESS);
}

