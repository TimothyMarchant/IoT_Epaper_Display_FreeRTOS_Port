#include "Sleep.h"
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
void ConfigSleep(unsigned char mode){
    PM_REGS->PM_SLEEPCFG=mode;
    //wait for the register to be written to before continuing
    while (PM_REGS->PM_SLEEPCFG!=mode);
}
