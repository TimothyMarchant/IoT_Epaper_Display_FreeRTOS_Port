#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CircularQueue.h"
#include "definitions.h"
#include "SPI_Methods.h"
#include "GPIO.h"
#include "Timer0_Methods.h"
#include "UART_Methods.h"
#include "Delay.h"
#include "Sleep.h"
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
#define ATTestResponse "\r\nOK\r\n"
#define ATCloseResponse "\r\nCLOSED\r\n\r\nOK\r\n"
//Expected response array.
unsigned volatile char ATResponse[30]={};
#define ATResponseMaxLength 30

void clearResponse(void){
    for (unsigned char i=0;i<ATResponseMaxLength;i++){
        ATResponse[i]=0;
    }
}
//ensure the device is working by using the AT command also should not send anything if not given "\r\nOK\r\n"
unsigned char ATbusy(void){
    clearResponse();
    BeginTransmission(strlen(ATString),ATString,6,ATResponse,0);
    while (isBusy());
    for (char i=0;i<6;i++){
        if (ATResponse[i]==ATTestResponse[i]){
            
        }
        else {
            return 1;
        }
    }
    
    return 0;
}
void disable_echo(void){
    //disable echo
    BeginTransmission(strlen(ATE0),ATE0,6,ATResponse,0);
    while(isBusy());
}
//test ESP and Epaper screen together
void TestSend(void){
    //echo is annoying
    disable_echo();
    while (ATbusy());
    //connect to server
    BeginTransmission(strlen(TCPSTART),TCPSTART,15,ATResponse,0);
    while(isBusy());
    //select message length.  In this case it will be 1
    BeginTransmission(strlen(TCPSENDSTART),TCPSENDSTART,6,ATResponse,0);
    while(isBusy());
    //use callback function actual values are not important
    BeginTransmission(1,dummy,1,ATResponse,1);
    while(isBusy());
    //close socket
    BeginTransmission(strlen(CLOSETCPSOCKET),CLOSETCPSOCKET,14,ATResponse,0);
    while(isBusy());
}
/*
//listen for a packet
void Listen(unsigned short length){
    
}
 * */