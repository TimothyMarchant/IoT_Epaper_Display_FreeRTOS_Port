#ifndef EIC_Configbits_h
#define EIC_Configbits_h
#define FilterEN 0x08
#define NoDetection 0x00
#define RISE 0x01
#define FALL 0x02
#define BOTH 0x03
#define HIGHLEVEL 0x04
#define LOWLEVEL 0x05
#endif
#ifndef EIC_FilterEN_h
#define EIC_FilterEN_h
#define FilterEN0(edge) (FilterEN|edge)
#define FilterEN1(edge) (FilterEN|edge)<<4
#define FilterEN2(edge) (FilterEN|edge)<<8
#define FilterEN3(edge) (FilterEN|edge)<<12
#define FilterEN4(edge) (FilterEN|edge)<<16
#define FilterEN5(edge) (FilterEN|edge)<<20
#define FilterEN6(edge) (FilterEN|edge)<<24
#define FilterEN7(edge) (FilterEN|edge)<<28
#endif
