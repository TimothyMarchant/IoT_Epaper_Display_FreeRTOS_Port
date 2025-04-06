#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "SPI_Methods.h"
#include "GPIO.h"
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
#ifndef EpaperMacros
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
#define RoomTemperature 0x14
#endif
#ifndef SPIShorthand
#define Write(data) SPI_Enqueue(data) 
#define SPIWait SPI_Wait_For_Last_Byte()
#endif
void TestSend(void);
//this is to write image data; this is not the most optimal way to write this, but this is just to test things.
//message format is +IPD,<LENGTH>:<DATA> where LENGTH<=1460
void EpaperReadWrite_UART_Callback(unsigned char data){
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
    Write(CMD);
    pinwrite(DC,HIGH);
}
//Pervaise Displays wants you to stop powering the screen once you are done writing to it, so this would be called more than once.
//However for this project we will skip that for now.
void Init_Screen(void){
    delay(5);
    configpin(RES,Output);
    pinwrite(RES,HIGH);
    delay(10);
    pinwrite(RES,LOW);
    delay(10);
    pinwrite(RES,HIGH);
    delay(10);
    //wait for busy to go LOW
    while (pinread(Busy,14));
    SPI_Begin(CS,1);
    sendcommand(Softreset);
    SPIWait;
    while (pinread(Busy,14));
    SPI_Begin(CS,4);
    sendcommand(TemperatureREG);
    //temperature
    Write(RoomTemperature);
    sendcommand(PanelSetting);
    Write(PanelSettingData);
    SPIWait;
}
//testing purposes
void testsendbuffer(void){
    SPI_Begin(CS,5001);
    sendcommand(DTM1REG);
    for (unsigned short i=0;i<5000;i++){
        Write(0x00);
    }
    SPIWait;
    SPI_Begin(CS,5001);
    sendcommand(DTM2REG);
    for (unsigned short i=0;i<5000;i++){
        Write(0x00);
    }
    SPIWait;
    //update display
    while(pinread(Busy,14));
    SPI_Begin(CS,1);
    sendcommand(UpdateDisplay);
    SPIWait;
    while(pinread(Busy,14));
}
//send necessary data to update screen
void updatescreen(void){
    Init_Screen();
    SPI_Begin(CS,5001);
    sendcommand(DTM1REG);
    TestSend();
    SPIWait;
    SPI_Begin(CS,5001);
    sendcommand(DTM2REG);
    for (unsigned short i=0;i<5000;i++){
        Write(0x00);
    }
    SPIWait;
    //update display
    while(pinread(Busy,14));
    SPI_Begin(CS,2);
    sendcommand(UpdateDisplay);
    SPIWait;
    while(pinread(Busy,14));
}
//For testing purposes
void testscreen(void){
    Init_Screen();
    testsendbuffer();
}