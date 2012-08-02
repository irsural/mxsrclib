/**************************************************
 *
 * Common macro declarations used for peripheral I/O
 * declarations for ARM IAR C/C++ Compiler and Assembler.
 *
 * Copyright 1999-2005 IAR Systems. All rights reserved.
 *
 * $Revision: 21623 $
 *
 **************************************************/

#ifndef armiomacrosH
#define armiomacrosH

/***********************************************
 *      C specific macros
 ***********************************************/

#ifdef __IAR_SYSTEMS_ICC__

#ifndef _SYSTEM_BUILD
  #pragma system_include
#endif

#pragma language=save
#pragma language=extended


//  ������ �� ������
#define HWREG(x) (*((volatile irs_u32*)(x)))

/* SFR sizes */
#define __REG8 unsigned char
#define __REG16 unsigned short
#define __REG32 unsigned long


/***********************************************
 * I/O reg attributes
 ***********************************************/
#define __READ_WRITE
#ifdef __cplusplus
#define __READ          /* Not supported */
#else
#define __READ          const
#endif
#define __WRITE         /* Not supported */

/***********************************************
 * I/O reg bits (default names)
 ***********************************************/
typedef struct
{
  unsigned char no0:1;
  unsigned char no1:1;
  unsigned char no2:1;
  unsigned char no3:1;
  unsigned char no4:1;
  unsigned char no5:1;
  unsigned char no6:1;
  unsigned char no7:1;
} __BITS8;

typedef struct
{
  unsigned short no0:1;
  unsigned short no1:1;
  unsigned short no2:1;
  unsigned short no3:1;
  unsigned short no4:1;
  unsigned short no5:1;
  unsigned short no6:1;
  unsigned short no7:1;
  unsigned short no8:1;
  unsigned short no9:1;
  unsigned short no10:1;
  unsigned short no11:1;
  unsigned short no12:1;
  unsigned short no13:1;
  unsigned short no14:1;
  unsigned short no15:1;
} __BITS16;

typedef struct
{
  unsigned long no0:1;
  unsigned long no1:1;
  unsigned long no2:1;
  unsigned long no3:1;
  unsigned long no4:1;
  unsigned long no5:1;
  unsigned long no6:1;
  unsigned long no7:1;
  unsigned long no8:1;
  unsigned long no9:1;
  unsigned long no10:1;
  unsigned long no11:1;
  unsigned long no12:1;
  unsigned long no13:1;
  unsigned long no14:1;
  unsigned long no15:1;
  unsigned long no16:1;
  unsigned long no17:1;
  unsigned long no18:1;
  unsigned long no19:1;
  unsigned long no20:1;
  unsigned long no21:1;
  unsigned long no22:1;
  unsigned long no23:1;
  unsigned long no24:1;
  unsigned long no25:1;
  unsigned long no26:1;
  unsigned long no27:1;
  unsigned long no28:1;
  unsigned long no29:1;
  unsigned long no30:1;
  unsigned long no31:1;
} __BITS32;

/***********************************************
 * Define NAME as an I/O reg
 * Access of 8/16/32 bit reg:  NAME
 ***********************************************/
#define __IO_REG8(NAME, ADDRESS, ATTRIBUTE)              \
                   volatile __no_init ATTRIBUTE unsigned char NAME @ ADDRESS

#define __IO_REG16(NAME, ADDRESS, ATTRIBUTE)             \
                   volatile __no_init ATTRIBUTE unsigned short NAME @ ADDRESS

#define __IO_REG32(NAME, ADDRESS, ATTRIBUTE)             \
                   volatile __no_init ATTRIBUTE unsigned long NAME @ ADDRESS

/***********************************************
 * Define NAME as an I/O reg
 * Access of 8/16/32 bit reg:  NAME
 * Access of bit(s):           NAME_bit.noX  (X=1-31)
 ***********************************************/
#define __IO_REG8_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)\
                       volatile __no_init ATTRIBUTE union \
                        {                                 \
                          unsigned char NAME;             \
                          BIT_STRUCT NAME ## _bit;      \
                        } @ ADDRESS

#define __IO_REG16_BIT(NAME, ADDRESS, ATTRIBUTE,BIT_STRUCT)\
                        volatile __no_init ATTRIBUTE union \
                         {                                 \
                           unsigned short NAME;            \
                           BIT_STRUCT NAME ## _bit;      \
                         } @ ADDRESS

#define __IO_REG32_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)\
                        volatile __no_init ATTRIBUTE union \
                         {                                 \
                           unsigned long NAME;             \
                           BIT_STRUCT NAME ## _bit;      \
                         } @ ADDRESS

#pragma language=restore

//  ������ �� ������
#define HWREG(x) (*((volatile irs_u32*)(x)))

#endif /* __IAR_SYSTEMS_ICC__ */


/***********************************************
 *      Assembler specific macros
 ***********************************************/

#ifdef __IAR_SYSTEMS_ASM__

/***********************************************
 * I/O reg attributes (ignored)
 ***********************************************/
#define __READ_WRITE 0
#define __READ 0
#define __WRITE 0

/***********************************************
 * Define NAME as an I/O reg
 ***********************************************/
#define __IO_REG8(NAME, ADDRESS, ATTRIBUTE)      \
                  NAME DEFINE ADDRESS

#define __IO_REG16(NAME, ADDRESS, ATTRIBUTE)     \
                   NAME DEFINE ADDRESS

#define __IO_REG32(NAME, ADDRESS, ATTRIBUTE)     \
                   NAME DEFINE ADDRESS

/***********************************************
 * Define NAME as an I/O reg
 ***********************************************/
#define __IO_REG8_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)  \
                      NAME DEFINE ADDRESS

#define __IO_REG16_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT) \
                       NAME DEFINE ADDRESS

#define __IO_REG32_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT) \
                       NAME DEFINE ADDRESS
#endif /* __IAR_SYSTEMS_ASM__ */

#endif  //  armiomacrosH
