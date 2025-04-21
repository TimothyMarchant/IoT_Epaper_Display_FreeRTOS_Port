#ifndef SPI_Methods_h
#define SPI_Methods_h
void InitSPI(const unsigned char baudrate);
void EnableSPI(void);
void DisableSPI(void);
void SPI_Begin(const unsigned int CS,const unsigned short length);
void SPI_Enqueue(unsigned char data);
void SPI_Write(unsigned char data);
void SPI_Wait_For_Last_Byte(void);
void SPI_End(unsigned int pin);
//BLOCKING methods
void StartSPI_BLOCKING(unsigned int CS);
void SPI_Write_BLOCKING(unsigned char data);
void EndSPI_BLOCKING(unsigned int pin);
#endif