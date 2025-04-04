#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "EICFlagbits.h"
#include "EIC_Configbits.h"
#include "definitions.h"
#include "GPIO.h"
#define EIC EIC_REGS
#define EICCTRLAmask 0x00
#define EnableDebounce 0x0F
#define Prescalersettings 0x000100012

void Init_EIC(const unsigned char EICINTEN, const unsigned int configvalue) {
    GCLK_REGS->GCLK_PCHCTRL[3]=0x00000040;
    EIC->EIC_CTRLA = EICCTRLAmask;
    EIC->EIC_DEBOUNCEN = EnableDebounce;
    EIC->EIC_DPRESCALER = Prescalersettings;
    EIC->EIC_INTENSET = EICINTEN;
    NVIC_SetPriority(EIC_EXTINT_0_IRQn, 3);
    NVIC_SetPriority(EIC_EXTINT_0_IRQn, 3);
    NVIC_SetPriority(EIC_EXTINT_0_IRQn, 3);
    NVIC_SetPriority(EIC_EXTINT_0_IRQn, 3);
}

void Set_EIC0(const unsigned char Edge) {
    //EIC0 is on this pin pin 19 on the SSOP package
    EIC->EIC_CONFIG|=FilterEN0(Edge);
    configpin(PORT_PA19,Input);
    pinmuxconfig(19,GROUPA);
    NVIC_EnableIRQ(EIC_EXTINT_0_IRQn);
}

void Set_EIC1(const unsigned char Edge) {
    EIC->EIC_CONFIG|=FilterEN1(Edge);
    //EIC1 is on this pin, PA22 pin 20 on the SSOP package
    configpin(PORT_PA22,Input);
    pinmuxconfig(22,GROUPA);
    NVIC_EnableIRQ(EIC_EXTINT_1_IRQn);
}

void Set_EIC2(const unsigned char Edge) {
    EIC->EIC_CONFIG|=FilterEN2(Edge);
    configpin(PORT_PA23,Input);
    pinmuxconfig(23,GROUPA);
    NVIC_EnableIRQ(EIC_EXTINT_2_IRQn);
}

void Set_EIC3(const unsigned char Edge) {
    EIC->EIC_CONFIG|=FilterEN3(Edge);
    NVIC_EnableIRQ(EIC_EXTINT_3_IRQn);
}

void Clear_EIC0(void) {
    NVIC_DisableIRQ(EIC_EXTINT_0_IRQn);
}

void Clear_EIC1(void) {
    NVIC_DisableIRQ(EIC_EXTINT_1_IRQn);
}

void Clear_EIC2(void) {
    NVIC_DisableIRQ(EIC_EXTINT_2_IRQn);
}

void Clear_EIC3(void) {
    NVIC_DisableIRQ(EIC_EXTINT_3_IRQn);
}

void Enable_EIC(void) {
    EIC->EIC_CTRLA |= 0x02;
}

void Disable_EIC(void) {
    EIC->EIC_CTRLA &= ~(0x02);
}