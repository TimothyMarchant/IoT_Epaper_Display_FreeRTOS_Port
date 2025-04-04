#ifndef SPI_Methods_h
#define SPI_Methods_h
void InitSPI(const unsigned char baudrate);
void EnableSPI(void);
void DisableSPI(void);
void SPI_Start(const volatile unsigned char pin,const volatile unsigned short length,unsigned char* givenPacket);
void SPI_Start_Unknown_Packet(const volatile unsigned int pin);
void SPI_Start_Repeated(const volatile unsigned char pin, const volatile unsigned short length, unsigned char data);
volatile void SPI_End(volatile unsigned int pin);
void SPI_Write_Blocking(unsigned char data);
void ChangetoLSB(void);
void ChangetoMSB(void);
#endif