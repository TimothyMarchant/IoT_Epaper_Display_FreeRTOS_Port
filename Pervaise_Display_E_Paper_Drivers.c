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
#define SPIShorthandRTOS 0
//use RTOS task methods.
#ifdef SPIShorthandRTOS
#if SPIShorthandRTOS == 1
#define Write(data) SPI_Enqueue(data) 
#define SPIWait SPI_Wait_For_Last_Byte()
//use blocking methods
#else
#define Write(data) SPI_Write_BLOCKING(data)
#define SPIWait EndSPI_BLOCKING(CS)
#endif
#endif
void GetImage(void);
//this is to write image data; this is not the most optimal way to write this, but this is just to test things.
//message format is +IPD,<LENGTH>:<DATA> where LENGTH<=1460

void EpaperReadWrite_UART_Callback(unsigned char data) {
}
//only called once

void Init_Epaper_IO(void) {
    configpin(DC, Output);
    configpin(RES, Output);
    configpin(CS, Output);
    configpin(Busy, Input);
    pinwrite(CS, HIGH);
    pinwrite(DC, HIGH);
}
//send a command

void sendcommand(unsigned char CMD) {
    pinwrite(DC, LOW);
    Write(CMD);
    pinwrite(DC, HIGH);
}
//Pervaise Displays wants you to stop powering the screen once you are done writing to it, so this would be called more than once.
//However for this project we will skip that for now.
//these values are in the datasheet for how long to wait.  Used only on setup.
#define DatasheetWait1 5
#define DatasheetWait2 10
void Init_Screen(void) {
    delay(DatasheetWait1);
    configpin(RES, Output);
    pinwrite(RES, HIGH);
    delay(DatasheetWait2);
    pinwrite(RES, LOW);
    delay(DatasheetWait2);
    pinwrite(RES, HIGH);
    delay(DatasheetWait2);
    //wait for busy to go LOW
    while (pinread(Busy, 14));
    StartSPI_BLOCKING(CS);
    sendcommand(Softreset);
    SPIWait;
    while (pinread(Busy, 14));
    StartSPI_BLOCKING(CS);
    sendcommand(TemperatureREG);
    //temperature
    Write(RoomTemperature);
    sendcommand(PanelSetting);
    Write(PanelSettingData);
    SPIWait;
}
//testing purposes

void testsendbuffer(void) {
    StartSPI_BLOCKING(CS);
    sendcommand(DTM1REG);
    for (unsigned short i = 0; i < 5000; i++) {
        Write(0x00);
    }
    SPIWait;
    StartSPI_BLOCKING(CS);
    sendcommand(DTM2REG);
    for (unsigned short i = 0; i < 5000; i++) {
        Write(0x00);
    }
    SPIWait;
    //update display
    while (pinread(Busy, 14));
    StartSPI_BLOCKING(CS);
    sendcommand(UpdateDisplay);
    SPIWait;
    while (pinread(Busy, 14));
}
//send necessary data to update screen

void updatescreen(void) {
    Init_Screen();
    StartSPI_BLOCKING(CS);
    sendcommand(DTM1REG);
    GetImage();
    SPIWait;
    StartSPI_BLOCKING(CS);
    sendcommand(DTM2REG);
    for (unsigned short i = 0; i < 5000; i++) {
        Write(0x00);
    }
    SPIWait;
    //update display
    while (pinread(Busy, 14));
    StartSPI_BLOCKING(CS);
    sendcommand(UpdateDisplay);
    SPIWait;
    while (pinread(Busy, 14));
}
//For testing purposes

void testscreen(void) {
    Init_Screen();
    testsendbuffer();
}