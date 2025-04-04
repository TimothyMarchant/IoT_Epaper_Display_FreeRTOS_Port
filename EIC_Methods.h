#include "EIC_Configbits.h"
#include "EICFlagbits.h"
#ifndef EIC_h
#define EIC_h
void Init_EIC(const unsigned char EICINTEN, const unsigned int configvalue);
void Set_EIC0(const unsigned char Edge);
void Set_EIC1(const unsigned char Edge);
void Set_EIC2(const unsigned char Edge);
void Set_EIC3(const unsigned char Edge);
void Clear_EIC0(void);
void Clear_EIC1(void);
void Clear_EIC2(void);
void Clear_EIC3(void);
void Enable_EIC(void);
void Disable_EIC(void);
#endif