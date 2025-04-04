#ifndef TC1_Methods_h
#define TC1_Methods_h
void Init_TC1(const volatile unsigned int prescaler,const volatile unsigned char runinstandby);
void Start_TC1(void);
void Stop_TC1(void);
void EnableinterruptOVF_TC1(void);
void DisableinterruptOVF_TC1(void);
#endif