#ifndef EICFlagbits_h
#define EICFlagbits_h
#define EIC0 0x01
#define EIC1 0x02
#define EIC2 0x04
#define EIC3 0x08
#define EIC4 0x10
#define EIC5 0x20
#define EIC6 0x40
#define EIC7 0x80
#endif
#ifndef EICFlagRegister
#define EICFlagRegister EIC_REGS->EIC_INTFLAG
#endif
#ifndef EICClearFlag
#define EICClearFlag(flag) EICFlagRegister=flag
#endif