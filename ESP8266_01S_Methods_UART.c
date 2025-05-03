#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "SPI_Methods.h"
#include "GPIO.h"
#include "UART_Methods.h"
#include "Sleep.h"
extern QueueHandle_t UART_Receive_Queue;
extern QueueHandle_t SPI_Queue;
#ifndef ATstrings
//nothing should change for this
#define ATString "AT\r\n"
//disable echo
#define ATE0 "ATE0\r\n"
//sleep for 1 second default sleep string will be used later
#define ESPSLEEP "AT+GSLP=1000\r\n"
//These strings would be needed to connect to a network, but this will be done off this microcontroller for privacy (don't want to expose my network)
//default WiFi mode; set to station mode
//const char* const ATCWMODE_DEF="AT+CWMODE_DEF=1";
//connect to this network with the correct SSID and password; for reference only
//const char* const ATCWJAP_DEF+"AT+CWJAP_DEF=\"NAME\",\"PASSWORD\"";
//get current status
#define ATCIPSTATUS "AT+CIPSTATUS\r\n"
//start the TCP connection
#define TCPSTART "AT+CIPSTART=\"TCP\",\"IP\",7777\r\n" //7777 is not special or anything it's just the port number I arbitarly chose.
#define TCPSENDSTART "AT+CIPSEND=1\r\n" //send this with the length of the message.
#define CLOSETCPSOCKET "AT+CIPCLOSE\r\n" //make sure to close the socket
//AT response array.
#define dummy "a"
//expected response from AT\r\n
#define ATErrorResponse "\r\nERROR\r\n"
#define ATTestResponse "\r\nOK\r\n"
#define ATCloseResponse "\r\nCLOSED\r\n\r\nOK\r\n"
//only the beginning of the response is needed.  The other info is just server information we don't need.
//meant for a quick comparison.
#define ATStatusString "\r\nSTATUS:"
#define ATConnectedToServerResponse "\r\nSTATUS:3\r\n"
#define ATConnectedToWiFi "\r\nSTATUS:2\r\n"
#define ATConnectedstrlen strlen(ATConnectedToServerResponse)
//return obvious failure value that isn't used.
#define FailedToGetStatus 0xFF
#define convertcharnumtonum(charnum) charnum-48
#endif
//Expected response array.
#define ATResponseSize 75
volatile unsigned char ATResponse[ATResponseSize] = {};
extern SemaphoreHandle_t ESP_Image_Received;
extern SemaphoreHandle_t Epaper_INIT_finished;
void StartUARTtoSPITransfer(void);
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
void StartConnection(void){
    UART_Begin(strlen(TCPSTART), 15,ATResponse);
    UART_sendstring(TCPSTART);
    UART_Wait;
}
void TCPSendstart_UART(void){
    UART_Begin(strlen(TCPSENDSTART), 6,NULL);
    UART_sendstring(TCPSENDSTART);
    UART_Wait;
}
void TCP_Close_Socket(void){
    UART_Begin(strlen(CLOSETCPSOCKET), 14,ATResponse);
    UART_sendstring(CLOSETCPSOCKET);
    UART_Wait;
}
void GetImage(void) {
    //connect to server
    StartConnection();
    //select message length.  In this case it will be 1
    TCPSendstart_UART();
    //transfer UART data to SPI
    xSemaphoreTake(Epaper_INIT_finished,pdMS_TO_TICKS(30000));
    StartUARTtoSPITransfer();
    UART_Wait;
    xSemaphoreGive(ESP_Image_Received);
    //close socket
    TCP_Close_Socket();
}