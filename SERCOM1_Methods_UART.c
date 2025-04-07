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
void SetPacketLengths(unsigned short,unsigned short);
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
    UART.SERCOM_INTFLAG=defaultinterrupts;
    //yes the flag sometimes is high before we've started.
    if (UART.SERCOM_INTFLAG&0x04){
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
void UART_Begin(unsigned short TLength,unsigned short RLength,QueueHandle_t receiverqueue){
    SetPacketLengths(TLength,RLength);
    SetReceiveQueue(receiverqueue);
    vTaskResume(UARTTask);
    Enableinterrupt();
}

void UART_Enqueue_Transmit(unsigned char data){
    xQueueSendToBack(UART_Transmit_Queue,&data,portMAX_DELAY);
}
void UART_sendstring(const char*string){
    for (unsigned char i=0;i<strlen(string);i++){
        UART_Enqueue_Transmit((unsigned char) *(string+i));
    }
}
void UART_Write(unsigned char data){
    UART.SERCOM_DATA=data;
}
volatile unsigned char UART_Read(void){
    volatile unsigned char data=UART.SERCOM_DATA;
    return data;
}
void UART_Wait_For_End_Of_Transmission(void){
    xSemaphoreTake(UARTFinished,portMAX_DELAY);
    Disableinterrupt();
    FlushReceiveQueue();
}
void FlushReceiveQueue(void){
    unsigned char data=0;
    while (xQueueReceive(UART_Receive_Queue,&data,0)==pdPASS){
        
    }
}