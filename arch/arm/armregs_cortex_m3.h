#ifndef armregs_cortex_m3H
#define armregs_cortex_m3H

#include <armiomacros.h>

#ifdef CORTEX_M3_CORE

/* Interrupt Controller Type Register */
typedef struct {
  __REG32  INTLINESNUM    : 5;
  __REG32                 :27;
} __nvic_bits;

/* SysTick Control and Status Register */
typedef struct {
  __REG32  ENABLE         : 1;
  __REG32  TICKINT        : 1;
  __REG32  CLKSOURCE      : 1;
  __REG32                 :13;
  __REG32  COUNTFLAG      : 1;
  __REG32                 :15;
} __systickcsr_bits;

/* SysTick Reload Value Register */
typedef struct {
  __REG32  RELOAD         :24;
  __REG32                 : 8;
} __systickrvr_bits;

/* SysTick Current Value Register */
typedef struct {
  __REG32  CURRENT        :24;
  __REG32                 : 8;
} __systickcvr_bits;

/* SysTick Calibration Value Register */
typedef struct {
  __REG32  TENMS          :24;
  __REG32                 : 6;
  __REG32  SKEW           : 1;
  __REG32  NOREF          : 1;
} __systickcalvr_bits;

/* Interrupt Priority Registers 0-3 */
typedef struct {
  __REG32  PRI_0          : 8;
  __REG32  PRI_1          : 8;
  __REG32  PRI_2          : 8;
  __REG32  PRI_3          : 8;
} __pri0_bits;

/* Interrupt Priority Registers 4-7 */
typedef struct {
  __REG32  PRI_4          : 8;
  __REG32  PRI_5          : 8;
  __REG32  PRI_6          : 8;
  __REG32  PRI_7          : 8;
} __pri1_bits;

/* Interrupt Priority Registers 8-11 */
typedef struct {
  __REG32  PRI_8          : 8;
  __REG32  PRI_9          : 8;
  __REG32  PRI_10         : 8;
  __REG32  PRI_11         : 8;
} __pri2_bits;

/* Interrupt Priority Registers 12-15 */
typedef struct {
  __REG32  PRI_12         : 8;
  __REG32  PRI_13         : 8;
  __REG32  PRI_14         : 8;
  __REG32  PRI_15         : 8;
} __pri3_bits;

/* Interrupt Priority Registers 16-19 */
typedef struct {
  __REG32  PRI_16         : 8;
  __REG32  PRI_17         : 8;
  __REG32  PRI_18         : 8;
  __REG32  PRI_19         : 8;
} __pri4_bits;

/* Interrupt Priority Registers 20-23 */
typedef struct {
  __REG32  PRI_20         : 8;
  __REG32  PRI_21         : 8;
  __REG32  PRI_22         : 8;
  __REG32  PRI_23         : 8;
} __pri5_bits;

/* Interrupt Priority Registers 24-27 */
typedef struct {
  __REG32  PRI_24         : 8;
  __REG32  PRI_25         : 8;
  __REG32  PRI_26         : 8;
  __REG32  PRI_27         : 8;
} __pri6_bits;

/* Interrupt Priority Registers 28-31 */
typedef struct {
  __REG32  PRI_28         : 8;
  __REG32  PRI_29         : 8;
  __REG32  PRI_30         : 8;
  __REG32  PRI_31         : 8;
} __pri7_bits;

/* Interrupt Priority Registers 32-35 */
typedef struct {
  __REG32  PRI_32         : 8;
  __REG32  PRI_33         : 8;
  __REG32  PRI_34         : 8;
  __REG32  PRI_35         : 8;
} __pri8_bits;

/* Interrupt Priority Registers 36-39 */
typedef struct {
  __REG32  PRI_36         : 8;
  __REG32  PRI_37         : 8;
  __REG32  PRI_38         : 8;
  __REG32  PRI_39         : 8;
} __pri9_bits;

/* Interrupt Priority Registers 40-43 */
typedef struct {
  __REG32  PRI_40         : 8;
  __REG32  PRI_41         : 8;
  __REG32  PRI_42         : 8;
  __REG32  PRI_43         : 8;
} __pri10_bits;

/* Interrupt Priority Registers 44-47 */
typedef struct {
  __REG32  PRI_44         : 8;
  __REG32  PRI_45         : 8;
  __REG32  PRI_46         : 8;
  __REG32  PRI_47         : 8;
} __pri11_bits;

/* Interrupt Priority Registers 48-51 */
typedef struct {
  __REG32  PRI_48         : 8;
  __REG32  PRI_49         : 8;
  __REG32  PRI_50         : 8;
  __REG32  PRI_51         : 8;
} __pri12_bits;

/* Interrupt Priority Registers 52-55 */
typedef struct {
  __REG32  PRI_52         : 8;
  __REG32  PRI_53         : 8;
  __REG32  PRI_54         : 8;
  __REG32  PRI_55         : 8;
} __pri13_bits;

/* Interrupt Priority Registers 56-59 */
typedef struct {
  __REG32  PRI_56         : 8;
  __REG32  PRI_57         : 8;
  __REG32  PRI_58         : 8;
  __REG32  PRI_59         : 8;
} __pri14_bits;

/* Interrupt Priority Registers 60-63 */
typedef struct {
  __REG32  PRI_60         : 8;
  __REG32  PRI_61         : 8;
  __REG32  PRI_62         : 8;
  __REG32  PRI_63         : 8;
} __pri15_bits;

/* CPU ID Base Register */
typedef struct {
  __REG32  REVISION       : 4;
  __REG32  PARTNO         :12;
  __REG32                 : 4;
  __REG32  VARIANT        : 4;
  __REG32  IMPLEMENTER    : 8;
} __cpuidbr_bits;

/* Interrupt Control State Register */
typedef struct {
  __REG32  VECTACTIVE     :10;
  __REG32                 : 1;
  __REG32  RETTOBASE      : 1;
  __REG32  VECTPENDING    :10;
  __REG32  ISRPENDING     : 1;
  __REG32  ISRPREEMPT     : 1;
  __REG32                 : 1;
  __REG32  PENDSTCLR      : 1;
  __REG32  PENDSTSET      : 1;
  __REG32  PENDSVCLR      : 1;
  __REG32  PENDSVSET      : 1;
  __REG32                 : 2;
  __REG32  NMIPENDSET     : 1;
} __icsr_bits;

/* Vector Table Offset Register */
typedef struct {
  __REG32                 : 7;
  __REG32  TBLOFF         :22;
  __REG32  TBLBASE        : 1;
  __REG32                 : 2;
} __vtor_bits;

/* Application Interrupt and Reset Control Register */
typedef struct {
  __REG32  VECTRESET      : 1;
  __REG32  VECTCLRACTIVE  : 1;
  __REG32  SYSRESETREQ    : 1;
  __REG32                 : 5;
  __REG32  PRIGROUP       : 3;
  __REG32                 : 4;
  __REG32  ENDIANESS      : 1;
  __REG32  VECTKEY        :16;
} __aircr_bits;

/* System Control Register */
typedef struct {
  __REG32                 : 1;
  __REG32  SLEEPONEXIT    : 1;
  __REG32  SLEEPDEEP      : 1;
  __REG32                 : 1;
  __REG32  SEVONPEND      : 1;
  __REG32                 :27;
} __scr_bits;

/* Configuration Control Register */
typedef struct {
  __REG32  NONEBASETHRDENA: 1;
  __REG32  USERSETMPEND   : 1;
  __REG32                 : 1;
  __REG32  UNALIGN_TRP    : 1;
  __REG32  DIV_0_TRP      : 1;
  __REG32                 : 3;
  __REG32  BFHFNMIGN      : 1;
  __REG32  STKALIGN       : 1;
  __REG32                 :22;
} __ccr_bits;

/* System Handler Control and State Register */
typedef struct {
  __REG32  MEMFAULTACT    : 1;
  __REG32  BUSFAULTACT    : 1;
  __REG32                 : 1;
  __REG32  USGFAULTACT    : 1;
  __REG32                 : 3;
  __REG32  SVCALLACT      : 1;
  __REG32  MONITORACT     : 1;
  __REG32                 : 1;
  __REG32  PENDSVACT      : 1;
  __REG32  SYSTICKACT     : 1;
/*#ifdef __LM3SxBxx_
  __REG32  USGFAULTPENDED : 1;
#else // __LM3SxBxx_
  __REG32                 : 1;
#endif // __LM3SxBxx_*/
  __REG32  USGFAULTPENDED : 1;
  __REG32  MEMFAULTPENDED : 1;
  __REG32  BUSFAULTPENDED : 1;
  __REG32  SVCALLPENDED   : 1;
  __REG32  MEMFAULTENA    : 1;
  __REG32  BUSFAULTENA    : 1;
  __REG32  USGFAULTENA    : 1;
  __REG32                 :13;
} __shcsr_bits;

/* Configurable Fault Status Registers */
typedef struct {
  __REG32  IACCVIOL       : 1;
  __REG32  DACCVIOL       : 1;
  __REG32                 : 1;
  __REG32  MUNSTKERR      : 1;
  __REG32  MSTKERR        : 1;
  __REG32                 : 2;
  __REG32  MMARVALID      : 1;
  __REG32  IBUSERR        : 1;
  __REG32  PRECISERR      : 1;
  __REG32  IMPRECISERR    : 1;
  __REG32  UNSTKERR       : 1;
  __REG32  STKERR         : 1;
  __REG32                 : 2;
  __REG32  BFARVALID      : 1;
  __REG32  UNDEFINSTR     : 1;
  __REG32  INVSTATE       : 1;
  __REG32  INVPC          : 1;
  __REG32  NOCP           : 1;
  __REG32                 : 4;
  __REG32  UNALIGNED      : 1;
  __REG32  DIVBYZERO      : 1;
  __REG32                 : 6;
} __cfsr_bits;

/* Hard Fault Status Register */
typedef struct {
  __REG32                 : 1;
  __REG32  VECTTBL        : 1;
  __REG32                 :28;
  __REG32  FORCED         : 1;
  __REG32  DEBUGEVT       : 1;
} __hfsr_bits;

/* Debug Fault Status Register */
typedef struct {
  __REG32  HALTED         : 1;
  __REG32  BKPT           : 1;
  __REG32  DWTTRAP        : 1;
  __REG32  VCATCH         : 1;
  __REG32  EXTERNAL       : 1;
  __REG32                 :27;
} __dfsr_bits;

/* Software Trigger Interrupt Register */
typedef struct {
  __REG32  INTID          : 9;
  __REG32                 :23;
} __stir_bits;

__IO_REG32_BIT(NVIC,              0xE000E004,__READ       ,__nvic_bits);
__IO_REG32_BIT(SYSTICKCSR,        0xE000E010,__READ_WRITE ,__systickcsr_bits);
__IO_REG32_BIT(SYSTICKRVR,        0xE000E014,__READ_WRITE ,__systickrvr_bits);
__IO_REG32_BIT(SYSTICKCVR,        0xE000E018,__READ_WRITE ,__systickcvr_bits);
__IO_REG32_BIT(SYSTICKCALVR,      0xE000E01C,__READ       ,__systickcalvr_bits);
__IO_REG32_BIT(SETENA0,           0xE000E100,__READ_WRITE ,__setena0_bits);
__IO_REG32_BIT(SETENA1,           0xE000E104,__READ_WRITE ,__setena1_bits);
__IO_REG32_BIT(CLRENA0,           0xE000E180,__READ_WRITE ,__clrena0_bits);
__IO_REG32_BIT(CLRENA1,           0xE000E184,__READ_WRITE ,__clrena1_bits);
__IO_REG32_BIT(SETPEND0,          0xE000E200,__READ_WRITE ,__setpend0_bits);
__IO_REG32_BIT(SETPEND1,          0xE000E204,__READ_WRITE ,__setpend1_bits);
__IO_REG32_BIT(CLRPEND0,          0xE000E280,__READ_WRITE ,__clrpend0_bits);
__IO_REG32_BIT(CLRPEND1,          0xE000E284,__READ_WRITE ,__clrpend1_bits);
__IO_REG32_BIT(ACTIVE0,           0xE000E300,__READ       ,__active0_bits);
__IO_REG32_BIT(ACTIVE1,           0xE000E304,__READ       ,__active1_bits);
__IO_REG32_BIT(IP0,               0xE000E400,__READ_WRITE ,__pri0_bits);
__IO_REG32_BIT(IP1,               0xE000E404,__READ_WRITE ,__pri1_bits);
__IO_REG32_BIT(IP2,               0xE000E408,__READ_WRITE ,__pri2_bits);
__IO_REG32_BIT(IP3,               0xE000E40C,__READ_WRITE ,__pri3_bits);
__IO_REG32_BIT(IP4,               0xE000E410,__READ_WRITE ,__pri4_bits);
__IO_REG32_BIT(IP5,               0xE000E414,__READ_WRITE ,__pri5_bits);
__IO_REG32_BIT(IP6,               0xE000E418,__READ_WRITE ,__pri6_bits);
__IO_REG32_BIT(IP7,               0xE000E41C,__READ_WRITE ,__pri7_bits);
__IO_REG32_BIT(IP8,               0xE000E420,__READ_WRITE ,__pri8_bits);
__IO_REG32_BIT(IP9,               0xE000E424,__READ_WRITE ,__pri9_bits);
__IO_REG32_BIT(IP10,              0xE000E428,__READ_WRITE ,__pri10_bits);
__IO_REG32_BIT(IP11,              0xE000E42C,__READ_WRITE ,__pri11_bits);
__IO_REG32_BIT(IP12,              0xE000E430,__READ_WRITE ,__pri12_bits);
__IO_REG32_BIT(IP13,              0xE000E434,__READ_WRITE ,__pri13_bits);
__IO_REG32_BIT(IP14,              0xE000E438,__READ_WRITE ,__pri14_bits);
__IO_REG32_BIT(IP15,              0xE000E43C,__READ_WRITE ,__pri15_bits);
__IO_REG32_BIT(CPUIDBR,           0xE000ED00,__READ       ,__cpuidbr_bits);
__IO_REG32_BIT(ICSR,              0xE000ED04,__READ_WRITE ,__icsr_bits);
__IO_REG32_BIT(VTOR,              0xE000ED08,__READ_WRITE ,__vtor_bits);
__IO_REG32_BIT(AITCR,             0xE000ED0C,__READ_WRITE ,__aircr_bits);
__IO_REG32_BIT(SCR,               0xE000ED10,__READ_WRITE ,__scr_bits);
__IO_REG32_BIT(CCR,               0xE000ED14,__READ_WRITE ,__ccr_bits);
__IO_REG32_BIT(SHPR0,             0xE000ED18,__READ_WRITE ,__pri1_bits);
__IO_REG32_BIT(SHPR1,             0xE000ED1C,__READ_WRITE ,__pri2_bits);
__IO_REG32_BIT(SHPR2,             0xE000ED20,__READ_WRITE ,__pri3_bits);
__IO_REG32_BIT(SHCSR,             0xE000ED24,__READ_WRITE ,__shcsr_bits);
__IO_REG32_BIT(CFSR,              0xE000ED28,__READ_WRITE ,__cfsr_bits);
__IO_REG32_BIT(HFSR,              0xE000ED2C,__READ_WRITE ,__hfsr_bits);
__IO_REG32_BIT(DFSR,              0xE000ED30,__READ_WRITE ,__dfsr_bits);
__IO_REG32(    MMFAR,             0xE000ED34,__READ_WRITE);
__IO_REG32(    BFAR,              0xE000ED38,__READ_WRITE);
__IO_REG32(    AFSR,              0xE000ED3C,__READ_WRITE);
__IO_REG32_BIT(STIR,              0xE000EF00,__WRITE      ,__stir_bits);

#endif  //  CORTEX_M3_CORE

#endif  //  armregs_cortex_m3H
