#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "EICFlagbits.h"
#include "EIC_Configbits.h"
#include "definitions.h"
#include "GPIO.h"
#ifndef NVM_Important_Values
#define NVM_Important_Values
#ifndef NVM_KEY
#define NVM_KEY 0xA5 //this value must be written in a few registers in order to do writes.
#endif
#ifndef ManualWrite_REG
#define ManualWrite_REG(bit0) NVMCTRL_REGS->NVMCTRL_CTRLC=bit0
#endif
#ifndef NVM_Interrupt_Masks
#define NVM_Interrupt_Masks
#define NVM_DONE 0x01 //DONE bit
#define NVM_ERROR 0x08 //NVME bit
#endif
#ifndef NVM_Status
#define NVM_Status NVMCTRL_REGS->STATUS
#define Readybit 0x0004
#define NVM_Status_READY NVM_Status&Readybit
#endif
#ifndef DataMemoryAddress //ADDR register
#define DataMemoryAddress //This is where the memory addresses that will be written to.
#define DATA_FLASH_START_ADDRESS 0x00400000 //this is the starting address for Data flash.
#define DATA_FLASH 0x00400000 //This corresponds to non application flash.
#define NVM_ROWS 0x00800000
#define STARTADDRESS 0x0000
#define Add_REG NVMCTRL_REGS->NVMCTRL_ADDR
#define DATA_FLASH_BIT 0x00400000
#define FLASH_BIT 0x00000000
#endif
#ifndef NVM_Commands_CTRLA
#define NVM_Commands_CTRLA
#define ER 0x0002 //Erase row
#define WP 0x0004 //Write Page
#define SPRM 0x0042 //Sets the power reduction mode
#define CPRM 0x0043 //Clears the power reduction mode
#define INVALL 0x0046 //Invalidates all cache lines.
#define SDAL0 0x004B //Set DAL=0
#define SDAL1 0x004C //Set DAL=1; do not use for SAML10 only for SAML11.
#define WRITECMD NVMCTRL_REGS->NVMCTRL_CTRLA=(WP|(NVM_KEY<<8))
#define ERASECMD NVMCTRL_REGS->NVMCTRL_CTRLA=(ER|(NVM_KEY<<8))
#endif
#ifndef NVM_PARAM //this register is read only.  The values that we write in the NVM are 32-bit values
#define Pagesize 64 //this is verrified in the debugger looking at PARAM register. 64 bytes
#define NumofPages 512 //number of pages in DATA FLASH
#define NVM_PARAM NVMCTRL_REGS->NVMCTRL_PARAM
#define Num_Of_Pages_In_DATA_FLASH ((NVM_PARAM&0xFFF00000)>>20)
#define NVM_Pagesize_READ ((NVM_PARAM&0x00070000)>>16)
#endif
#endif
unsigned int* Dataflashpointer=DATA_FLASH_START_ADDRESS;
unsigned int* Pagebuffer;
//may need to be changed some.   On reset Manualwrite is default on.
void Init_NVM(void){
    ManualWrite_REG(1);
}
//write page buffer to Data flash memory area.
void WritePageTo_DATAFLASH(unsigned char* Data,unsigned short pagenum){
    Add_REG=DATA_FLASH_BIT;
    Pagebuffer=(unsigned int*)Data;
    Dataflashpointer=(unsigned int *) (DATA_FLASH_START_ADDRESS+(64*pagenum));
    for (unsigned char i=0;i<(Pagesize/sizeof(unsigned int));i++){
        *Dataflashpointer++=Pagebuffer[i];
    }
    Add_REG|=(64*pagenum)+DATA_FLASH_START_ADDRESS;
    WRITECMD;
}
void ReadPageFrom_DATAFLASH(unsigned char* Readbuffer,unsigned short pagenum){
    unsigned char*Readpointer=(unsigned char *) (DATA_FLASH_START_ADDRESS+(64*pagenum));
    for (unsigned char i;i<Pagesize;i++){
        Readbuffer[i]=*(Readpointer+i);
    }
}
void ReadStringFrom_DATAFLASH(unsigned char* Readbuffer,unsigned char stringlength,unsigned int offset){
    unsigned char*Readpointer=(unsigned char *) (DATA_FLASH_START_ADDRESS+(offset));
    for (unsigned char i;i<stringlength;i++){
        Readbuffer[i]=*(Readpointer+i);
    }
}
//read single int value from flash.  Offset needs to be a multiple of 4.
void ReadIntFrom_DATAFLASH(unsigned int* data,unsigned int offset){
    *data=*((unsigned int*) (DATA_FLASH_START_ADDRESS+(offset)));
}
//erase page.
void ErasePageFrom_DATAFLASH(unsigned short pagenum){
    Add_REG=DATA_FLASH_BIT;
    Add_REG|=(64*pagenum)+DATA_FLASH_START_ADDRESS;
    ERASECMD;
}
//blocking loop.  Once HIGH can leave loop.  Needs to be called by application between writes.
void BLOCKING_IsNVM_Busy(void){
    while(!(NVM_Status_READY));
}