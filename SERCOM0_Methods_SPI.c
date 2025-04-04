//This is not for general use, we are only transmitting
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
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
volatile unsigned char databuffer = 0x00;
volatile unsigned short packetlength = 0;
volatile unsigned short packetpointer = 0;
volatile unsigned char* Packet;
volatile unsigned char Repeatedsendmode = 0;
volatile unsigned int currentCS = 0;
static inline void Disableinterrupts(void);
#define PA PORT_REGS->GROUP[0]
//void SPI_Queue_Callback(void);
volatile void SPI_End( volatile unsigned int pin);
//void SPI_Queue_Callback();

void __attribute__((interrupt)) SERCOM0_0_Handler(void) {
    if (SPI.SERCOM_INTFLAG & DRE) {
        if (!Repeatedsendmode) {
            //clears interrupt flag
            DataREG = (*(Packet + packetpointer));

        } 
        //for repeated sending.  There are a few use cases for this
        else {
            DataREG = *Packet;
        }
        packetpointer++;
    }
    //end SPI
    if (packetpointer == packetlength) {
        //blocking wait for last byte; should be very short
        while (!(SPI.SERCOM_INTFLAG & DRE));
        SPI_End(currentCS);
    }
}

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
    NVIC_SetPriority(SERCOM0_0_IRQn, 3);

}

void EnableSPI(void) {
    SPI.SERCOM_CTRLA |= 0x02;
}

void DisableSPI(void) {
    SPI.SERCOM_CTRLA &= ~(0x02);
}
//called by the same function, so use inline (supposedly the compiler should optimize)
static inline void Enableinterrupts(void) {
    SPI.SERCOM_INTENSET |= DRE;
    NVIC_EnableIRQ(SERCOM0_0_IRQn);
}

static inline void Disableinterrupts(void) {
    SPI.SERCOM_INTENCLR |= DRE;
    NVIC_DisableIRQ(SERCOM0_0_IRQn);
}

volatile void SPI_Start(const volatile unsigned char pin, const volatile unsigned short length, unsigned char* givenPacket) {
    pinwrite(pin, LOW);
    currentCS = pin;
    packetlength = length;
    Packet = givenPacket;
    Enableinterrupts();
}
//for packets of unknown length, or sending packets of very small length.  Use blocking write function (mainly meant for when I don't care enough to define packets)
void SPI_Start_Unknown_Packet(const volatile unsigned int pin) {
    currentCS = pin;
    pinwrite(pin, LOW);
}
//For writing the same value repeatedly. There is a legitimate use case for this which will be seen shortly
void SPI_Start_Repeated(const volatile unsigned char pin, const volatile unsigned short length, unsigned char data) {
    Repeatedsendmode = 1;
    SPI_Start(pin, length, &data);
}
//We are done transferring use regardless of which start method was chosen
volatile void SPI_End(volatile unsigned int pin) {
    pinwrite(pin, HIGH);
    Disableinterrupts();
    currentCS = 0x00;
    //reset everything
    Repeatedsendmode = 0;
    packetpointer = 0;
    packetlength = 0;
}
//this write method uses a blocking loop until we can write again.  For testing purposes mostly or small amount of writes

void SPI_Write_Blocking(unsigned char data) {
    while (!(SPI.SERCOM_INTFLAG & DRE));
    SPI.SERCOM_DATA = data;
    while (!(SPI.SERCOM_INTFLAG & DRE));
}
void ChangetoLSB(void){
    DisableSPI();
    SPI.SERCOM_CTRLA|=0x40000000;
    EnableSPI();
}
void ChangetoMSB(void){
    DisableSPI();
    SPI.SERCOM_CTRLA&=~(0x40000000);
    EnableSPI();
}