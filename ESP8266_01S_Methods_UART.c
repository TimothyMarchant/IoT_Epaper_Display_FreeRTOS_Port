#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Parse_short.h"
#include "definitions.h"
#include "SPI_Methods.h"
#include "GPIO.h"
#include "UART_Methods.h"
#include "Sleep.h"
#include "ESP_Command_Strings.h"
extern QueueHandle_t UART_Receive_Queue;
extern QueueHandle_t SPI_Queue;
#ifndef maxdigits_short
#define maxdigits_short 5
#endif
#ifndef zeroasciivalue
#define zeroasciivalue 48
#endif
#define ATSENDSTART_VAR_STARTPOS 11
//Expected response array.
#define ATResponseSize 75
volatile unsigned char ATResponse[ATResponseSize] = {};
extern SemaphoreHandle_t ESP_Image_Received;
extern SemaphoreHandle_t Epaper_INIT_finished;
void SetID(void);
_Bool IsExpectedMessage(const char* ExpectedResponse){
    for (unsigned char i=0;i<strlen(ExpectedResponse);i++){
        if (ATResponse[i]!=*(ExpectedResponse+i)){
            return 0;
        }
    }
    return 1;
}
void ClearATResponse(void){
    for (unsigned char i=0;i<ATResponseSize;i++){
       ATResponse[i]=0; 
    }
}
unsigned char GetConnectionStatus(void){
    UART_Begin(strlen(ATCIPSTATUS),strlen(ATConnectedToServerResponse),ATResponse);
    UART_sendstring(ATCIPSTATUS);
    UART_Wait;
    //default to this value.  If it fails it won't change.
    unsigned char status=FailedToGetStatus;
    _Bool IsStatusMessage=IsExpectedMessage(ATStatusString);
    if (IsStatusMessage){
        status=convertcharnumtonum(ATResponse[strlen(ATStatusString)]);
    }
    return status;
}
void disable_echo(void) {
    //disable echo; discard response
    UART_Begin(strlen(ATE0), 6,NULL);
    UART_sendstring(ATE0);
    UART_Wait;
}
//Get Epaper image for the display.
unsigned char StartConnection(void){
    UART_Begin(strlen(TCPSTART), 15,ATResponse);
    UART_sendstring(TCPSTART);
    UART_Wait;
    return 1;
}
//call this before sending data.  Tells ESP how much data to send.
void TCPSendstart_UART(const char* TCPSENDSTARTLEN){
    //expected response is \r\nOK\r\n>.
    UART_Begin(strlen(TCPSENDSTARTLEN), 7,NULL);
    UART_sendstring(TCPSENDSTARTLEN);
    UART_Wait;
    ClearATResponse();
}
//for strings of varying length.
void TCPSendStart_UART_Varying(unsigned short msglen){
    //msglen must be greater than zero.
    if (msglen==0){
        return;
    }
    //we only have 5 digits
    char digits[5]={0,0,0,0,0};
    parseshort(digits,msglen);
    unsigned char currentdigit=0;
    unsigned char TCPSendStart_Varying[20]="AT+CIPSEND=";
    for (unsigned char i=0;i<maxdigits_short;i++){
        if (digits[i]!=0){
            TCPSendStart_Varying[ATSENDSTART_VAR_STARTPOS+currentdigit]=digits[i];
            currentdigit++;
        }
    }
    //required \r\n at the end.
    TCPSendStart_Varying[ATSENDSTART_VAR_STARTPOS+currentdigit]='\r';
    TCPSendStart_Varying[ATSENDSTART_VAR_STARTPOS+currentdigit+1]='\n';
    UART_Begin(ATSENDSTART_VAR_STARTPOS+currentdigit+2, 6,NULL);
    UART_sendstring(TCPSendStart_Varying);
    UART_Wait;
}
//expectedresponselength must be less than or equal to 75.
//messages should be saved as macros and have a specific use case in mind.  
//In other words they are not dynamic.  They are defined when the program is compiled.
void SendMessage_UART(const char* Message,const unsigned char expectedresponselength){
    //do not allow responses greater than 75 or for empty messages
    if (expectedresponselength>75||strlen(Message)<=0){
        return;
    }
    TCPSendstart_UART(Message);
    UART_Begin(strlen(Message),expectedresponselength,ATResponse);
    UART_sendstring(Message);
    UART_Wait;
}
void TCP_Close_Socket(void){
    UART_Begin(strlen(CLOSETCPSOCKET), 14,ATResponse);
    UART_sendstring(CLOSETCPSOCKET);
    UART_Wait;
}
void SendQuitString(void){
    TCPSendstart_UART(QuitSENDSTART);
    
    UART_Begin(strlen(Quit_string),QuitResponselength+RECVxBytesStringResponse,ATResponse);
    UART_sendstring(Quit_string);
    UART_Wait;
}
#ifndef TCPSegmentImageInfo
#define TCPSegmentImageInfo
#define TCPSegmentLength 1000
#define TCPSegmentnum 5
#define ImageUARTPacketLength RECVxBytesStringResponse+ImageIPDSTRINGlength+TCPSegmentLength
#endif
unsigned char TransmitImageSPI(void){
    unsigned char data=0;
    for (unsigned char i=0;i<TCPSegmentnum;i++){
        TCPSendstart_UART(ACKSENDSTART);
        UART_Begin(strlen(ACK),ImageUARTPacketLength,NULL);
        UART_sendstring(ACK);
        while (data!=':'){
            if (xQueueReceive(UART_Receive_Queue,&data,pdMS_TO_TICKS(1000))==pdFALSE){
                return 0xff;
            }
        }
        for (unsigned short j=0;j<TCPSegmentLength;j++){
            if (xQueueReceive(UART_Receive_Queue,&data,pdMS_TO_TICKS(1000))==pdFALSE){
                return 0xf0;
            }
        SPI_Write_BLOCKING(data);
        }
        UART_Wait;
    }
    
    return 1;
}
//Get image data from server application for displaying on screen.
void GetImage(void) {
    //This is here until I can find a better spot for it.
    SetID();
    //connect to server
    StartConnection();
    //select message length.  We are trying to get an image.
    TCPSendstart_UART(GetImageSENDSTART);
    //the expected response is IDFOUND as the SetID should of been called before this.
    UART_Begin(strlen(GetImage_string),strlen(IDFOUND)+RECVxBytesLENGTH,ATResponse);
    UART_sendstring(GetImage_string);
    UART_Wait;

    //wait for Epaper initialization.  If it doesn't initialize close the socket.
    if (xSemaphoreTake(Epaper_INIT_finished,pdMS_TO_TICKS(1000))==pdFALSE){
        TCP_Close_Socket();
        return;
    }
    
    //transfer UART data to SPI
    if (TransmitImageSPI()!=1){
        return;
    }
    xSemaphoreGive(ESP_Image_Received);
    //close socket
    ClearATResponse();
    SendQuitString();
}
//meant for later
//Get ID from a future database to determine what image to get.  The server will handle this.
//Only called once.
void GetID(void){
    
}
//called on startup.
void SetID(void){
    disable_echo();
    //connect to server
    StartConnection();
    //select message length.  We are trying to get an image.
    TCPSendstart_UART(SetIDSENDSTART);
    //the expected response is IDFOUND as the SetID should of been called before this.
    UART_Begin(strlen(SetID_string),IDRESPONSELENGTH+RECVxBytesLENGTH,ATResponse);
    UART_sendstring(SetID_string);
    UART_Wait;
    //close socket
    SendQuitString();
}