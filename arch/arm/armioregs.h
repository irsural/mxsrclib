#ifndef armioregsH
#define armioregsH

#if (((__TID__ >> 8) & 0x7F) != 0x4F)
#error This file should only be compiled by ARM IAR compiler and assembler
#endif

#include <armiomacros.h>

#ifdef __LM3Sx9xx__
  #include <armregs_lm3s.h>
#endif  //  __LM3Sx9xx__

#endif    /* __IOLM3SXXXX_H */
