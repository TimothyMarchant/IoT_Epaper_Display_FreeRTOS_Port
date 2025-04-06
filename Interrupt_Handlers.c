#include "Definitions.h"
#include "EICFlagbits.h"
#define SPITXCClear SERCOM0_REGS->SPIM.SERCOM_INTFLAG=0x02
#define UARTTXCClear SERCOM1_REGS->USART_INT.SERCOM_INTFLAG=0x02
#define UARTRXCClear SERCOM1_REGS->USART_INT.SERCOM_INTFLAG=0x04
void EIC0_Callback(void);
void EIC1_Callback(void);
void SPI_Callback(void);
void UART_CallbackTX(void);
void UART_CallbackRX(void);
void __attribute__((interrupt)) EIC_EXTINT_0_Handler(void){
    EIC0_Callback();
    EICClearFlag(EIC0);
}
void __attribute__((interrupt)) EIC_EXTINT_1_Handler(void){
    EIC1_Callback();
    EICClearFlag(EIC1);
}
void __attribute__((interrupt)) EIC_EXTINT_2_Handler(void){
    EICClearFlag(EIC2);
}
void __attribute__((interrupt)) EIC_EXTINT_3_Handler(void){
    EICClearFlag(EIC3);
}
void __attribute__((interrupt)) SERCOM0_1_Handler(void){
    SPI_Callback();
    SPITXCClear;
}
//TX
void __attribute__((interrupt)) SERCOM1_1_Handler(void){
    UART_CallbackTX();
    UARTTXCClear;
}
//RX
void __attribute__((interrupt)) SERCOM1_2_Handler(void){
    UART_CallbackRX();
    UARTRXCClear;
}