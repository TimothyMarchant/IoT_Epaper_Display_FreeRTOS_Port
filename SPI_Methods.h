#ifndef SPI_Methods_h
#define SPI_Methods_h
void InitSPI(const unsigned char baudrate);
void EnableSPI(void);
void DisableSPI(void);
void SPI_Begin(const unsigned int CS,const volatile unsigned short length);
void SPI_Enqueue(unsigned char data);
void SPI_Write(unsigned char data);
void SPI_Wait_For_Last_Byte(void);
void SPI_Start_Unknown_Packet(const volatile unsigned int pin);
void SPI_End(volatile unsigned int pin);
void SPI_Write_Blocking(unsigned char data);
#endif