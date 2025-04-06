#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include "UART_Methods.h"
#include "SPI_Methods.h"
//can be modified by other tasks
static volatile unsigned short packetlength=0;
static volatile unsigned int CS=0x00;
extern SemaphoreHandle_t SPIready;
extern SemaphoreHandle_t SPIFinished;
extern QueueHandle_t SPI_Queue;
//call these from SPI_Begin.
void SetPacketlength(const unsigned short length){
    packetlength=length;
}
void SetCS(const unsigned int CSpin){
    CS=CSpin;
}
//called from ISR
void SPI_Callback(void){
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(SPIready,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}
/*
 * SPI_Task is where SPI transfer are done.
 * We suspend the task until SPI_Begin is called
 * We then wait for data to arrive in the queue (whenever SPI_Enqueue is called
 * Then data is sent
 * Then we wait for the ISR to interrupt and give SPIready semaphore (the transfer has been completed)
 * Do this for the entire packet length.
 * Lastly end SPI communication by setting CS HIGH and giving the SPIFinished semaphore.
 */
void SPI_task(void * pvParameters){
    //there should never be a timeout, but just in case we will deal with one
    BaseType_t isTimeout=pdTRUE;
    unsigned char data;
    while (1){
        packetlength=0;
        data=0;
        //suspend this task until we are told to start the transfer
        vTaskSuspend(NULL);
        for (unsigned short i=0;i<packetlength;i++){
            //wait for data here wait no more than 100ms.  Something went wrong if we're waiting that long
            isTimeout=xQueueReceive(SPI_Queue,&data,portMAX_DELAY);
            //break loop and suspend task something went wrong.
            if (isTimeout==errQUEUE_EMPTY){
                //break;
            }
            //send data
            SPI_Write(data);
            //wait for semaphore, wait at most 100ms.  If we do not progress something happened!
            isTimeout=xSemaphoreTake(SPIready,portMAX_DELAY);
            if (isTimeout==pdFALSE){
                //break;
            }
        }
        //end communication here
        SPI_End(CS);
        xSemaphoreGive(SPIFinished);
    }
}