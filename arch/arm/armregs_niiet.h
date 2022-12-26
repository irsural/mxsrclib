#ifndef ARMREGS_NIIET
#define ARMREGS_NIIET

#include <irsdefs.h>

#include <armregs_stm32f7xx.h>

#include <irsfinal.h>

#define GPIO_DATA_S                        0x00
#define GPIO_DATAOUT_S                     0x04
#define GPIO_DATAOUTSET_S                  0x08
#define GPIO_DATAOUTCLR_S                  0x0C
#define GPIO_DATAOUTTGL_S                  0x10
#define GPIO_DENSET_S                      0x14
#define GPIO_DENCLR_S                      0x18
#define GPIO_INMODE_S                      0x1C
#define GPIO_PULLMODE_S                    0x20
#define GPIO_OUTMODE_S                     0x24
#define GPIO_DRIVEMODE_S                   0x28
#define GPIO_OUTENSET_S                    0x2C
#define GPIO_OUTENCLR_S                    0x30
#define GPIO_ALTFUNCSET_S                  0x34
#define GPIO_ALTFUNCCLR_S                  0x38
#define GPIO_SYNCSET_S                     0x44
#define GPIO_SYNCCLR_S                     0x48
#define GPIO_QUALSET_S                     0x4C
#define GPIO_QUALCLR_S                     0x50
#define GPIO_QUALMODESET_S                 0x54
#define GPIO_QUALMODECLR_S                 0x58
#define GPIO_QUALSAMPLE_S                  0x5C
#define GPIO_INTENSET_S                    0x60
#define GPIO_INTENCLR_S                    0x64
#define GPIO_INTTYPESET_S                  0x68
#define GPIO_INTTYPECLR_S                  0x6C
#define GPIO_INTPOLSET_S                   0x70
#define GPIO_INTPOLCLR_S                   0x74
#define GPIO_INTEDGESET_S                  0x78
#define GPIO_INTEDGECLR_S                  0x7C
#define GPIO_INTSTATUS_S                   0x80
#define GPIO_DMAREQSET_S                   0x84
#define GPIO_DMAREQCLR_S                   0x88
#define GPIO_ADCSOCSET_S                   0x8C
#define GPIO_ADCSOCCLR_S                   0x90
#define GPIO_RXEVSET_S                     0x94
#define GPIO_RXEVCLR_S                     0x98
#define GPIO_LOCKKEY_S                     0x9C
#define GPIO_LOCKSTAT_S                    0x9C
#define GPIO_LOCKSET_S                     0xA0
#define GPIO_LOCKCLR_S                     0xA4         

#define GPIO_DRIVEMODE_HIGHSPEED_HIGHLOAD  0x00
#define GPIO_DRIVEMODE_LOWSPEED_HIGHLOAD   0x01
#define GPIO_DRIVEMODE_HIGHSPEED_LOWLOAD   0x02
#define GPIO_DRIVEMODE_LOWSPEED_LOWLOAD    0x03

#define GPIO_DENCLR_CLEAR                  0x01
#define GPIO_DENSET_SET                    0x01

#define GPIO_INMODE_SCHMITT_BUFFER         0x00                
#define GPIO_INMODE_CMOS_BUFFER            0x01                
#define GPIO_INMODE_DISABLE                0x03    

#define GPIO_PULLMODE_FLOATING             0x00
#define GPIO_PULLMODE_PULLUP               0x01
#define GPIO_PULLMODE_PULLDOWN             0x02

#define GPIO_OUTENCLR_CLEAR                0x01
#define GPIO_OUTENSET_SET                  0x01

#define GPIO_OUTMODE_PUSHPULL              0x00
#define GPIO_OUTMODE_OPEN_DRAIN            0x01
#define GPIO_OUTMODE_OPEN_SOURCE           0x02

#endif // armregs_stm32f2xxH
