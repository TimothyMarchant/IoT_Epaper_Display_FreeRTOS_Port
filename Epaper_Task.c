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
#include "EPaper_Methods.h"
volatile void (* ScreenMethod)(void);
void SetScreenMethod(volatile void (*Method)(void)){
    
    ScreenMethod=Method;
}
void Epaper_Task(void * pvParameters){
    
    while (1){
        vTaskSuspend(NULL);
        ScreenMethod();
    }
}
