#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#ifndef UART_Methods_h
#define UART_Methods_h
void InitUART(void);
unsigned char isBusy(void);
void EnableUART(void);
void UART_Begin(unsigned short TLength,unsigned short RLength,QueueHandle_t receiverqueue);
void UART_Enqueue_Transmit(unsigned char data);
void UART_sendstring(const char*string);
void DisableUART(void);
void Enableinterrupt(void);
void Disableinterrupt(void);
void UART_Wait_For_End_Of_Transmission(void);
void BeginTransmission(unsigned short Tlength, const unsigned char* Tpacket, unsigned short Rlength,unsigned char* Rpacket,unsigned char isScreenTransfer);
#endif
#define UART_Wait UART_Wait_For_End_Of_Transmission()