#ifndef UART_Methods_h
#define UART_Methods_h
void InitUART(void);
unsigned char isBusy(void);
void StartUART(void);
void EndUART(void);
void Enableinterrupt(void);
void Disableinterrupt(void);
void BeginTransmission(unsigned short Tlength, const unsigned char* Tpacket, unsigned short Rlength,unsigned char* Rpacket,unsigned char isScreenTransfer);
void Resetvaliddata(void);
#endif