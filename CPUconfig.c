#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
//meant for configuring CPU frequency.  4MHz may be too slow for FreeRTOS to be effective.
#define InternalCPUCTRL OSCCTRL_REGS->OSCCTRL_OSC16MCTRL
#define DefaultMask 0x82
void configCPUspeed(unsigned char config){
    InternalCPUCTRL=DefaultMask|config;
}
