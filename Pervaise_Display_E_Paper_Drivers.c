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
#define Busy PORT_PA23
#define BusyPinnum 23
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
extern SemaphoreHandle_t ESP_Image_Received;
extern SemaphoreHandle_t Epaper_INIT_finished;
extern SemaphoreHandle_t Epaper_Finished;
extern SemaphoreHandle_t BusyLOW;
void EIC2_Callback(void){
    BaseType_t higherprioritytask = pdFALSE;
    xSemaphoreGiveFromISR(BusyLOW,&higherprioritytask);
    portYIELD_FROM_ISR(higherprioritytask);
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

static void sendcommand(unsigned char CMD) {
    pinwrite(DC, LOW);
    Write(CMD);
    pinwrite(DC, HIGH);
}
static void WaitForBusy(void){
    //if HIGH
    if (pinread(Busy,BusyPinnum)){
    xSemaphoreTake(BusyLOW,portMAX_DELAY);
    }
}
//Pervaise Displays wants you to stop powering the screen once you are done writing to it, so this would be called more than once.
//However for this project we will skip that for now.
//these values are in the datasheet for how long to wait.  Used only on setup.
#define DatasheetWait1 5
#define DatasheetWait2 10
static void Init_Screen(void) {
    delay(DatasheetWait1);
    configpin(RES, Output);
    pinwrite(RES, HIGH);
    delay(DatasheetWait2);
    pinwrite(RES, LOW);
    delay(DatasheetWait2);
    pinwrite(RES, HIGH);
    delay(DatasheetWait2);
    //wait for busy to go LOW
    WaitForBusy();
    StartSPI_BLOCKING(CS);
    sendcommand(Softreset);
    SPIWait;
    WaitForBusy();
    StartSPI_BLOCKING(CS);
    sendcommand(TemperatureREG);
    //temperature
    Write(RoomTemperature);
    sendcommand(PanelSetting);
    Write(PanelSettingData);
    SPIWait;
}
static void SendToDTM1REG(){
    StartSPI_BLOCKING(CS);
    sendcommand(DTM1REG);
    for (volatile unsigned short i = 0; i < 5000; i++) {
        Write(0);
    }
    SPIWait;
}
static void SendToDTM2REG(void){
    StartSPI_BLOCKING(CS);
    sendcommand(DTM2REG);
    for (volatile unsigned short i = 0; i < 5000; i++) {
        Write(0);
    }
    SPIWait;
}
static void SendUpdateCMD(void){
    //update display
    WaitForBusy();
    StartSPI_BLOCKING(CS);
    sendcommand(UpdateDisplay);
    SPIWait;
    WaitForBusy();
}
static void GiveEpaperFinished(void){
    xSemaphoreGive(Epaper_Finished);
}
//testing purposes

void testsendbuffer(void) {
    SendToDTM1REG();
    SendToDTM2REG();
    //update display
    SendUpdateCMD();
}
//send necessary data to update screen

void updatescreen(void) {
    Init_Screen();
    StartSPI_BLOCKING(CS);
    sendcommand(DTM1REG);
    xSemaphoreGive(Epaper_INIT_finished);
    //wait at most 10 seconds for the image to be received.  if for some reason it's not received abort.
    xSemaphoreTake(ESP_Image_Received,pdMS_TO_TICKS(30000));
    SPIWait;
    StartSPI_BLOCKING(CS);
    SendToDTM2REG();
    SendUpdateCMD();
    GiveEpaperFinished();
}
//For testing purposes

void ClearScreen(void) {
    Init_Screen();
    testsendbuffer();
    GiveEpaperFinished();
}