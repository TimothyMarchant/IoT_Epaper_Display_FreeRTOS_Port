#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#ifndef UART_Methods_h
#define UART_Methods_h
void InitUART(void);
void EnableUART(void);
void UART_Begin(unsigned short TLength,unsigned short RLength);
void UART_Enqueue_Transmit(unsigned char data);
void UART_sendstring(const char*string);
void UART_Write(unsigned char data);
volatile unsigned char UART_Read(void);
void DisableUART(void);
void Enableinterrupt(void);
void Disableinterrupt(void);
void UART_Wait_For_End_Of_Transmission(void);
void FlushReceiveQueue(void);
#endif
#define UART_Wait UART_Wait_For_End_Of_Transmission()