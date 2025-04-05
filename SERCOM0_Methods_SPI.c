//This is not for general use, we are only transmitting
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include "GPIO.h"
//SERCOM0 SPIM
#define SPI SERCOM0_REGS->SPIM
#define DataREG SPI.SERCOM_DATA
//for enabling peripherals
#define GCLKSERCOM0 GCLK_REGS->GCLK_PCHCTRL[11]
#define CTRLBRegisterSettings 0x00000000
#define Transmit_Complete 0x02
#define DRE 0x01
#define GCLKPERDefaultMask 0x00000040
#define CTRLARegisterMask 0x1030008C
#define Success 1
#define Failure 0
extern QueueHandle_t SPI_Queue;
extern TaskHandle_t SPITask;
extern SemaphoreHandle_t SPIFinished;
void SetPacketlength(unsigned short);
void SetCS(const unsigned int CSpin);
static inline void Disableinterrupts(void);
#define PA PORT_REGS->GROUP[0]
//void SPI_Queue_Callback(void);
volatile void SPI_End( volatile unsigned int pin);

void InitSPI(const unsigned char baudrate) {
    //using SSOP24 package.  Enable pins for SERCOM0
    //pinmuxconfig(2, GROUPD); //pad[2] PA2 pin 7 not needed for transmission kept for reference
    //pinmuxconfig(3, GROUPD); //pad[3] PA3 pin 8 not needed for transmission kept for reference
    configpin(PORT_PA04,Output);
    pinwrite(PORT_PA04,LOW);
    configpin(PORT_PA05,Output);
    pinwrite(PORT_PA05,LOW);
    pinmuxconfig(4, GROUPD); //pad[0] PA4 pin 9 with current settings MOSI
    pinmuxconfig(5, GROUPD); //pad[1] PA5 pin 10 with current settings clock line
    //enable SERCOM0
    GCLKSERCOM0 = GCLKPERDefaultMask;
    SPI.SERCOM_BAUD = baudrate;
    SPI.SERCOM_CTRLB = CTRLBRegisterSettings;
    SPI.SERCOM_CTRLA = CTRLARegisterMask;
    NVIC_SetPriority(SERCOM0_1_IRQn, 3);

}

void EnableSPI(void) {
    SPI.SERCOM_CTRLA |= 0x02;
}

void DisableSPI(void) {
    SPI.SERCOM_CTRLA &= ~(0x02);
}
//called by the same function, so use inline (supposedly the compiler should optimize)
static inline void Enableinterrupts(void) {
    SPI.SERCOM_INTENSET = Transmit_Complete;
    NVIC_EnableIRQ(SERCOM0_1_IRQn);
}
static inline void Disableinterrupts(void) {
    SPI.SERCOM_INTENCLR =Transmit_Complete;
    NVIC_DisableIRQ(SERCOM0_1_IRQn);
}
void SPI_Begin(const unsigned int CS,const volatile unsigned short length){
    pinwrite(CS,LOW);
    SetPacketlength(length);
    SetCS(CS);
    Enableinterrupts();
    vTaskResume(SPITask);
}
//enqueue a byte
void SPI_Enqueue(unsigned char data){
    //wait at most 100 ms
    xQueueSendToBack(SPI_Queue,&data,portMAX_DELAY);
}
void SPI_Write(unsigned char data){
    SPI.SERCOM_DATA=data;
}
//wait for last byte to transfer.
void SPI_Wait_For_Last_Byte(void){
    //wait at most 100ms for spi to finish transfering last byte.  We should never get a timeout, but its here just in case
    xSemaphoreTake(SPIFinished,portMAX_DELAY);
}
//for packets of unknown length, or sending packets of very small length.  Use blocking write function (mainly meant for when I don't care enough to define packets)
void SPI_Start_Unknown_Packet(const volatile unsigned int pin) {
    pinwrite(pin, LOW);
}
//end communication, task suspends itself.
volatile void SPI_End(volatile unsigned int pin) {
    pinwrite(pin, HIGH);
    Disableinterrupts();
}
//this write method uses a blocking loop until we can write again.  For testing purposes mostly or small amount of writes

void SPI_Write_Blocking(unsigned char data) {
    while (!(SPI.SERCOM_INTFLAG & DRE));
    SPI.SERCOM_DATA = data;
    while (!(SPI.SERCOM_INTFLAG & DRE));
}