/*Unit tests file.  Unit test certain methods with dummy data.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "ESP_Methods.h"
#include "ESP_Command_Strings.h"
//meant to determine if we need to compile code in this file.
#ifndef IsTesting
#define IsTesting 1
#endif
//Expected response array.
#ifndef ATResponseSize
#define ATResponseSize 75
#endif
//cause endless loop if test fails.
#ifndef C_ASSERT
#define C_ASSERT(Given,Expected) while(Given!=Expected);
#endif
extern volatile unsigned char ATResponse[ATResponseSize];
extern QueueHandle_t UART_Receive_Queue;
extern volatile unsigned char DoNeedToSaveResponse;
extern volatile unsigned char * receiverarray;
//only compile if we are actively testing.
#if IsTesting == 1
BaseType_t IsSuccessful=1;
void InsertstringintoResponse(const char* teststring){
    for (unsigned int i=0;i<strlen(teststring);i++){
        ATResponse[i]=*(teststring+i);
    }
}
void UnitTest_GetConnectionStatus(void){
    InsertstringintoResponse(ATErrorResponse);
    unsigned char status=GetConnectionStatus();
    C_ASSERT(status,0xFF);
    ClearATResponse();
    InsertstringintoResponse(ATConnectedToServerResponse);
    status=GetConnectionStatus();
    C_ASSERT(status,3);
    ClearATResponse();
    InsertstringintoResponse(ATConnectedToWiFi);
    status=GetConnectionStatus();
    C_ASSERT(status,2);
    
}
void UnitTest_FlushReceiveQueue(void){
    receiverarray=ATResponse;
    DoNeedToSaveResponse=1;
    for (unsigned char i=0;i<5;i++){
        xQueueSendToBack(UART_Receive_Queue,&i,portMAX_DELAY);
    }
    FlushReceiveQueue();
    for (unsigned char i=0;i<5;i++){
        C_ASSERT(*(receiverarray+i),i);
    }
    DoNeedToSaveResponse=0;
    for (unsigned char i=0;i<5;i++){
        xQueueSendToBack(UART_Receive_Queue,&i,portMAX_DELAY);
    }
    FlushReceiveQueue();
    unsigned char t=0;
    //assert that queue is empty
    BaseType_t temp=xQueueReceive(UART_Receive_Queue,&t,0);
    C_ASSERT(temp,errQUEUE_EMPTY);
}
void TestEpaper(void){
    
}
void testmethods(void){
    UnitTest_GetConnectionStatus();
    UnitTest_FlushReceiveQueue();
}
#endif