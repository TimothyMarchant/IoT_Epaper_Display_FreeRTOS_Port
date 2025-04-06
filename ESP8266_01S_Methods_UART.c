#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "SPI_Methods.h"
#include "GPIO.h"
#include "UART_Methods.h"
#include "Sleep.h"
extern QueueHandle_t UART_Receive_Queue;
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
//ensure the device is working by using the AT command also should not send anything if not given "\r\nOK\r\n"
unsigned char ATbusy(void){
    return 1;
}
void disable_echo(void){
    //disable echo
    UART_Begin(strlen(ATE0),6,UART_Receive_Queue);
    UART_sendstring(ATE0);
    UART_Wait;
}
//test ESP and Epaper screen together
void TestSend(void){
    //echo is annoying
    disable_echo();
    UART_Wait;
    //connect to server
    UART_Begin(strlen(TCPSTART),15,UART_Receive_Queue);
    UART_sendstring(TCPSTART);
    //BeginTransmission(strlen(TCPSTART),TCPSTART,15,ATResponse,0);
    UART_Wait;
    //select message length.  In this case it will be 1
    UART_Begin(strlen(TCPSENDSTART),6,UART_Receive_Queue);
    UART_sendstring(TCPSENDSTART);
    //BeginTransmission(strlen(TCPSENDSTART),TCPSENDSTART,6,ATResponse,0);
    UART_Wait;
    //use callback function actual values are not important
    UART_Begin(1,5000,UART_Receive_Queue);
    UART_Enqueue_Transmit('a');
    //BeginTransmission(1,dummy,1,ATResponse,1);
    UART_Wait;
    //close socket
    UART_Begin(strlen(CLOSETCPSOCKET),14,UART_Receive_Queue);
    //BeginTransmission(strlen(CLOSETCPSOCKET),CLOSETCPSOCKET,14,ATResponse,0);
    UART_Wait;
}
/*
//listen for a packet
void Listen(unsigned short length){
    
}
 * */