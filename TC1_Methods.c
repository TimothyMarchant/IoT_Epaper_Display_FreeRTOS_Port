#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#define TC1 TC1_REGS
#define Enablebit 0x02
#define Standbybit 0x40
//register value needs to be 6
//we only will increment the timer
void Init_TC1(const volatile unsigned int prescaler,const volatile unsigned char runinstandby){
    //enable timer0
    GCLK_REGS->GCLK_PCHCTRL[14]|=0b01000000;
    TC1->COUNT8.TC_CTRLBCLR=0x01;
    TC1->COUNT8.TC_CTRLA|=0x004|(prescaler);
    if (runinstandby){
        TC1->COUNT8.TC_CTRLA|=Standbybit;
    }
    //setup interrupt priority
    NVIC_SetPriority(TC1_IRQn,3);
}
void Start_TC1(void){
    TC1->COUNT8.TC_COUNT=6;
    TC1->COUNT8.TC_CTRLA|=Enablebit;
}
void Stop_TC1(void){
    TC1->COUNT8.TC_CTRLA&=~(Enablebit);
}
void EnableinterruptOVF_TC1(void){
    TC1->COUNT8.TC_INTENSET=0x01;
    NVIC_EnableIRQ(TC1_IRQn);
}
void DisableinterruptOVF_TC1(void){
    TC1->COUNT8.TC_INTENCLR=0x01;
    NVIC_DisableIRQ(TC1_IRQn);
}