#ifndef armregs_stm32H
#define armregs_stm32H

#include <armiomacros.h>

//  Определение семейства МК

#if defined(__STM32F100RBT__)
  #define STM32F100_FAMILY
#else
  #error Тип контроллера не определён
#endif  //  MCU_FAMILY

//  Определение базовых адресов периферии

#ifdef STM32F100_FAMILY
  #define TIM2_BASE   0x40000000
  #define TIM3_BASE   0x40000400
  #define TIM4_BASE   0x40000800
  #define TIM6_BASE   0x40001000
  #define TIM7_BASE   0x40001400
  #define RTC_BASE    0x40002800
  #define WWDG_BASE   0x40002C00
  #define IWDG_BASE   0x40003000
  #define SPI2_BASE   0x40003800
  #define USART2_BASE 0x40004400
  #define USART3_BASE 0x40004800
  #define I2C1_BASE   0x40005400
  #define I2C2_BASE   0x40005C00
  #define BKP_BASE    0x40006C00
  #define PWR_BASE    0x40007000
  #define DAC_BASE    0x40007400
  #define CEC_BASE    0x40007800
  #define AFIO_BASE   0x40010000
  #define EXTI_BASE   0x40010400
  #define PORTA_BASE  0x40010800
  #define PORTB_BASE  0x40010C00
  #define PORTC_BASE  0x40011000
  #define PORTD_BASE  0x40011400
  #define PORTE_BASE  0x40011800
  #define ADC1_BASE   0x40012400
  #define TIM1_BASE   0x40012C00
  #define SPI1_BASE   0x40013000
  #define USART1_BASE 0x40013800
  #define TIM15_BASE  0x40014000
  #define TIM16_BASE  0x40014400
  #define TIM17_BASE  0x40014800
  #define DMA_BASE    0x40020000
  #define RCC_BASE    0x40021000
  #define FLI_BASE    0x40022000  //  Flash Interface
  #define CRC_BASE    0x40023000
#endif  //  Base Addresses

//  Регистры периферии

//  RCC

#ifdef STM32F100_FAMILY

#define CR        0x000
#define CFGR      0x004
#define CIR       0x008
#define APB2RSTR  0x00C
#define APB1RSTR  0x010
#define AHBENR    0x014
#define APB2ENR   0x018
#define APB1ENR   0x01C
#define BDCR      0x020
#define CSR       0x024
#define CFGR2     0x02C

typedef struct {
  __REG32  AFIOEN         : 1;
  __REG32                 : 1;
  __REG32  IOPAEN         : 1;
  __REG32  IOPBEN         : 1;
  __REG32  IOPCEN         : 1;
  __REG32  IOPDEN         : 1;
  __REG32  IOPEEN         : 1;
  __REG32                 : 1;
  __REG32                 : 1;
  __REG32  ADC1EN         : 1;
  __REG32                 : 1;
  __REG32  TIM1EN         : 1;
  __REG32  SPI1EN         : 1;
  __REG32                 : 1;
  __REG32  USART1EN       : 1;
  __REG32                 : 1;
  __REG32  TIM15EN        : 1;
  __REG32  TIM17EN        : 1;
  __REG32  TIM16EN        : 1;
  __REG32                 : 13;
} __apb2enr_bits;

typedef struct {
  __REG32  HSION          : 1;
  __REG32  HSIRDY         : 1;
  __REG32                 : 1;
  __REG32  HSITRIM        : 5;
  __REG32  HSICAL         : 8;
  __REG32  HSEON          : 1;
  __REG32  HSERDY         : 1;
  __REG32  HSEBYP         : 1;
  __REG32  CSSON          : 1;
  __REG32                 : 4;
  __REG32  PLLON          : 1;
  __REG32  PLLRDY         : 1;
  __REG32                 : 6;
} __cr_bits;

typedef struct {
  __REG32  SW             : 2;
  __REG32  SWS            : 2;
  __REG32  HPRE           : 4;
  __REG32  PPRE1          : 3;
  __REG32  PPRE2          : 3;
  __REG32  ADCPRE         : 2;
  __REG32  PLLSRC         : 1;
  __REG32  PLLXTPRE       : 1;
  __REG32  PLLMUL         : 4;
  __REG32                 : 2;
  __REG32  MCO            : 3;
  __REG32                 : 5;
} __cfgr_bits;

typedef struct {
  __REG32  PREDIV1        : 4;
  __REG32                 : 28;
} __cfgr2_bits;

__IO_REG32_BIT(RCC_APB2ENR, RCC_BASE + APB2ENR,  __READ_WRITE, __apb2enr_bits);
__IO_REG32_BIT(RCC_CR,      RCC_BASE + CR,       __READ_WRITE, __cr_bits);
__IO_REG32_BIT(RCC_CFGR,    RCC_BASE + CFGR,     __READ_WRITE, __cfgr_bits);
__IO_REG32_BIT(RCC_CFGR2,   RCC_BASE + CFGR2,    __READ_WRITE, __cfgr2_bits);

#endif  //  STM32F100_FAMILY

//  GPIO

#ifdef STM32F100_FAMILY

#define GPIO_PORTA (*((volatile irs_u32 *) PORTA_BASE))
#define GPIO_PORTB (*((volatile irs_u32 *) PORTB_BASE))
#define GPIO_PORTC (*((volatile irs_u32 *) PORTC_BASE))
#define GPIO_PORTD (*((volatile irs_u32 *) PORTD_BASE))
#define GPIO_PORTE (*((volatile irs_u32 *) PORTE_BASE))

#define CRL   0x00
#define CRH   0x04
#define IDR   0x08
#define ODR   0x0C
#define BSRR  0x10
#define BRR   0x14
#define LCKR  0x18

#define GPIO_WIDTH                16
#define GPIO_PUSHPULL_OUT_MASK    2
#define GPIO_OPEN_DRAIN_OUT_MASK  6
#define GPIO_FLOAT_IN_MASK        4
#define GPIO_MASK_SIZE            4
#define GPIO_FULL_MASK            0xF

typedef struct {
  __REG32  MODE0          : 2;
  __REG32  CNF0           : 2;
  __REG32  MODE1          : 2;
  __REG32  CNF1           : 2;
  __REG32  MODE2          : 2;
  __REG32  CNF2           : 2;
  __REG32  MODE3          : 2;
  __REG32  CNF3           : 2;
  __REG32  MODE4          : 2;
  __REG32  CNF4           : 2;
  __REG32  MODE5          : 2;
  __REG32  CNF5           : 2;
  __REG32  MODE6          : 2;
  __REG32  CNF6           : 2;
  __REG32  MODE7          : 2;
  __REG32  CNF7           : 2;
} __crl_bits;

typedef struct {
  __REG32  MODE8          : 2;
  __REG32  CNF8           : 2;
  __REG32  MODE9          : 2;
  __REG32  CNF9           : 2;
  __REG32  MODE10         : 2;
  __REG32  CNF10          : 2;
  __REG32  MODE11         : 2;
  __REG32  CNF11          : 2;
  __REG32  MODE12         : 2;
  __REG32  CNF12          : 2;
  __REG32  MODE13         : 2;
  __REG32  CNF13          : 2;
  __REG32  MODE14         : 2;
  __REG32  CNF14          : 2;
  __REG32  MODE15         : 2;
  __REG32  CNF15          : 2;
} __crh_bits;

typedef struct {
  __REG32  BS0            : 1;
  __REG32  BS1            : 1;
  __REG32  BS2            : 1;
  __REG32  BS3            : 1;
  __REG32  BS4            : 1;
  __REG32  BS5            : 1;
  __REG32  BS6            : 1;
  __REG32  BS7            : 1;
  __REG32  BS8            : 1;
  __REG32  BS9            : 1;
  __REG32  BS10           : 1;
  __REG32  BS11           : 1;
  __REG32  BS12           : 1;
  __REG32  BS13           : 1;
  __REG32  BS14           : 1;
  __REG32  BS15           : 1;
  __REG32  BR0            : 1;
  __REG32  BR1            : 1;
  __REG32  BR2            : 1;
  __REG32  BR3            : 1;
  __REG32  BR4            : 1;
  __REG32  BR5            : 1;
  __REG32  BR6            : 1;
  __REG32  BR7            : 1;
  __REG32  BR8            : 1;
  __REG32  BR9            : 1;
  __REG32  BR10           : 1;
  __REG32  BR11           : 1;
  __REG32  BR12           : 1;
  __REG32  BR13           : 1;
  __REG32  BR14           : 1;
  __REG32  BR15           : 1;
} __bsrr_bits;

typedef struct {
  __BITS16;
  __REG32                 : 16;
} __hw_bits;

typedef struct {
  __BITS16;
  __REG32  LCKK           : 1;
  __REG32                 : 15;
} __lckr_bits;

__IO_REG32_BIT(PORTA_CRL,  PORTA_BASE + CRL,  __READ_WRITE, __crl_bits);
__IO_REG32_BIT(PORTA_CRH,  PORTA_BASE + CRH,  __READ_WRITE, __crh_bits);
__IO_REG32_BIT(PORTA_IDR,  PORTA_BASE + IDR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTA_ODR,  PORTA_BASE + ODR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTA_BSRR, PORTA_BASE + BSRR, __WRITE,      __bsrr_bits);
__IO_REG32_BIT(PORTA_LCKR, PORTA_BASE + LCKR, __READ_WRITE, __lckr_bits);

__IO_REG32_BIT(PORTB_CRL,  PORTA_BASE + CRL,  __READ_WRITE, __crl_bits);
__IO_REG32_BIT(PORTB_CRH,  PORTA_BASE + CRH,  __READ_WRITE, __crh_bits);
__IO_REG32_BIT(PORTB_IDR,  PORTA_BASE + IDR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTB_ODR,  PORTA_BASE + ODR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTB_BSRR, PORTA_BASE + BSRR, __WRITE,      __bsrr_bits);
__IO_REG32_BIT(PORTB_LCKR, PORTA_BASE + LCKR, __READ_WRITE, __lckr_bits);

__IO_REG32_BIT(PORTC_CRL,  PORTA_BASE + CRL,  __READ_WRITE, __crl_bits);
__IO_REG32_BIT(PORTC_CRH,  PORTA_BASE + CRH,  __READ_WRITE, __crh_bits);
__IO_REG32_BIT(PORTC_IDR,  PORTA_BASE + IDR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTC_ODR,  PORTA_BASE + ODR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTC_BSRR, PORTA_BASE + BSRR, __WRITE,      __bsrr_bits);
__IO_REG32_BIT(PORTC_LCKR, PORTA_BASE + LCKR, __READ_WRITE, __lckr_bits);

__IO_REG32_BIT(PORTD_CRL,  PORTA_BASE + CRL,  __READ_WRITE, __crl_bits);
__IO_REG32_BIT(PORTD_CRH,  PORTA_BASE + CRH,  __READ_WRITE, __crh_bits);
__IO_REG32_BIT(PORTD_IDR,  PORTA_BASE + IDR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTD_ODR,  PORTA_BASE + ODR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTD_BSRR, PORTA_BASE + BSRR, __WRITE,      __bsrr_bits);
__IO_REG32_BIT(PORTD_LCKR, PORTA_BASE + LCKR, __READ_WRITE, __lckr_bits);

__IO_REG32_BIT(PORTE_CRL,  PORTA_BASE + CRL,  __READ_WRITE, __crl_bits);
__IO_REG32_BIT(PORTE_CRH,  PORTA_BASE + CRH,  __READ_WRITE, __crh_bits);
__IO_REG32_BIT(PORTE_IDR,  PORTA_BASE + IDR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTE_ODR,  PORTA_BASE + ODR,  __READ,       __hw_bits);
__IO_REG32_BIT(PORTE_BSRR, PORTA_BASE + BSRR, __WRITE,      __bsrr_bits);
__IO_REG32_BIT(PORTE_LCKR, PORTA_BASE + LCKR, __READ_WRITE, __lckr_bits);

//  NVIC

////////////////////////////////////////////////////////////////////////////////

/* Interrupt Set-Enable Registers 0-31 */
typedef struct {
  __REG32  SETENA0        : 1;
  __REG32  SETENA1        : 1;
  __REG32  SETENA2        : 1;
  __REG32  SETENA3        : 1;
  __REG32  SETENA4        : 1;
  __REG32  SETENA5        : 1;
  __REG32  SETENA6        : 1;
  __REG32  SETENA7        : 1;
  __REG32  SETENA8        : 1;
  __REG32  SETENA9        : 1;
  __REG32  SETENA10       : 1;
  __REG32  SETENA11       : 1;
  __REG32  SETENA12       : 1;
  __REG32  SETENA13       : 1;
  __REG32  SETENA14       : 1;
  __REG32  SETENA15       : 1;
  __REG32  SETENA16       : 1;
  __REG32  SETENA17       : 1;
  __REG32  SETENA18       : 1;
  __REG32  SETENA19       : 1;
  __REG32  SETENA20       : 1;
  __REG32  SETENA21       : 1;
  __REG32  SETENA22       : 1;
  __REG32  SETENA23       : 1;
  __REG32  SETENA24       : 1;
  __REG32  SETENA25       : 1;
  __REG32  SETENA26       : 1;
  __REG32  SETENA27       : 1;
  __REG32  SETENA28       : 1;
  __REG32  SETENA29       : 1;
  __REG32  SETENA30       : 1;
  __REG32  SETENA31       : 1;
} __setena0_bits;

/* Interrupt Set-Enable Registers 32-63 */
typedef struct {
  __REG32  SETENA32       : 1;
  __REG32  SETENA33       : 1;
  __REG32  SETENA34       : 1;
  __REG32  SETENA35       : 1;
  __REG32  SETENA36       : 1;
  __REG32  SETENA37       : 1;
  __REG32  SETENA38       : 1;
  __REG32  SETENA39       : 1;
  __REG32  SETENA40       : 1;
  __REG32  SETENA41       : 1;
  //__REG32  NVIC_ETH_INT   : 1;
  __REG32  SETENA42 : 1;
  __REG32  SETENA43       : 1;
  __REG32  SETENA44       : 1;
  __REG32  SETENA45       : 1;
  __REG32  SETENA46       : 1;
  __REG32  SETENA47       : 1;
  __REG32  SETENA48       : 1;
  __REG32  SETENA49       : 1;
  __REG32  SETENA50       : 1;
  __REG32  SETENA51       : 1;
  __REG32  SETENA52       : 1;
  __REG32  SETENA53       : 1;
  //__REG32  NVIC_PORTJ_INT : 1;
  __REG32  SETENA54 : 1;
  __REG32  SETENA55       : 1;
  __REG32  SETENA56       : 1;
  __REG32  SETENA57       : 1;
  __REG32  SETENA58       : 1;
  __REG32  SETENA59       : 1;
  __REG32  SETENA60       : 1;
  __REG32  SETENA61       : 1;
  __REG32  SETENA62       : 1;
  __REG32  SETENA63       : 1;
} __setena1_bits;

/* Interrupt Clear-Enable Registers 0-31 */
typedef struct {
  __REG32  CLRENA0        : 1;
  __REG32  CLRENA1        : 1;
  __REG32  CLRENA2        : 1;
  __REG32  CLRENA3        : 1;
  __REG32  CLRENA4        : 1;
  __REG32  CLRENA5        : 1;
  __REG32  CLRENA6        : 1;
  __REG32  CLRENA7        : 1;
  __REG32  CLRENA8        : 1;
  __REG32  CLRENA9        : 1;
  __REG32  CLRENA10       : 1;
  __REG32  CLRENA11       : 1;
  __REG32  CLRENA12       : 1;
  __REG32  CLRENA13       : 1;
  __REG32  CLRENA14       : 1;
  __REG32  CLRENA15       : 1;
  __REG32  CLRENA16       : 1;
  __REG32  CLRENA17       : 1;
  __REG32  CLRENA18       : 1;
  __REG32  CLRENA19       : 1;
  __REG32  CLRENA20       : 1;
  __REG32  CLRENA21       : 1;
  __REG32  CLRENA22       : 1;
  __REG32  CLRENA23       : 1;
  __REG32  CLRENA24       : 1;
  __REG32  CLRENA25       : 1;
  __REG32  CLRENA26       : 1;
  __REG32  CLRENA27       : 1;
  __REG32  CLRENA28       : 1;
  __REG32  CLRENA29       : 1;
  __REG32  CLRENA30       : 1;
  __REG32  CLRENA31       : 1;
} __clrena0_bits;

/* Interrupt Clear-Enable Registers 32-63 */
typedef struct {
  __REG32  CLRENA32       : 1;
  __REG32  CLRENA33       : 1;
  __REG32  CLRENA34       : 1;
  __REG32  CLRENA35       : 1;
  __REG32  CLRENA36       : 1;
  __REG32  CLRENA37       : 1;
  __REG32  CLRENA38       : 1;
  __REG32  CLRENA39       : 1;
  __REG32  CLRENA40       : 1;
  __REG32  CLRENA41       : 1;
  __REG32  CLRENA42       : 1;
  __REG32  CLRENA43       : 1;
  __REG32  CLRENA44       : 1;
  __REG32  CLRENA45       : 1;
  __REG32  CLRENA46       : 1;
  __REG32  CLRENA47       : 1;
  __REG32  CLRENA48       : 1;
  __REG32  CLRENA49       : 1;
  __REG32  CLRENA50       : 1;
  __REG32  CLRENA51       : 1;
  __REG32  CLRENA52       : 1;
  __REG32  CLRENA53       : 1;
  __REG32  CLRENA54       : 1;
  __REG32  CLRENA55       : 1;
  __REG32  CLRENA56       : 1;
  __REG32  CLRENA57       : 1;
  __REG32  CLRENA58       : 1;
  __REG32  CLRENA59       : 1;
  __REG32  CLRENA60       : 1;
  __REG32  CLRENA61       : 1;
  __REG32  CLRENA62       : 1;
  __REG32  CLRENA63       : 1;
} __clrena1_bits;

/* Interrupt Set-Pending Register 0-31 */
typedef struct {
  __REG32  SETPEND0       : 1;
  __REG32  SETPEND1       : 1;
  __REG32  SETPEND2       : 1;
  __REG32  SETPEND3       : 1;
  __REG32  SETPEND4       : 1;
  __REG32  SETPEND5       : 1;
  __REG32  SETPEND6       : 1;
  __REG32  SETPEND7       : 1;
  __REG32  SETPEND8       : 1;
  __REG32  SETPEND9       : 1;
  __REG32  SETPEND10      : 1;
  __REG32  SETPEND11      : 1;
  __REG32  SETPEND12      : 1;
  __REG32  SETPEND13      : 1;
  __REG32  SETPEND14      : 1;
  __REG32  SETPEND15      : 1;
  __REG32  SETPEND16      : 1;
  __REG32  SETPEND17      : 1;
  __REG32  SETPEND18      : 1;
  __REG32  SETPEND19      : 1;
  __REG32  SETPEND20      : 1;
  __REG32  SETPEND21      : 1;
  __REG32  SETPEND22      : 1;
  __REG32  SETPEND23      : 1;
  __REG32  SETPEND24      : 1;
  __REG32  SETPEND25      : 1;
  __REG32  SETPEND26      : 1;
  __REG32  SETPEND27      : 1;
  __REG32  SETPEND28      : 1;
  __REG32  SETPEND29      : 1;
  __REG32  SETPEND30      : 1;
  __REG32  SETPEND31      : 1;
} __setpend0_bits;

/* Interrupt Set-Pending Register 32-63 */
typedef struct {
  __REG32  SETPEND32      : 1;
  __REG32  SETPEND33      : 1;
  __REG32  SETPEND34      : 1;
  __REG32  SETPEND35      : 1;
  __REG32  SETPEND36      : 1;
  __REG32  SETPEND37      : 1;
  __REG32  SETPEND38      : 1;
  __REG32  SETPEND39      : 1;
  __REG32  SETPEND40      : 1;
  __REG32  SETPEND41      : 1;
  __REG32  SETPEND42      : 1;
  __REG32  SETPEND43      : 1;
  __REG32  SETPEND44      : 1;
  __REG32  SETPEND45      : 1;
  __REG32  SETPEND46      : 1;
  __REG32  SETPEND47      : 1;
  __REG32  SETPEND48      : 1;
  __REG32  SETPEND49      : 1;
  __REG32  SETPEND50      : 1;
  __REG32  SETPEND51      : 1;
  __REG32  SETPEND52      : 1;
  __REG32  SETPEND53      : 1;
  __REG32  SETPEND54      : 1;
  __REG32  SETPEND55      : 1;
  __REG32  SETPEND56      : 1;
  __REG32  SETPEND57      : 1;
  __REG32  SETPEND58      : 1;
  __REG32  SETPEND59      : 1;
  __REG32  SETPEND60      : 1;
  __REG32  SETPEND61      : 1;
  __REG32  SETPEND62      : 1;
  __REG32  SETPEND63      : 1;
} __setpend1_bits;

/* Interrupt Clear-Pending Register 0-31 */
typedef struct {
  __REG32  CLRPEND0       : 1;
  __REG32  CLRPEND1       : 1;
  __REG32  CLRPEND2       : 1;
  __REG32  CLRPEND3       : 1;
  __REG32  CLRPEND4       : 1;
  __REG32  CLRPEND5       : 1;
  __REG32  CLRPEND6       : 1;
  __REG32  CLRPEND7       : 1;
  __REG32  CLRPEND8       : 1;
  __REG32  CLRPEND9       : 1;
  __REG32  CLRPEND10      : 1;
  __REG32  CLRPEND11      : 1;
  __REG32  CLRPEND12      : 1;
  __REG32  CLRPEND13      : 1;
  __REG32  CLRPEND14      : 1;
  __REG32  CLRPEND15      : 1;
  __REG32  CLRPEND16      : 1;
  __REG32  CLRPEND17      : 1;
  __REG32  CLRPEND18      : 1;
  __REG32  CLRPEND19      : 1;
  __REG32  CLRPEND20      : 1;
  __REG32  CLRPEND21      : 1;
  __REG32  CLRPEND22      : 1;
  __REG32  CLRPEND23      : 1;
  __REG32  CLRPEND24      : 1;
  __REG32  CLRPEND25      : 1;
  __REG32  CLRPEND26      : 1;
  __REG32  CLRPEND27      : 1;
  __REG32  CLRPEND28      : 1;
  __REG32  CLRPEND29      : 1;
  __REG32  CLRPEND30      : 1;
  __REG32  CLRPEND31      : 1;
} __clrpend0_bits;

/* Interrupt Clear-Pending Register 32-63 */
typedef struct {
  __REG32  CLRPEND32      : 1;
  __REG32  CLRPEND33      : 1;
  __REG32  CLRPEND34      : 1;
  __REG32  CLRPEND35      : 1;
  __REG32  CLRPEND36      : 1;
  __REG32  CLRPEND37      : 1;
  __REG32  CLRPEND38      : 1;
  __REG32  CLRPEND39      : 1;
  __REG32  CLRPEND40      : 1;
  __REG32  CLRPEND41      : 1;
  __REG32  CLRPEND42      : 1;
  __REG32  CLRPEND43      : 1;
  __REG32  CLRPEND44      : 1;
  __REG32  CLRPEND45      : 1;
  __REG32  CLRPEND46      : 1;
  __REG32  CLRPEND47      : 1;
  __REG32  CLRPEND48      : 1;
  __REG32  CLRPEND49      : 1;
  __REG32  CLRPEND50      : 1;
  __REG32  CLRPEND51      : 1;
  __REG32  CLRPEND52      : 1;
  __REG32  CLRPEND53      : 1;
  __REG32  CLRPEND54      : 1;
  __REG32  CLRPEND55      : 1;
  __REG32  CLRPEND56      : 1;
  __REG32  CLRPEND57      : 1;
  __REG32  CLRPEND58      : 1;
  __REG32  CLRPEND59      : 1;
  __REG32  CLRPEND60      : 1;
  __REG32  CLRPEND61      : 1;
  __REG32  CLRPEND62      : 1;
  __REG32  CLRPEND63      : 1;
} __clrpend1_bits;

/* Active Bit Register 0-31 */
typedef struct {
  __REG32  ACTIVE0        : 1;
  __REG32  ACTIVE1        : 1;
  __REG32  ACTIVE2        : 1;
  __REG32  ACTIVE3        : 1;
  __REG32  ACTIVE4        : 1;
  __REG32  ACTIVE5        : 1;
  __REG32  ACTIVE6        : 1;
  __REG32  ACTIVE7        : 1;
  __REG32  ACTIVE8        : 1;
  __REG32  ACTIVE9        : 1;
  __REG32  ACTIVE10       : 1;
  __REG32  ACTIVE11       : 1;
  __REG32  ACTIVE12       : 1;
  __REG32  ACTIVE13       : 1;
  __REG32  ACTIVE14       : 1;
  __REG32  ACTIVE15       : 1;
  __REG32  ACTIVE16       : 1;
  __REG32  ACTIVE17       : 1;
  __REG32  ACTIVE18       : 1;
  __REG32  ACTIVE19       : 1;
  __REG32  ACTIVE20       : 1;
  __REG32  ACTIVE21       : 1;
  __REG32  ACTIVE22       : 1;
  __REG32  ACTIVE23       : 1;
  __REG32  ACTIVE24       : 1;
  __REG32  ACTIVE25       : 1;
  __REG32  ACTIVE26       : 1;
  __REG32  ACTIVE27       : 1;
  __REG32  ACTIVE28       : 1;
  __REG32  ACTIVE29       : 1;
  __REG32  ACTIVE30       : 1;
  __REG32  ACTIVE31       : 1;
} __active0_bits;

/* Active Bit Register 32-63 */
typedef struct {
  __REG32  ACTIVE32       : 1;
  __REG32  ACTIVE33       : 1;
  __REG32  ACTIVE34       : 1;
  __REG32  ACTIVE35       : 1;
  __REG32  ACTIVE36       : 1;
  __REG32  ACTIVE37       : 1;
  __REG32  ACTIVE38       : 1;
  __REG32  ACTIVE39       : 1;
  __REG32  ACTIVE40       : 1;
  __REG32  ACTIVE41       : 1;
  __REG32  ACTIVE42       : 1;
  __REG32  ACTIVE43       : 1;
  __REG32  ACTIVE44       : 1;
  __REG32  ACTIVE45       : 1;
  __REG32  ACTIVE46       : 1;
  __REG32  ACTIVE47       : 1;
  __REG32  ACTIVE48       : 1;
  __REG32  ACTIVE49       : 1;
  __REG32  ACTIVE50       : 1;
  __REG32  ACTIVE51       : 1;
  __REG32  ACTIVE52       : 1;
  __REG32  ACTIVE53       : 1;
  __REG32  ACTIVE54       : 1;
  __REG32  ACTIVE55       : 1;
  __REG32  ACTIVE56       : 1;
  __REG32  ACTIVE57       : 1;
  __REG32  ACTIVE58       : 1;
  __REG32  ACTIVE59       : 1;
  __REG32  ACTIVE60       : 1;
  __REG32  ACTIVE61       : 1;
  __REG32  ACTIVE62       : 1;
  __REG32  ACTIVE63       : 1;
} __active1_bits;

////////////////////////////////////////////////////////////////////////////////

#endif  //  STM32F100_FAMILY

#endif  //  armregs_stm32H
