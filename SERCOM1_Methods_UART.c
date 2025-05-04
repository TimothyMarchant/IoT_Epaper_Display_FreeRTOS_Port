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
#include "Sleep.h"
#define CTRLBmask 0x00030000
#define CTRLAmask 0x40100084
//value needed for 115200 baud rate
#define requiredbaudvalue 35337
//value needed for 9600 baud rate (approximately)
#define normalbaudvalue 63019
#define UART SERCOM1_REGS->USART_INT
#define RXC_Flag 0x04
#define TXC_Flag 0x02
#define GCLKPERDefaultMask 0x40
//enable specific interrupts.  These are the ones we want to activate normally
#define defaultinterrupts 0x06 //RXC and TXC bits
#define enablebit 0x02
unsigned char* datatoread;
const unsigned char* transmissionpacket;
volatile unsigned char * receiverarray=NULL;
volatile unsigned char DoNeedToSaveResponse=0;
unsigned short packetlengthT = 0;
unsigned short packetlengthR = 0;

//transmission pointer
volatile unsigned short packetpointerT = 0;
//receiving pointer
volatile unsigned short packetpointerR = 0;
unsigned char IsTransferingToSPI = 0;
unsigned char validdata = 0;
extern QueueHandle_t UART_Transmit_Queue;
extern QueueHandle_t UART_Receive_Queue;
extern TaskHandle_t UARTTask;
extern SemaphoreHandle_t UARTFinished;
//meant to be called from other files
void Disableinterrupt(void);
void SetReceiveQueue(QueueHandle_t);
void SetPacketLengths(unsigned short, unsigned short);
void EpaperReadWrite_UART_Callback(unsigned char);
void FlushReceiveQueue(void);
unsigned char UART_Read(void);

void InitUART(void) {
    //activate peripherial
    GCLK_REGS->GCLK_PCHCTRL[12] = GCLKPERDefaultMask;
    configpin(PORT_PA01, Input);
    configpin(PORT_PA00, Output);
    pinmuxconfig(0, GROUPD); //SERCOM1 [0] TX
    pinmuxconfig(1, GROUPD); //SERCOM1 [1] RX
    UART.SERCOM_BAUD = normalbaudvalue;
    UART.SERCOM_CTRLA = CTRLAmask;
    UART.SERCOM_CTRLB = CTRLBmask;
    //needs to be higher priority than other interrupts
    NVIC_SetPriority(SERCOM1_1_IRQn, 2);
    NVIC_SetPriority(SERCOM1_2_IRQn, 1);
}
//turn on sercom1

void EnableUART(void) {
    UART.SERCOM_CTRLA |= enablebit;
}
//turn off sercom1

void DisableUART(void) {
    UART.SERCOM_CTRLA &= ~(enablebit);
}

void Enableinterrupt(void) {
    UART.SERCOM_INTENSET = defaultinterrupts;
    UART.SERCOM_INTFLAG = defaultinterrupts;
    //The RXC flag is high sometimes before I even enable interrupts or the UART module for some reason.
    //just in case it's set, we have to read from the register to clear it.
    if (UART.SERCOM_INTFLAG & 0x04) {
        UART_Read();
    }
    NVIC_EnableIRQ(SERCOM1_1_IRQn);
    NVIC_EnableIRQ(SERCOM1_2_IRQn);
}

void Disableinterrupt(void) {
    UART.SERCOM_INTENCLR = defaultinterrupts;
    NVIC_DisableIRQ(SERCOM1_1_IRQn);
    NVIC_DisableIRQ(SERCOM1_2_IRQn);
}

void UART_Begin(unsigned short TLength, unsigned short RLength,volatile unsigned char* Receiverarr) {
    SetPacketLengths(TLength, RLength);
    receiverarray=Receiverarr;
    //whether or not we want to save the response or not.  The Response may not be useful and we can throw it away.
    if (receiverarray==NULL){
        DoNeedToSaveResponse=0;
    }
    //Response is needed for something
    else {
        DoNeedToSaveResponse=1;
    }
    vTaskResume(UARTTask);
    Enableinterrupt();
}

void UART_Enqueue_Transmit(unsigned char data) {
    xQueueSendToBack(UART_Transmit_Queue, &data, portMAX_DELAY);
}

void UART_sendstring(const char*string) {
    for (unsigned short i = 0; i < strlen(string); i++) {
        UART_Enqueue_Transmit((unsigned char) *(string + i));
    }
}
//for nonconstant size strings (has few use cases)
void UART_sendarray(unsigned char*arr){
    for (unsigned short i = 0; i < strlen(arr); i++) {
        UART_Enqueue_Transmit((unsigned char) *(arr + i));
    }
}
void UART_Write(unsigned char data) {
    UART.SERCOM_DATA = data;
}

volatile unsigned char UART_Read(void) {
    volatile unsigned char data = UART.SERCOM_DATA;
    return data;
}

void UART_Wait_For_End_Of_Transmission(void) {
    //wait for UART to be finished
    xSemaphoreTake(UARTFinished, portMAX_DELAY);
    Disableinterrupt();
    //Empty the response into the array from the start method or just discard the response.
    FlushReceiveQueue();
}
//remove contents of UART_Receive_Queue into either an array or just get rid of it altogether.
void FlushReceiveQueue(void) {
    unsigned char i = 0;
    if (DoNeedToSaveResponse){
        while (xQueueReceive(UART_Receive_Queue,(void *) (receiverarray+i),0)==pdPASS){
            i++;
        }
    }
    else {
        while (xQueueReceive(UART_Receive_Queue, &i, 0) == pdPASS) {

        }
    }
}