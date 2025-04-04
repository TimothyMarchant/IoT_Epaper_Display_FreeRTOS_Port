#ifndef Sleep_h
#define Sleep_h
#define IDLE 0x02
#define STANDBY 0x04
#define SHUTOFF 0x06
//excute the WFI instruction to enter into sleep mode.  This define is here for context
#define SLEEP __WFI()
void ConfigSleep(unsigned char mode);
#endif