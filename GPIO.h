#ifndef GPIO_Methods_h
#define GPIO_Methods_h
void Init_IO(void);
void configpin(const volatile unsigned int pin,const unsigned char type);
void pinwrite(const volatile unsigned int pin,const unsigned char level);
unsigned char pinread(const volatile unsigned int pin,const unsigned char pinnum);
void pinmuxconfig(const unsigned char pinnum,const unsigned char group);
void pinmuxconfigdisable(const unsigned char pinnum);
#endif
#ifndef SpecialValues
#define SpecialValues
#define Input 1
#define Output 0
#define HIGH 1
#define LOW 0
#define GROUPA 0x00
#define GROUPB 0x01
#define GROUPC 0x02
#define GROUPD 0x03
#define GROUPE 0x04
#define GROUPG 0x06
#define GROUPH 0x07
#define GROUPI 0x08
#endif