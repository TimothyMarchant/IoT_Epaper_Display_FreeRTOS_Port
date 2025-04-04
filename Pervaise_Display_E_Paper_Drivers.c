#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "SPI_Methods.h"
#include "GPIO.h"
#include "Timer0_Methods.h"
#include "UART_Methods.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#define delay(ms) vTaskDelay(pdMS_TO_TICKS(ms))
#define UART SERCOM1_REGS->USART_INT
#define SPI SERCOM0_REGS->SPIM
#define RXC 0x04
//outputs
#define DC PORT_PA15
#define RES PORT_PA08
//input
#define Busy PORT_PA14
#define CS PORT_PA16
//I took this from my previous working of this screen
#define Softreset 0x12
#define TemperatureREG 0x1A
#define PanelSetting 0x22
#define PanelSettingData 0xD7
#define DTM1REG 0x24
#define DTM2REG 0x26
#define UpdateDisplay 0x20
//image size in bytes
unsigned short imagelength=5000;
unsigned short currentcount=0;
unsigned char currentpacketlengthptr=0;
//TCP packet lengths we know in advanced.  These will not change as the image size is 5000 bytes
unsigned short packetlengths[4]={1460,1460,1460,620};
unsigned short packetlengthcount=0;
unsigned char isImagedata=0;
unsigned char i=0;
void TestSend(void);
//this is to write image data; this is not the most optimal way to write this, but this is just to test things.
//message format is +IPD,<LENGTH>:<DATA> where LENGTH<=1460
void EpaperReadWrite_UART_Callback(unsigned char data){
    if (!isImagedata){
        //this is how the ESP sends data back something, length and finally : afterwards is all data.
        if (data==':'){
            isImagedata=1;
        }
        return;
    }
        //Normally I would have a blocking loop here, but since our UART perhiperial transfers so slowly it's strictly unnecessary (since our SPI speed is pretty fast in comparison)
        //while (!(SPI.SERCOM_INTFLAG&0x01));
        SPI.SERCOM_DATA = data;
        packetlengthcount++;
        //if this is true do not send more data until the original conditional is satisfied again.
        if (packetlengthcount==packetlengths[i]){
            i++;
            packetlengthcount=0;
            isImagedata=0;
        }
        currentcount++;
        //disable interrupt and stop transmission
        if (currentcount==imagelength){
            UART.SERCOM_INTENCLR=RXC;
            SPI_End(CS);
            i=0;
            currentcount=0;
        }
        return;
    
}
//only called once
void Init_Epaper_IO(void){
    configpin(DC,Output);
    configpin(RES,Output);
    configpin(CS,Output);
    configpin(Busy,Input);
    pinwrite(CS,HIGH);
    pinwrite(DC,HIGH);
    //pinwrite(RES,LOW);
}
//send a command
void sendcommand(unsigned char CMD){
    pinwrite(DC,LOW);
    SPI_Write_Blocking(CMD);
    pinwrite(DC,HIGH);
}
//Pervaise Displays wants you to stop powering the screen once you are done writing to it, so this would be called more than once.
//However for this project we will skip that for now.
void Init_Screen(void){
    delay(30);
    configpin(RES,Output);
    pinwrite(RES,HIGH);
    delay(25);
    pinwrite(RES,LOW);
    delay(25);
    pinwrite(RES,HIGH);
    delay(35);
    //wait for busy to go LOW
    while (pinread(Busy,14)>=1);
    pinwrite(DC,LOW);
    SPI_Start_Unknown_Packet(CS);
    SPI_Write_Blocking(Softreset);
    pinwrite(DC,HIGH);
    SPI_End(CS);
    while (pinread(Busy,14)>=1);
    SPI_Start_Unknown_Packet(CS);
    sendcommand(TemperatureREG);
    //temperature
    SPI_Write_Blocking(0x14);
    sendcommand(PanelSetting);
    SPI_Write_Blocking(PanelSettingData);
    SPI_End(CS);
}
//testing purposes
void testsendbuffer(void){
    SPI_Start_Unknown_Packet(CS);
    sendcommand(DTM1REG);
    for (unsigned short i=0;i<5000;i++){
        SPI_Write_Blocking(0x00);
    }
    SPI_End(CS);
    SPI_Start_Unknown_Packet(CS);
    sendcommand(DTM2REG);
    for (unsigned short i=0;i<5000;i++){
        SPI_Write_Blocking(0x00);
    }
    SPI_End(CS);
    //update display
    while(pinread(Busy,14));
    SPI_Start_Unknown_Packet(CS);
    sendcommand(0x20);
    SPI_End(CS);
    while(pinread(Busy,14));
}
//send necessary data to update screen
void updatescreen(void){
    Init_Screen();
    SPI_Start_Unknown_Packet(CS);
    sendcommand(DTM1REG);
    TestSend();
    SPI_End(CS);
    SPI_Start_Unknown_Packet(CS);
    sendcommand(DTM2REG);
    for (unsigned short i=0;i<5000;i++){
        SPI_Write_Blocking(0x00);
    }
    SPI_End(CS);
    //update display
    while(pinread(Busy,14));
    SPI_Start_Unknown_Packet(CS);
    sendcommand(0x20);
    SPI_End(CS);
    while(pinread(Busy,14));
}
//For testing purposes
void testscreen(void){
    Init_Screen();
    testsendbuffer();
}