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
#define RXCaller 0x04
#define TXCaller 0x02
extern QueueHandle_t UART_Transmit_Queue;
extern QueueHandle_t UART_Receive_Queue;
extern SemaphoreHandle_t TXready;
extern SemaphoreHandle_t RXready;
extern SemaphoreHandle_t UARTFinished;

//points to another queue that will receive incoming data
//QueueHandle_t * UART_Receive_Queue_ptr;
static volatile unsigned short TransmitLength=0;
static volatile unsigned short ReceiveLength=0;
static volatile unsigned char UARTdata=0;
void UART_CallbackTX(void){
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(TXready,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
}
void UART_CallbackRX(void){
    BaseType_t higherprioritytask=pdFALSE;
    xSemaphoreGiveFromISR(RXready,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
    //required for clear RXC flag
    UARTdata=UART_Read();
}
void SetPacketLengths(unsigned short TXlength,unsigned short RXlength){
    TransmitLength=TXlength;
    ReceiveLength=RXlength;
}
//receiver queue
void SetReceiveQueue(QueueHandle_t queue){
    //UART_Receive_Queue_ptr=&queue;
}
//use static inline so compiler knows to optimize these functions
//transmit sequence similiar to SPI_task.
static inline void TransmitSequence(void){
    for (unsigned short i=0;i<TransmitLength;i++){
        xQueueReceive(UART_Transmit_Queue,(unsigned char *)&UARTdata,portMAX_DELAY);
        //UART write
        UART_Write(UARTdata);
        //wait for transmission to finish
        xSemaphoreTake(TXready,portMAX_DELAY);
    }
}
static inline void ReceiveSequence(void){
    for (unsigned short i=0;i<ReceiveLength;i++){
        //wait for data on RX line
        xSemaphoreTake(RXready,portMAX_DELAY);
        //read data
        
        //put data in a queue to be read elsewhere.
        xQueueSendToBack(UART_Receive_Queue,(unsigned char *)&UARTdata,portMAX_DELAY);
    }
}
void UART_task(void * pvParameters){
    
    
    while (1){
        
        UARTdata=0;
        TransmitLength=0;
        ReceiveLength=0;
        vTaskSuspend(NULL);
        if (TransmitLength>0){
            TransmitSequence();
        }
        if (ReceiveLength>0){
            ReceiveSequence();
        }
        xSemaphoreGive(UARTFinished);
    }
}