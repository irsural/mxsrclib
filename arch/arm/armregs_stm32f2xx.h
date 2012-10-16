#ifndef armregs_stm32f2xxH
#define armregs_stm32f2xxH

#include <irsdefs.h>

#include <armiomacros.h>

#include <irsfinal.h>

#ifdef IRS_STM32F2xx

#define IRS_RNG_BASE              0x50060800
#define IRS_DCMI_BASE             0x50050000
#define IRS_USB_OTG_FS_BASE       0x50000000
#define IRS_USB_OTG_HS_BASE       0x40040000
#define IRS_ETHERNET_BASE         0x40028000
#define IRS_DMA2_BASE             0x40026400
#define IRS_DMA1_BASE             0x40026000
#define IRS_BKPSRAM_BASE          0x40024000
#define IRS_FLASH_INTERFACE_BASE  0x40023C00
#define IRS_RCC_BASE              0x40023800
#define IRS_CRC_BASE              0x40023000
#define IRS_PORTI_BASE            0x40022000
#define IRS_PORTH_BASE            0x40021C00
#define IRS_PORTG_BASE            0x40021800
#define IRS_PORTF_BASE            0x40021400
#define IRS_PORTE_BASE            0x40021000
#define IRS_PORTD_BASE            0x40020C00
#define IRS_PORTC_BASE            0x40020800
#define IRS_PORTB_BASE            0x40020400
#define IRS_PORTA_BASE            0x40020000
#define IRS_TIM11_BASE            0x40014800
#define IRS_TIM10_BASE            0x40014400
#define IRS_TIM9_BASE             0x40014000
#define IRS_EXTI_BASE             0x40013C00
#define IRS_SYSCFG_BASE           0x40013800
#define IRS_SPI1_BASE             0x40013000
#define IRS_SDIO_BASE             0x40012C00
#define IRS_ADC1_ADC2_ADC3_BASE   0x40012000
#define IRS_ADC1_BASE             0x40012000
#define IRS_ADC2_BASE             0x40012100
#define IRS_ADC3_BASE             0x40012200
#define IRS_USART6_BASE           0x40011400
#define IRS_USART1_BASE           0x40011000
#define IRS_TIM8_PWM2_BASE        0x40010400
#define IRS_TIM1_PWM1_BASE        0x40010000
#define IRS_DAC1_DAC2_BASE        0x40007400
#define IRS_PWR_BASE              0x40007000
#define IRS_BxCAN2_BASE           0x40006800
#define IRS_BxCAN1_BASE           0x40006400
#define IRS_I2C3_BASE             0x40005C00
#define IRS_I2C2_BASE             0x40005800
#define IRS_I2C1_BASE             0x40005400
#define IRS_UART5_BASE            0x40005000
#define IRS_UART4_BASE            0x40004C00
#define IRS_USART3_BASE           0x40004800
#define IRS_USART2_BASE           0x40004400
#define IRS_SPI3_I2S3_BASE        0x40003C00
#define IRS_SPI2_I2S2_BASE        0x40003800
#define IRS_IWDG_BASE             0x40003000
#define IRS_WWDG_BASE             0x40002C00
#define IRS_RTC_BKP_BASE          0x40002800
#define IRS_TIM14_BASE            0x40002000
#define IRS_TIM13_BASE            0x40001C00
#define IRS_TIM12_BASE            0x40001800
#define IRS_TIM7_BASE             0x40001400
#define IRS_TIM6_BASE             0x40001000
#define IRS_TIM5_BASE             0x40000C00
#define IRS_TIM4_BASE             0x40000800
#define IRS_TIM3_BASE             0x40000400
#define IRS_TIM2_BASE             0x40000000

enum gpio_channel_t {
  PNONE = -1,
  PA0 = 0,
  PA1,
  PA2,
  PA3,
  PA4,
  PA5,
  PA6,
  PA7,
  PA8,
  PA9,
  PA10,
  PA11,
  PA12,
  PA13,
  PA14,
  PA15,

  PB0,
  PB1,
  PB2,
  PB3,
  PB4,
  PB5,
  PB6,
  PB7,
  PB8,
  PB9,
  PB10,
  PB11,
  PB12,
  PB13,
  PB14,
  PB15,

  PC0,
  PC1,
  PC2,
  PC3,
  PC4,
  PC5,
  PC6,
  PC7,
  PC8,
  PC9,
  PC10,
  PC11,
  PC12,
  PC13,
  PC14,
  PC15,

  PD0,
  PD1,
  PD2,
  PD3,
  PD4,
  PD5,
  PD6,
  PD7,
  PD8,
  PD9,
  PD10,
  PD11,
  PD12,
  PD13,
  PD14,
  PD15,

  PE0,
  PE1,
  PE2,
  PE3,
  PE4,
  PE5,
  PE6,
  PE7,
  PE8,
  PE9,
  PE10,
  PE11,
  PE12,
  PE13,
  PE14,
  PE15,

  PF0,
  PF1,
  PF2,
  PF3,
  PF4,
  PF5,
  PF6,
  PF7,
  PF8,
  PF9,
  PF10,
  PF11,
  PF12,
  PF13,
  PF14,
  PF15,

  PG0,
  PG1,
  PG2,
  PG3,
  PG4,
  PG5,
  PG6,
  PG7,
  PG8,
  PG9,
  PG10,
  PG11,
  PG12,
  PG13,
  PG14,
  PG15,

  PH0,
  PH1,
  PH2,
  PH3,
  PH4,
  PH5,
  PH6,
  PH7,
  PH8,
  PH9,
  PH10,
  PH11,
  PH12,
  PH13,
  PH14,
  PH15,

  PI0,
  PI1,
  PI2,
  PI3,
  PI4,
  PI5,
  PI6,
  PI7,
  PI8,
  PI9,
  PI10,
  PI11,
  PI12,
  PI13,
  PI14,
  PI15
};

enum st_timer_name_t {
  stm_tim_none,
  stn_tim1,
  stn_tim2,
  stn_tim3,
  stn_tim4,
  stn_tim5,
  stn_tim6,
  stn_tim7,
  stn_tim8,
  stn_tim9,
  stn_tim10,
  stn_tim11,
  stn_tim12,
  stn_tim13,
  stn_tim14
};

inline size_t get_timer_address(st_timer_name_t a_st_timer_name)
{
  switch (a_st_timer_name) {
    case stn_tim1: return IRS_TIM1_PWM1_BASE;
    case stn_tim2: return IRS_TIM2_BASE;
    case stn_tim3: return IRS_TIM3_BASE;
    case stn_tim4: return IRS_TIM4_BASE;
    case stn_tim5: return IRS_TIM5_BASE;
    case stn_tim6: return IRS_TIM6_BASE;
    case stn_tim7: return IRS_TIM7_BASE;
    case stn_tim8: return IRS_TIM8_PWM2_BASE;
    case stn_tim9: return IRS_TIM9_BASE;
    case stn_tim10: return IRS_TIM10_BASE;
    case stn_tim11: return IRS_TIM11_BASE;
    case stn_tim12: return IRS_TIM12_BASE;
    case stn_tim13: return IRS_TIM13_BASE;
    case stn_tim14: return IRS_TIM14_BASE;
  }
  return 0;
}

// RCC

#define RCC_CR_S          0x00
#define RCC_PLLCFGR_S     0x04
#define RCC_CFGR_S        0x08
#define RCC_CIR_S         0x0C
#define RCC_AHB1RSTR_S    0x10
#define RCC_AHB2RSTR_S    0x14
#define RCC_AHB3RSTR_S    0x18
#define RCC_APB1RSTR_S    0x20
#define RCC_APB2RSTR_S    0x24
#define RCC_AHB1ENR_S     0x30
#define RCC_AHB2ENR_S     0x34
#define RCC_AHB3ENR_S     0x38
#define RCC_APB1ENR_S     0x40
#define RCC_APB2ENR_S     0x44
#define RCC_AHB1LPENR_S   0x50
#define RCC_AHB2LPENR_S   0x54
#define RCC_AHB3LPENR_S   0x58
#define RCC_APB1LPENR_S   0x60
#define RCC_APB2LPENR_S   0x64
#define RCC_BDCR_S        0x70
#define RCC_CSR_S         0x74
#define RCC_SSCGR_S       0x80
#define RCC_PLLI2SCFGR_S  0x84

/* Clock control register (RCC_CR) */
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
  __REG32  PLL2ON         : 1;
  __REG32  PLL2RDY        : 1;
  __REG32                 : 4;
} __rcc_cr_bits;

/* RCC PLL configuration register (RCC_PLLCFGR) */
typedef struct {
  __REG32  PLLM           : 6;
  __REG32  PLLN           : 9;
  __REG32                 : 1;
  __REG32  PLLP           : 2;
  __REG32                 : 4;
  __REG32  PLLSRC         : 1;
  __REG32                 : 1;
  __REG32  PLLQ           : 4;
  __REG32                 : 4;
} __rcc_pllcfgr_bits;

/* Clock configuration register (RCC_CFGR) */
typedef struct {
  __REG32  SW             : 2;
  __REG32  SWS            : 2;
  __REG32  HPRE           : 4;
  __REG32                 : 2;
  __REG32  PPRE1          : 3;
  __REG32  PPRE2          : 3;
  __REG32  RTCPRE         : 5;
  __REG32  MCO1           : 2;
  __REG32  I2SSCR         : 1;
  __REG32  MCO1PRE        : 3;
  __REG32  MCO2PRE        : 3;
  __REG32  MCO2           : 2;
} __rcc_cfgr_bits;

/* Clock interrupt register (RCC_CIR) */
typedef struct {
  __REG32  LSIRDYF        : 1;
  __REG32  LSERDYF        : 1;
  __REG32  HSIRDYF        : 1;
  __REG32  HSERDYF        : 1;
  __REG32  PLLRDYF        : 1;
  __REG32  PLL2RDYF       : 1;
  __REG32                 : 1;
  __REG32  CSSF           : 1;
  __REG32  LSIRDYIE       : 1;
  __REG32  LSERDYIE       : 1;
  __REG32  HSIRDYIE       : 1;
  __REG32  HSERDYIE       : 1;
  __REG32  PLLRDYIE       : 1;
  __REG32  PLL2RDYIE      : 1;
  __REG32                 : 2;
  __REG32  LSIRDYC        : 1;
  __REG32  LSERDYC        : 1;
  __REG32  HSIRDYC        : 1;
  __REG32  HSERDYC        : 1;
  __REG32  PLLRDYC        : 1;
  __REG32  PLL2RDYC       : 1;
  __REG32                 : 1;
  __REG32  CSSC           : 1;
  __REG32                 : 8;
} __rcc_cir_bits;

/* RCC AHB1 peripheral reset register (RCC_AHB1RSTR) */
typedef struct {
  __REG32  GPIOARST       : 1;
  __REG32  GPIOBRST       : 1;
  __REG32  GPIOCRST       : 1;
  __REG32  GPIODRST       : 1;
  __REG32  GPIOERST       : 1;
  __REG32  GPIOFRST       : 1;
  __REG32  GPIOGRST       : 1;
  __REG32  GPIOHRST       : 1;
  __REG32  GPIOIRST       : 1;
  __REG32                 : 3;
  __REG32  CRCRST         : 1;
  __REG32                 : 8;
  __REG32  DMA1RST        : 1;
  __REG32  DMA2RST        : 1;
  __REG32                 : 2;
  __REG32  ETHMACRST      : 1;
  __REG32                 : 3;
  __REG32  OTGHSRST       : 1;
  __REG32                 : 2;
} __rcc_ahb1rstr_bits;

/* RCC AHB2 peripheral reset register (RCC_AHB2RSTR) */
typedef struct {
  __REG32  DCMIRST        : 1;
  __REG32                 : 3;
  __REG32  CRYPRST        : 1;
  __REG32  HASHRST        : 1;
  __REG32  RNGRST         : 1;
  __REG32  OTGFSRST       : 1;
  __REG32                 :24;
} __rcc_ahb2rstr_bits;

/* RCC AHB3 peripheral reset register (RCC_AHB3RSTR) */
typedef struct {
  __REG32  FSMCRST        : 1;
  __REG32                 :31;
} __rcc_ahb3rstr_bits;

/* RCC APB1 peripheral reset register (RCC_APB1RSTR) */
typedef struct {
  __REG32  TIM2RST        : 1;
  __REG32  TIM3RST        : 1;
  __REG32  TIM4RST        : 1;
  __REG32  TIM5RST        : 1;
  __REG32  TIM6RST        : 1;
  __REG32  TIM7RST        : 1;
  __REG32  TIM12RST       : 1;
  __REG32  TIM13RST       : 1;
  __REG32  TIM14RST       : 1;
  __REG32                 : 2;
  __REG32  WWDGRST        : 1;
  __REG32                 : 2;
  __REG32  SPI2RST        : 1;
  __REG32  SPI3RST        : 1;
  __REG32                 : 1;
  __REG32  UART2RST       : 1;
  __REG32  UART3RST       : 1;
  __REG32  UART4RST       : 1;
  __REG32  UART5RST       : 1;
  __REG32  I2C1RST        : 1;
  __REG32  I2C2RST        : 1;
  __REG32  I2C3RST        : 1;
  __REG32                 : 1;
  __REG32  CAN1RST        : 1;
  __REG32  CAN2RST        : 1;
  __REG32                 : 1;
  __REG32  PWRRST         : 1;
  __REG32  DACRST         : 1;
  __REG32                 : 2;
} __rcc_apb1rstr_bits;

/* RCC APB2 peripheral reset register (RCC_APB2RSTR) */
typedef struct {
  __REG32  TIM1RST        : 1;
  __REG32  TIM8RST        : 1;
  __REG32                 : 2;
  __REG32  USART1RST      : 1;
  __REG32  USART6RST      : 1;
  __REG32                 : 2;
  __REG32  ADCRST         : 1;
  __REG32                 : 2;
  __REG32  SDIORST        : 1;
  __REG32  SPI1RST        : 1;
  __REG32                 : 1;
  __REG32  SYSCFGRST      : 1;
  __REG32                 : 1;
  __REG32  TIM9RST        : 1;
  __REG32  TIM10RST       : 1;
  __REG32  TIM11RST       : 1;
  __REG32                 :13;
} __rcc_apb2rstr_bits;

/* RCC AHB1 peripheral clock register (RCC_AHB1ENR) */
typedef struct {
  __REG32  GPIOAEN        : 1;
  __REG32  GPIOBEN        : 1;
  __REG32  GPIOCEN        : 1;
  __REG32  GPIODEN        : 1;
  __REG32  GPIOEEN        : 1;
  __REG32  GPIOFEN        : 1;
  __REG32  GPIOGEN        : 1;
  __REG32  GPIOHEN        : 1;
  __REG32  GPIOIEN        : 1;
  __REG32                 : 3;
  __REG32  CRCEN          : 1;
  __REG32                 : 5;
  __REG32  BKPSRAMEN      : 1;
  __REG32                 : 2;
  __REG32  DMA1EN         : 1;
  __REG32  DMA2EN         : 1;
  __REG32                 : 2;
  __REG32  ETHMACEN       : 1;
  __REG32  ETHMACTXEN     : 1;
  __REG32  ETHMACRXEN     : 1;
  __REG32  ETHMACPTPEN    : 1;
  __REG32  OTGHSEN        : 1;
  __REG32  OTGHSULPIEN    : 1;
  __REG32                 : 1;
} __rcc_ahb1enr_bits;

/* RCC AHB2 peripheral clock register (RCC_AHB2ENR) */
typedef struct {
  __REG32  DCMIEN         : 1;
  __REG32                 : 3;
  __REG32  CRYPEN         : 1;
  __REG32  HASHEN         : 1;
  __REG32  RNGEN          : 1;
  __REG32  OTGFSEN        : 1;
  __REG32                 :24;
} __rcc_ahb2enr_bits;

/* RCC AHB3 peripheral clock register (RCC_AHB3ENR) */
typedef struct {
  __REG32  FSMCEN         : 1;
  __REG32                 :31;
} __rcc_ahb3enr_bits;

/* RCC APB1 peripheral clock enable register (RCC_APB1ENR) */
typedef struct {
  __REG32  TIM2EN        : 1;
  __REG32  TIM3EN        : 1;
  __REG32  TIM4EN        : 1;
  __REG32  TIM5EN        : 1;
  __REG32  TIM6EN        : 1;
  __REG32  TIM7EN        : 1;
  __REG32  TIM12EN       : 1;
  __REG32  TIM13EN       : 1;
  __REG32  TIM14EN       : 1;
  __REG32                : 2;
  __REG32  WWDGEN        : 1;
  __REG32                : 2;
  __REG32  SPI2EN        : 1;
  __REG32  SPI3EN        : 1;
  __REG32                : 1;
  __REG32  USART2EN      : 1;
  __REG32  USART3EN      : 1;
  __REG32  UART4EN       : 1;
  __REG32  UART5EN       : 1;
  __REG32  I2C1EN        : 1;
  __REG32  I2C2EN        : 1;
  __REG32  I2C3EN        : 1;
  __REG32                : 1;
  __REG32  CAN1EN        : 1;
  __REG32  CAN2EN        : 1;
  __REG32                : 1;
  __REG32  PWREN         : 1;
  __REG32  DACEN         : 1;
  __REG32                : 2;
} __rcc_apb1enr_bits;

/* RCC APB2 peripheral clock enable register (RCC_APB2ENR) */
typedef struct {
  __REG32  TIM1EN        : 1;
  __REG32  TIM8EN        : 1;
  __REG32                : 2;
  __REG32  USART1EN      : 1;
  __REG32  USART6EN      : 1;
  __REG32                : 2;
  __REG32  ADC1EN        : 1;
  __REG32  ADC2EN        : 1;
  __REG32  ADC3EN        : 1;
  __REG32  SDIOEN        : 1;
  __REG32  SPI1EN        : 1;
  __REG32                : 1;
  __REG32  SYSCFGEN      : 1;
  __REG32                : 1;
  __REG32  TIM9EN        : 1;
  __REG32  TIM10EN       : 1;
  __REG32  TIM11EN       : 1;
  __REG32                :13;
} __rcc_apb2enr_bits;

/* RCC AHB1 peripheral clock enable in low power mode register (RCC_AHB1LPENR) */
typedef struct {
  __REG32  GPIOALPEN        : 1;
  __REG32  GPIOBLPEN        : 1;
  __REG32  GPIOCLPEN        : 1;
  __REG32  GPIODLPEN        : 1;
  __REG32  GPIOELPEN        : 1;
  __REG32  GPIOFLPEN        : 1;
  __REG32  GPIOGLPEN        : 1;
  __REG32  GPIOHLPEN        : 1;
  __REG32  GPIOILPEN        : 1;
  __REG32                   : 3;
  __REG32  CRCLPEN          : 1;
  __REG32                   : 2;
  __REG32  FLITFLPEN        : 1;
  __REG32  SRAM1LPEN        : 1;
  __REG32  SRAM2LPEN        : 1;
  __REG32  BKPSRAMLPEN      : 1;
  __REG32                   : 2;
  __REG32  DMA1LPEN         : 1;
  __REG32  DMA2LPEN         : 1;
  __REG32                   : 2;
  __REG32  ETHMACLPEN       : 1;
  __REG32  ETHTXLPEN        : 1;
  __REG32  ETHMACRXLPEN     : 1;
  __REG32  ETHMACPTPLPEN    : 1;
  __REG32  OTGHSLPEN        : 1;
  __REG32  OTGHSULPILPEN    : 1;
  __REG32                   : 1;
} __rcc_ahb1lpenr_bits;

/* RCC AHB2 peripheral clock enable in low power mode register (RCC AHB2 peripheral clock enable in low power mode register (RCC_AHB2LPENR) */
typedef struct {
  __REG32  DCMILPEN         : 1;
  __REG32                   : 3;
  __REG32  CRYPLPEN         : 1;
  __REG32  HASHLPEN         : 1;
  __REG32  RNGLPEN          : 1;
  __REG32  OTGFSLPEN        : 1;
  __REG32                   :24;
} __rcc_ahb2lpenr_bits;

/* RCC AHB3 peripheral clock enable in low power mode register (RCC_AHB3LPENR) */
typedef struct {
  __REG32  FSMCLPEN         : 1;
  __REG32                   :31;
} __rcc_ahb3lpenr_bits;

/* RCC APB1 peripheral clock enable in low power mode register (RCC_APB1LPENR) */
typedef struct {
  __REG32  TIM2LPEN        : 1;
  __REG32  TIM3LPEN        : 1;
  __REG32  TIM4LPEN        : 1;
  __REG32  TIM5LPEN        : 1;
  __REG32  TIM6LPEN        : 1;
  __REG32  TIM7LPEN        : 1;
  __REG32  TIM12LPEN       : 1;
  __REG32  TIM13LPEN       : 1;
  __REG32  TIM14LPEN       : 1;
  __REG32                  : 2;
  __REG32  WWDGLPEN        : 1;
  __REG32                  : 2;
  __REG32  SPI2LPEN        : 1;
  __REG32  SPI3LPEN        : 1;
  __REG32                  : 1;
  __REG32  USART2LPEN      : 1;
  __REG32  USART3LPEN      : 1;
  __REG32  USART4LPEN      : 1;
  __REG32  USART5LPEN      : 1;
  __REG32  I2C1LPEN        : 1;
  __REG32  I2C2LPEN        : 1;
  __REG32  I2C3LPEN        : 1;
  __REG32                  : 1;
  __REG32  CAN1LPEN        : 1;
  __REG32  CAN2LPEN        : 1;
  __REG32                  : 1;
  __REG32  PWRLPEN         : 1;
  __REG32  DACLPEN         : 1;
  __REG32                  : 2;
} __rcc_apb1lpenr_bits;

/* RCC APB2 peripheral clock enabled in low power mode register (RCC_APB2LPENR) */
typedef struct {
  __REG32  TIM1LPEN        : 1;
  __REG32  TIM8LPEN        : 1;
  __REG32                  : 2;
  __REG32  USART1LPEN      : 1;
  __REG32  USART6LPEN      : 1;
  __REG32                  : 2;
  __REG32  ADC1LPEN        : 1;
  __REG32  ADC2LPEN        : 1;
  __REG32  ADC3LPEN        : 1;
  __REG32  SDIOLPEN        : 1;
  __REG32  SPI1LPEN        : 1;
  __REG32                  : 1;
  __REG32  SYSCFGLPEN      : 1;
  __REG32                  : 1;
  __REG32  TIM9LPEN        : 1;
  __REG32  TIM10LPEN       : 1;
  __REG32  TIM11LPEN       : 1;
  __REG32                  :13;
} __rcc_apb2lpenr_bits;

/* Backup domain control register (RCC_BDCR) */
typedef struct {
  __REG32  LSEON          : 1;
  __REG32  LSERDY         : 1;
  __REG32  LSEBYP         : 1;
  __REG32                 : 5;
  __REG32  RTCSEL         : 2;
  __REG32                 : 5;
  __REG32  RTCEN          : 1;
  __REG32  BDRST          : 1;
  __REG32                 :15;
} __rcc_bdcr_bits;

/* Control/status register (RCC_CSR) */
typedef struct {
  __REG32  LSION          : 1;
  __REG32  LSIRDY         : 1;
  __REG32                 :22;
  __REG32  RMVF           : 1;
  __REG32  BORRSTF        : 1;
  __REG32  PINRSTF        : 1;
  __REG32  PORRSTF        : 1;
  __REG32  SFTRSTF        : 1;
  __REG32  IWDGRSTF       : 1;
  __REG32  WWDGRSTF       : 1;
  __REG32  LPWRRSTF       : 1;
} __rcc_csr_bits;

/* RCC spread spectrum clock generation register (RCC_SSCGR) */
typedef struct {
  __REG32  MODPER         :16;
  __REG32  INCSTEP        :12;
  __REG32                 : 2;
  __REG32  SPREADSEL      : 1;
  __REG32  SSCGEN         : 1;
} __rcc_sscgr_bits;

/* RCC PLLI2S configuration register (RCC_PLLI2SCFGR) */
typedef struct {
  __REG32                 : 6;
  __REG32  PLLRI2SN       : 9;
  __REG32                 :13;
  __REG32  PLLI2SR        : 3;
  __REG32                 : 1;
} __rcc_plli2scfgr_bits;

__IO_REG32_BIT(RCC_CR,            IRS_RCC_BASE + RCC_CR_S,__READ_WRITE ,__rcc_cr_bits);
__IO_REG32_BIT(RCC_PLLCFGR,       IRS_RCC_BASE + RCC_PLLCFGR_S,__READ_WRITE ,__rcc_pllcfgr_bits);
__IO_REG32_BIT(RCC_CFGR,          IRS_RCC_BASE + RCC_CFGR_S,__READ_WRITE ,__rcc_cfgr_bits);
__IO_REG32_BIT(RCC_CIR,           IRS_RCC_BASE + RCC_CIR_S,__READ_WRITE ,__rcc_cir_bits);
__IO_REG32_BIT(RCC_AHB1RSTR,      IRS_RCC_BASE + RCC_AHB1RSTR_S,__READ_WRITE ,__rcc_ahb1rstr_bits);
__IO_REG32_BIT(RCC_AHB2RSTR,      IRS_RCC_BASE + RCC_AHB2RSTR_S,__READ_WRITE ,__rcc_ahb2rstr_bits);
__IO_REG32_BIT(RCC_AHB3RSTR,      IRS_RCC_BASE + RCC_AHB3RSTR_S,__READ_WRITE ,__rcc_ahb3rstr_bits);
__IO_REG32_BIT(RCC_APB1RSTR,      IRS_RCC_BASE + RCC_APB1RSTR_S,__READ_WRITE ,__rcc_apb1rstr_bits);
__IO_REG32_BIT(RCC_APB2RSTR,      IRS_RCC_BASE + RCC_APB2RSTR_S,__READ_WRITE ,__rcc_apb2rstr_bits);
__IO_REG32_BIT(RCC_AHB1ENR,       IRS_RCC_BASE + RCC_AHB1ENR_S,__READ_WRITE ,__rcc_ahb1enr_bits);
__IO_REG32_BIT(RCC_AHB2ENR,       IRS_RCC_BASE + RCC_AHB2ENR_S,__READ_WRITE ,__rcc_ahb2enr_bits);
__IO_REG32_BIT(RCC_AHB3ENR,       IRS_RCC_BASE + RCC_AHB3ENR_S,__READ_WRITE ,__rcc_ahb3enr_bits);
__IO_REG32_BIT(RCC_APB1ENR,       IRS_RCC_BASE + RCC_APB1ENR_S,__READ_WRITE ,__rcc_apb1enr_bits);
__IO_REG32_BIT(RCC_APB2ENR,       IRS_RCC_BASE + RCC_APB2ENR_S,__READ_WRITE ,__rcc_apb2enr_bits);
__IO_REG32_BIT(RCC_AHB1LPENR,     IRS_RCC_BASE + RCC_AHB1LPENR_S,__READ_WRITE ,__rcc_ahb1lpenr_bits);
__IO_REG32_BIT(RCC_AHB2LPENR,     IRS_RCC_BASE + RCC_AHB2LPENR_S,__READ_WRITE ,__rcc_ahb2lpenr_bits);
__IO_REG32_BIT(RCC_AHB3LPENR,     IRS_RCC_BASE + RCC_AHB3LPENR_S,__READ_WRITE ,__rcc_ahb3lpenr_bits);
__IO_REG32_BIT(RCC_APB1LPENR,     IRS_RCC_BASE + RCC_APB1LPENR_S,__READ_WRITE ,__rcc_apb1lpenr_bits);
__IO_REG32_BIT(RCC_APB2LPENR,     IRS_RCC_BASE + RCC_APB2LPENR_S,__READ_WRITE ,__rcc_apb2lpenr_bits);
__IO_REG32_BIT(RCC_BDCR,          IRS_RCC_BASE + RCC_BDCR_S,__READ_WRITE ,__rcc_bdcr_bits);
__IO_REG32_BIT(RCC_CSR,           IRS_RCC_BASE + RCC_CSR_S,__READ_WRITE ,__rcc_csr_bits);
__IO_REG32_BIT(RCC_SSCGR,         IRS_RCC_BASE + RCC_SSCGR_S,__READ_WRITE ,__rcc_sscgr_bits);
__IO_REG32_BIT(RCC_PLLI2SCFGR,    IRS_RCC_BASE + RCC_PLLI2SCFGR_S,__READ_WRITE ,__rcc_plli2scfgr_bits);

#define GPIO_PORTA (*((volatile irs_u32 *) IRS_PORTA_BASE))
#define GPIO_PORTB (*((volatile irs_u32 *) IRS_PORTB_BASE))
#define GPIO_PORTC (*((volatile irs_u32 *) IRS_PORTC_BASE))
#define GPIO_PORTD (*((volatile irs_u32 *) IRS_PORTD_BASE))
#define GPIO_PORTE (*((volatile irs_u32 *) IRS_PORTE_BASE))
#define GPIO_PORTF (*((volatile irs_u32 *) IRS_PORTF_BASE))
#define GPIO_PORTG (*((volatile irs_u32 *) IRS_PORTG_BASE))
#define GPIO_PORTH (*((volatile irs_u32 *) IRS_PORTH_BASE))
#define GPIO_PORTI (*((volatile irs_u32 *) IRS_PORTI_BASE))

// GPIO

#define GPIO_MODER_S    0x00
#define GPIO_OTYPER_S   0x04
#define GPIO_OSPEEDR_S  0x08
#define GPIO_PUPDR_S    0x0C
#define GPIO_IDR_S      0x10
#define GPIO_ODR_S      0x14
#define GPIO_BSRR_S     0x18
#define GPIO_LCKR_S     0x1C
#define GPIO_AFRL_S     0x20
#define GPIO_AFRH_S     0x24

#define GPIO_WIDTH 16
#define GPIO_PUPDR_FLOAT 0
#define GPIO_PUPDR_PULL_UP 1
#define GPIO_PUPDR_PULL_DOWN 2
#define GPIO_MODER_INPUT 0
#define GPIO_MODER_OUTPUT 1
#define GPIO_MODER_ALTERNATE_FUNCTION 2
#define GPIO_MODER_ANALOG 3
#define GPIO_OTYPER_OUTPUT_PUSH_PULL 0
#define GPIO_OTYPER_OUTPUT_OPEN_DRAIN 1

/* GPIO port mode register (GPIOx_MODER) (x = A..I) */
typedef struct {
  __REG32  MODER0         : 2;
  __REG32  MODER1         : 2;
  __REG32  MODER2         : 2;
  __REG32  MODER3         : 2;
  __REG32  MODER4         : 2;
  __REG32  MODER5         : 2;
  __REG32  MODER6         : 2;
  __REG32  MODER7         : 2;
  __REG32  MODER8         : 2;
  __REG32  MODER9         : 2;
  __REG32  MODER10        : 2;
  __REG32  MODER11        : 2;
  __REG32  MODER12        : 2;
  __REG32  MODER13        : 2;
  __REG32  MODER14        : 2;
  __REG32  MODER15        : 2;
} __gpio_moder_bits;

/* GPIO port output type register (GPIOx_OTYPER) (x = A..I) */
typedef struct {
  __REG32  OT0            : 1;
  __REG32  OT1            : 1;
  __REG32  OT2            : 1;
  __REG32  OT3            : 1;
  __REG32  OT4            : 1;
  __REG32  OT5            : 1;
  __REG32  OT6            : 1;
  __REG32  OT7            : 1;
  __REG32  OT8            : 1;
  __REG32  OT9            : 1;
  __REG32  OT10           : 1;
  __REG32  OT11           : 1;
  __REG32  OT12           : 1;
  __REG32  OT13           : 1;
  __REG32  OT14           : 1;
  __REG32  OT15           : 1;
  __REG32                 :16;
} __gpio_otyper_bits;

/* GPIO port output speed register (GPIOx_OSPEEDR) (x = A..I) */
typedef struct {
  __REG32  OSPEEDR0       : 2;
  __REG32  OSPEEDR1       : 2;
  __REG32  OSPEEDR2       : 2;
  __REG32  OSPEEDR3       : 2;
  __REG32  OSPEEDR4       : 2;
  __REG32  OSPEEDR5       : 2;
  __REG32  OSPEEDR6       : 2;
  __REG32  OSPEEDR7       : 2;
  __REG32  OSPEEDR8       : 2;
  __REG32  OSPEEDR9       : 2;
  __REG32  OSPEEDR10      : 2;
  __REG32  OSPEEDR11      : 2;
  __REG32  OSPEEDR12      : 2;
  __REG32  OSPEEDR13      : 2;
  __REG32  OSPEEDR14      : 2;
  __REG32  OSPEEDR15      : 2;
} __gpio_ospeedr_bits;

/* GPIO port pull-up/pull-down register (GPIOx_PUPDR)(x = A..I) */
typedef struct {
  __REG32  PUPDR0         : 2;
  __REG32  PUPDR1         : 2;
  __REG32  PUPDR2         : 2;
  __REG32  PUPDR3         : 2;
  __REG32  PUPDR4         : 2;
  __REG32  PUPDR5         : 2;
  __REG32  PUPDR6         : 2;
  __REG32  PUPDR7         : 2;
  __REG32  PUPDR8         : 2;
  __REG32  PUPDR9         : 2;
  __REG32  PUPDR10        : 2;
  __REG32  PUPDR11        : 2;
  __REG32  PUPDR12        : 2;
  __REG32  PUPDR13        : 2;
  __REG32  PUPDR14        : 2;
  __REG32  PUPDR15        : 2;
} __gpio_pupdr_bits;

/* GPIO port input data register (GPIOx_IDR) (x = A..I) */
typedef struct {
  __REG32  IDR0           : 1;
  __REG32  IDR1           : 1;
  __REG32  IDR2           : 1;
  __REG32  IDR3           : 1;
  __REG32  IDR4           : 1;
  __REG32  IDR5           : 1;
  __REG32  IDR6           : 1;
  __REG32  IDR7           : 1;
  __REG32  IDR8           : 1;
  __REG32  IDR9           : 1;
  __REG32  IDR10          : 1;
  __REG32  IDR11          : 1;
  __REG32  IDR12          : 1;
  __REG32  IDR13          : 1;
  __REG32  IDR14          : 1;
  __REG32  IDR15          : 1;
  __REG32                 :16;
} __gpio_idr_bits;

/* GPIO port output data register (GPIOx_ODR) (x = A..I) */
typedef struct {
  __REG32  ODR0           : 1;
  __REG32  ODR1           : 1;
  __REG32  ODR2           : 1;
  __REG32  ODR3           : 1;
  __REG32  ODR4           : 1;
  __REG32  ODR5           : 1;
  __REG32  ODR6           : 1;
  __REG32  ODR7           : 1;
  __REG32  ODR8           : 1;
  __REG32  ODR9           : 1;
  __REG32  ODR10          : 1;
  __REG32  ODR11          : 1;
  __REG32  ODR12          : 1;
  __REG32  ODR13          : 1;
  __REG32  ODR14          : 1;
  __REG32  ODR15          : 1;
  __REG32                 :16;
} __gpio_odr_bits;

/* GPIO port bit set/reset register (GPIOx_BSRR) (x = A..I) */
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
} __gpio_bsrr_bits;

/* GPIO port configuration lock register (GPIOx_LCKR) (x = A..I) */
typedef struct {
  __REG32  LCK0           : 1;
  __REG32  LCK1           : 1;
  __REG32  LCK2           : 1;
  __REG32  LCK3           : 1;
  __REG32  LCK4           : 1;
  __REG32  LCK5           : 1;
  __REG32  LCK6           : 1;
  __REG32  LCK7           : 1;
  __REG32  LCK8           : 1;
  __REG32  LCK9           : 1;
  __REG32  LCK10          : 1;
  __REG32  LCK11          : 1;
  __REG32  LCK12          : 1;
  __REG32  LCK13          : 1;
  __REG32  LCK14          : 1;
  __REG32  LCK15          : 1;
  __REG32  LCKK           : 1;
  __REG32                 :15;
} __gpio_lckr_bits;

/* GPIO alternate function low register (GPIOx_AFRL) (x = A..I) */
typedef struct {
  __REG32  AFRL0          : 4;
  __REG32  AFRL1          : 4;
  __REG32  AFRL2          : 4;
  __REG32  AFRL3          : 4;
  __REG32  AFRL4          : 4;
  __REG32  AFRL5          : 4;
  __REG32  AFRL6          : 4;
  __REG32  AFRL7          : 4;
} __gpio_afrl_bits;

/* GPIO alternate function high register (GPIOx_AFRH) (x = A..I) */
typedef struct {
  __REG32  AFRH8          : 4;
  __REG32  AFRH9          : 4;
  __REG32  AFRH10         : 4;
  __REG32  AFRH11         : 4;
  __REG32  AFRH12         : 4;
  __REG32  AFRH13         : 4;
  __REG32  AFRH14         : 4;
  __REG32  AFRH15         : 4;
} __gpio_afrh_bits;

/***************************************************************************
 **
 ** GPIOA
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOA_MODER,       IRS_PORTA_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOA_OTYPER,      IRS_PORTA_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOA_OSPEEDR,     IRS_PORTA_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOA_PUPDR,       IRS_PORTA_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOA_IDR,         IRS_PORTA_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOA_ODR,         IRS_PORTA_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOA_BSRR,        IRS_PORTA_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOA_LCKR,        IRS_PORTA_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOA_AFRL,        IRS_PORTA_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOA_AFRH,        IRS_PORTA_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOB
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOB_MODER,       IRS_PORTB_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOB_OTYPER,      IRS_PORTB_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOB_OSPEEDR,     IRS_PORTB_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOB_PUPDR,       IRS_PORTB_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOB_IDR,         IRS_PORTB_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOB_ODR,         IRS_PORTB_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOB_BSRR,        IRS_PORTB_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOB_LCKR,        IRS_PORTB_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOB_AFRL,        IRS_PORTB_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOB_AFRH,        IRS_PORTB_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOC
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOC_MODER,       IRS_PORTC_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOC_OTYPER,      IRS_PORTC_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOC_OSPEEDR,     IRS_PORTC_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOC_PUPDR,       IRS_PORTC_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOC_IDR,         IRS_PORTC_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOC_ODR,         IRS_PORTC_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOC_BSRR,        IRS_PORTC_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOC_LCKR,        IRS_PORTC_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOC_AFRL,        IRS_PORTC_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOC_AFRH,        IRS_PORTC_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOD
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOD_MODER,       IRS_PORTD_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOD_OTYPER,      IRS_PORTD_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOD_OSPEEDR,     IRS_PORTD_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOD_PUPDR,       IRS_PORTD_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOD_IDR,         IRS_PORTD_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOD_ODR,         IRS_PORTD_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOD_BSRR,        IRS_PORTD_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOD_LCKR,        IRS_PORTD_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOD_AFRL,        IRS_PORTD_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOD_AFRH,        IRS_PORTD_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOE
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOE_MODER,       IRS_PORTE_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOE_OTYPER,      IRS_PORTE_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOE_OSPEEDR,     IRS_PORTE_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOE_PUPDR,       IRS_PORTE_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOE_IDR,         IRS_PORTE_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOE_ODR,         IRS_PORTE_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOE_BSRR,        IRS_PORTE_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOE_LCKR,        IRS_PORTE_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOE_AFRL,        IRS_PORTE_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOE_AFRH,        IRS_PORTE_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOF
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOF_MODER,       IRS_PORTF_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOF_OTYPER,      IRS_PORTF_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOF_OSPEEDR,     IRS_PORTF_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOF_PUPDR,       IRS_PORTF_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOF_IDR,         IRS_PORTF_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOF_ODR,         IRS_PORTF_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOF_BSRR,        IRS_PORTF_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOF_LCKR,        IRS_PORTF_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOF_AFRL,        IRS_PORTF_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOF_AFRH,        IRS_PORTF_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOG
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOG_MODER,       IRS_PORTG_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOG_OTYPER,      IRS_PORTG_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOG_OSPEEDR,     IRS_PORTG_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOG_PUPDR,       IRS_PORTG_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOG_IDR,         IRS_PORTG_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOG_ODR,         IRS_PORTG_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOG_BSRR,        IRS_PORTG_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOG_LCKR,        IRS_PORTG_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOG_AFRL,        IRS_PORTG_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOG_AFRH,        IRS_PORTG_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOH
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOH_MODER,       IRS_PORTH_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOH_OTYPER,      IRS_PORTH_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOH_OSPEEDR,     IRS_PORTH_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOH_PUPDR,       IRS_PORTH_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOH_IDR,         IRS_PORTH_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOH_ODR,         IRS_PORTH_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOH_BSRR,        IRS_PORTH_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOH_LCKR,        IRS_PORTH_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOH_AFRL,        IRS_PORTH_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOH_AFRH,        IRS_PORTH_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOI
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOI_MODER,       IRS_PORTI_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOI_OTYPER,      IRS_PORTI_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOI_OSPEEDR,     IRS_PORTI_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOI_PUPDR,       IRS_PORTI_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOI_IDR,         IRS_PORTI_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOI_ODR,         IRS_PORTI_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOI_BSRR,        IRS_PORTI_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOI_LCKR,        IRS_PORTI_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOI_AFRL,        IRS_PORTI_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOI_AFRH,        IRS_PORTI_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

// TIMER3
#define TIM_CR1_S       0x00
#define TIM_CR2_S       0x04
#define TIM_SMCR_S      0x08
#define TIM_DIER_S      0x0C
#define TIM_SR_S        0x10
#define TIM_EGR_S       0x14
#define TIM_CCMR1_S     0x18
#define TIM_CCMR2_S     0x1C
#define TIM_CCER_S      0x20
#define TIM_CNT_S       0x24
#define TIM_PSC_S       0x28
#define TIM_ARR_S       0x2C
#define TIM_RCR_S       0x30
#define TIM_CCR1_S      0x34
#define TIM_CCR2_S      0x38
#define TIM_CCR3_S      0x3C
#define TIM_CCR4_S      0x40
#define TIM_BDTR_S      0x44
#define TIM_DCR_S       0x48
#define TIM_DMAR_S      0x4C
#define TIM_OR_S        0x50

/* Control register 1 (TIM1_CR1) */
typedef struct {
  __REG32 CEN             : 1;
  __REG32 UDIS            : 1;
  __REG32 URS             : 1;
  __REG32 OPM             : 1;
  __REG32 DIR             : 1;
  __REG32 CMS             : 2;
  __REG32 ARPE            : 1;
  __REG32 CKD             : 2;
  __REG32                 :22;
} __tim1_cr1_bits;

/* Control register 2 (TIM1_CR2) */
typedef struct {
  __REG32 CCPC            : 1;
  __REG32                 : 1;
  __REG32 CCUS            : 1;
  __REG32 CCDS            : 1;
  __REG32 MMS             : 3;
  __REG32 TI1S            : 1;
  __REG32 OIS1            : 1;
  __REG32 OIS1N           : 1;
  __REG32 OIS2            : 1;
  __REG32 OIS2N           : 1;
  __REG32 OIS3            : 1;
  __REG32 OIS3N           : 1;
  __REG32 OIS4            : 1;
  __REG32                 :17;
} __tim1_cr2_bits;

/* Slave mode control register (TIM1_SMCR) */
typedef struct {
  __REG32 SMS             : 3;
  __REG32                 : 1;
  __REG32 TS              : 3;
  __REG32 MSM             : 1;
  __REG32 ETF             : 4;
  __REG32 ETPS            : 2;
  __REG32 ECE             : 1;
  __REG32 ETP             : 1;
  __REG32                 :16;
} __tim1_smcr_bits;

/* DMA/Interrupt enable register (TIM1_DIER) */
typedef struct {
  __REG32 UIE             : 1;
  __REG32 CC1IE           : 1;
  __REG32 CC2IE           : 1;
  __REG32 CC3IE           : 1;
  __REG32 CC4IE           : 1;
  __REG32 COMIE           : 1;
  __REG32 TIE             : 1;
  __REG32 BIE             : 1;
  __REG32 UDE             : 1;
  __REG32 CC1DE           : 1;
  __REG32 CC2DE           : 1;
  __REG32 CC3DE           : 1;
  __REG32 CC4DE           : 1;
  __REG32 COMDE           : 1;
  __REG32 TDE             : 1;
  __REG32                 :17;
} __tim1_dier_bits;

/* Status register (TIM1_SR) */
typedef struct {
  __REG32 UIF             : 1;
  __REG32 CC1IF           : 1;
  __REG32 CC2IF           : 1;
  __REG32 CC3IF           : 1;
  __REG32 CC4IF           : 1;
  __REG32 COMIF           : 1;
  __REG32 TIF             : 1;
  __REG32 BIF             : 1;
  __REG32                 : 1;
  __REG32 CC1OF           : 1;
  __REG32 CC2OF           : 1;
  __REG32 CC3OF           : 1;
  __REG32 CC4OF           : 1;
  __REG32                 :19;
} __tim1_sr_bits;

/* Event generation register (TIM1_EGR) */
typedef struct {
  __REG32 UG              : 1;
  __REG32 CC1G            : 1;
  __REG32 CC2G            : 1;
  __REG32 CC3G            : 1;
  __REG32 CC4G            : 1;
  __REG32 COMG            : 1;
  __REG32 TG              : 1;
  __REG32 BG              : 1;
  __REG32                 :24;
} __tim1_egr_bits;

/* Capture/compare mode register 1 (TIM1_CCMR1) */
typedef union {
  /* TIMx_CCMR1*/
  struct {
  __REG32 IC1S            : 2;
  __REG32 IC1PSC          : 2;
  __REG32 IC1F            : 4;
  __REG32 IC2S            : 2;
  __REG32 IC2PSC          : 2;
  __REG32 IC2F            : 4;
  __REG32                 :16;
  };
  /* TIMx_OCMR1*/
  struct {
  __REG32 OC1S            : 2;
  __REG32 OC1FE           : 1;
  __REG32 OC1PE           : 1;
  __REG32 OC1M            : 3;
  __REG32 OC1CE           : 1;
  __REG32 OC2S            : 2;
  __REG32 OC2FE           : 1;
  __REG32 OC2PE           : 1;
  __REG32 OC2M            : 3;
  __REG32 OC2CE           : 1;
  __REG32                 :16;
  };
} __tim1_ccmr1_bits;

/* Capture/compare mode register 2 (TIM1_CCMR2) */
typedef union {
  /* TIMx_CCMR2*/
  struct {
  __REG32 IC3S            : 2;
  __REG32 IC3PSC          : 2;
  __REG32 IC3F            : 4;
  __REG32 IC4S            : 2;
  __REG32 IC4PSC          : 2;
  __REG32 IC4F            : 4;
  __REG32                 :16;
  };
  /* TIMx_OCMR2*/
  struct {
  __REG32 OC3S            : 2;
  __REG32 OC3FE           : 1;
  __REG32 OC3PE           : 1;
  __REG32 OC3M            : 3;
  __REG32 OC3CE           : 1;
  __REG32 OC4S            : 2;
  __REG32 OC4FE           : 1;
  __REG32 OC4PE           : 1;
  __REG32 OC4M            : 3;
  __REG32 OC4CE           : 1;
  __REG32                 :16;
  };
} __tim1_ccmr2_bits;

/* Capture/compare enable register (TIM1_CCER) */
typedef struct {
  __REG32 CC1E            : 1;
  __REG32 CC1P            : 1;
  __REG32 CC1NE           : 1;
  __REG32 CC1NP           : 1;
  __REG32 CC2E            : 1;
  __REG32 CC2P            : 1;
  __REG32 CC2NE           : 1;
  __REG32 CC2NP           : 1;
  __REG32 CC3E            : 1;
  __REG32 CC3P            : 1;
  __REG32 CC3NE           : 1;
  __REG32 CC3NP           : 1;
  __REG32 CC4E            : 1;
  __REG32 CC4P            : 1;
  __REG32                 :18;
} __tim1_ccer_bits;

/* Counter (TIM1_CNT) */
typedef struct {
  __REG32 CNT             :16;
  __REG32                 :16;
} __tim1_cnt_bits;

/* Prescaler (TIM1_PSC) */
typedef struct {
  __REG32 PSC             :16;
  __REG32                 :16;
} __tim1_psc_bits;

/* Auto-reload register (TIM1_ARR) */
typedef struct {
  __REG32 ARR             :16;
  __REG32                 :16;
} __tim1_arr_bits;

/* Repetition counter register (TIM1_RCR) */
typedef struct {
  __REG32 REP             : 8;
  __REG32                 :24;
} __tim1_rcr_bits;

/* Capture/compare register (TIM1_CCR) */
typedef struct {
  __REG32 CCR             :16;
  __REG32                 :16;
} __tim1_ccr_bits;

/* Break and dead-time register (TIM1_BDTR) */
typedef struct {
  __REG32 DTG             : 8;
  __REG32 LOCK            : 2;
  __REG32 OSSI            : 1;
  __REG32 OSSR            : 1;
  __REG32 BKE             : 1;
  __REG32 BKP             : 1;
  __REG32 AOE             : 1;
  __REG32 MOE             : 1;
  __REG32                 :16;
} __tim1_bdtr_bits;

/* DMA control register (TIM1_DCR) */
typedef struct {
  __REG32 DBA             : 5;
  __REG32                 : 3;
  __REG32 DBL             : 5;
  __REG32                 :19;
} __tim1_dcr_bits;

/* DMA address for burst mode (TIM1_DMAR) */
typedef struct {
  __REG32 DMAB            :16;
  __REG32                 :16;
} __tim1_dmar_bits;

/* Control register 1 (TIMx_CR1) */
typedef struct {
  __REG32 CEN             : 1;
  __REG32 UDIS            : 1;
  __REG32 URS             : 1;
  __REG32 OPM             : 1;
  __REG32 DIR             : 1;
  __REG32 CMS             : 2;
  __REG32 ARPE            : 1;
  __REG32 CKD             : 2;
  __REG32                 :22;
} __tim_cr1_bits;

/* Control register 2 (TIMx_CR2) */
typedef struct {
  __REG32                 : 3;
  __REG32 CCDS            : 1;
  __REG32 MMS             : 3;
  __REG32 TI1S            : 1;
  __REG32                 :24;
} __tim_cr2_bits;

/* Slave mode control register (TIMx_SMCR) */
typedef struct {
  __REG32 SMS             : 3;
  __REG32                 : 1;
  __REG32 TS              : 3;
  __REG32 MSM             : 1;
  __REG32 ETF             : 4;
  __REG32 ETPS            : 2;
  __REG32 ECE             : 1;
  __REG32 ETP             : 1;
  __REG32                 :16;
} __tim_smcr_bits;

/* DMA/Interrupt enable register (TIMx_DIER) */
typedef struct {
  __REG32 UIE             : 1;
  __REG32 CC1IE           : 1;
  __REG32 CC2IE           : 1;
  __REG32 CC3IE           : 1;
  __REG32 CC4IE           : 1;
  __REG32                 : 1;
  __REG32 TIE             : 1;
  __REG32                 : 1;
  __REG32 UDE             : 1;
  __REG32 CC1DE           : 1;
  __REG32 CC2DE           : 1;
  __REG32 CC3DE           : 1;
  __REG32 CC4DE           : 1;
  __REG32                 : 1;
  __REG32 TDE             : 1;
  __REG32                 :17;
} __tim_dier_bits;

/* Status register (TIMx_SR) */
typedef struct {
  __REG32 UIF             : 1;
  __REG32 CC1IF           : 1;
  __REG32 CC2IF           : 1;
  __REG32 CC3IF           : 1;
  __REG32 CC4IF           : 1;
  __REG32                 : 1;
  __REG32 TIF             : 1;
  __REG32                 : 2;
  __REG32 CC1OF           : 1;
  __REG32 CC2OF           : 1;
  __REG32 CC3OF           : 1;
  __REG32 CC4OF           : 1;
  __REG32                 :19;
} __tim_sr_bits;

/* Event generation register (TIMx_EGR) */
typedef struct {
  __REG32 UG              : 1;
  __REG32 CC1G            : 1;
  __REG32 CC2G            : 1;
  __REG32 CC3G            : 1;
  __REG32 CC4G            : 1;
  __REG32                 : 1;
  __REG32 TG              : 1;
  __REG32                 :25;
} __tim_egr_bits;

/* Capture/compare mode register 1 (TIMx_CCMR1) */
typedef union {
  /* TIMx_CCMR1*/
  struct {
  __REG32 IC1S            : 2;
  __REG32 IC1PSC          : 2;
  __REG32 IC1F            : 4;
  __REG32 IC2S            : 2;
  __REG32 IC2PSC          : 2;
  __REG32 IC2F            : 4;
  __REG32                 :16;
  };
  /* TIMx_OCMR1*/
  struct {
  __REG32 OC1S            : 2;
  __REG32 OC1FE           : 1;
  __REG32 OC1PE           : 1;
  __REG32 OC1M            : 3;
  __REG32 OC1CE           : 1;
  __REG32 OC2S            : 2;
  __REG32 OC2FE           : 1;
  __REG32 OC2PE           : 1;
  __REG32 OC2M            : 3;
  __REG32 OC2CE           : 1;
  __REG32                 :16;
  };
} __tim_ccmr1_bits;

/* Capture/compare mode register 2 (TIMx_CCMR2) */
typedef union {
  /* TIMx_CCMR2*/
  struct {
  __REG32 IC3S            : 2;
  __REG32 IC3PSC          : 2;
  __REG32 IC3F            : 4;
  __REG32 IC4S            : 2;
  __REG32 IC4PSC          : 2;
  __REG32 IC4F            : 4;
  __REG32                 :16;
  };
  /* TIMx_OCMR2*/
  struct {
  __REG32 OC3S            : 2;
  __REG32 OC3FE           : 1;
  __REG32 OC3PE           : 1;
  __REG32 OC3M            : 3;
  __REG32 OC3CE           : 1;
  __REG32 OC4S            : 2;
  __REG32 OC4FE           : 1;
  __REG32 OC4PE           : 1;
  __REG32 OC4M            : 3;
  __REG32 OC4CE           : 1;
  __REG32                 :16;
  };
} __tim_ccmr2_bits;

/* Capture/compare enable register (TIMx_CCER) */
typedef struct {
  __REG32 CC1E            : 1;
  __REG32 CC1P            : 1;
  __REG32                 : 1;
  __REG32 CC1NP           : 1;
  __REG32 CC2E            : 1;
  __REG32 CC2P            : 1;
  __REG32                 : 1;
  __REG32 CC2NP           : 1;
  __REG32 CC3E            : 1;
  __REG32 CC3P            : 1;
  __REG32                 : 1;
  __REG32 CC3NP           : 1;
  __REG32 CC4E            : 1;
  __REG32 CC4P            : 1;
  __REG32                 : 1;
  __REG32 CC4NP           : 1;
  __REG32                 :16;
} __tim_ccer_bits;

/* Prescaler (TIMx_PSC) */
typedef struct {
  __REG32 PSC             :16;
  __REG32                 :16;
} __tim_psc_bits;

/* DMA control register (TIMx_DCR) */
typedef struct {
  __REG32 DBA             : 5;
  __REG32                 : 3;
  __REG32 DBL             : 5;
  __REG32                 :19;
} __tim_dcr_bits;

/* DMA address for burst mode (TIMx_DMAR) */
typedef struct {
  __REG32 DMAB            :16;
  __REG32                 :16;
} __tim_dmar_bits;

/* TIM2 option register (TIM2_OR) */
typedef struct {
  __REG32                 :10;
  __REG32 ITR1_RMP        : 2;
  __REG32                 :20;
} __tim2_or_bits;

/* TIM5 option register (TIM5_OR) */
typedef struct {
  __REG32                 : 6;
  __REG32 TI4_RMP         : 2;
  __REG32                 :24;
} __tim5_or_bits;

/* Control register 1 (TIM9_CR1) */
/* Control register 1 (TIM12_CR1) */
typedef struct {
  __REG32 CEN             : 1;
  __REG32 UDIS            : 1;
  __REG32 URS             : 1;
  __REG32 OPM             : 1;
  __REG32                 : 3;
  __REG32 ARPE            : 1;
  __REG32 CKD             : 2;
  __REG32                 :22;
} __tim9_cr1_bits;

/* Control register 2 (TIM9_CR2) */
/* Control register 2 (TIM12_CR2) */
typedef struct {
  __REG32                 : 4;
  __REG32 MMS             : 3;
  __REG32                 :25;
} __tim9_cr2_bits;

/* Slave mode control register (TIM9_SMCR) */
/* Slave mode control register (TIM12_SMCR) */
typedef struct {
  __REG32 SMS             : 3;
  __REG32                 : 1;
  __REG32 TS              : 3;
  __REG32 MSM             : 1;
  __REG32                 :24;
} __tim9_smcr_bits;

/* Interrupt enable register (TIM9_DIER) */
/* Interrupt enable register (TIM12_DIER) */
typedef struct {
  __REG32 UIE             : 1;
  __REG32 CC1IE           : 1;
  __REG32 CC2IE           : 1;
  __REG32                 : 3;
  __REG32 TIE             : 1;
  __REG32                 :25;
} __tim9_dier_bits;

/* Status register (TIM9_SR) */
/* Status register (TIM12_SR) */
typedef struct {
  __REG32 UIF             : 1;
  __REG32 CC1IF           : 1;
  __REG32 CC2IF           : 1;
  __REG32                 : 3;
  __REG32 TIF             : 1;
  __REG32                 : 2;
  __REG32 CC1OF           : 1;
  __REG32 CC2OF           : 1;
  __REG32                 :21;
} __tim9_sr_bits;

/* Event generation register (TIM9_EGR) */
/* Event generation register (TIM12_EGR) */
typedef struct {
  __REG32 UG              : 1;
  __REG32 CC1G            : 1;
  __REG32 CC2G            : 1;
  __REG32                 : 3;
  __REG32 TG              : 1;
  __REG32                 :25;
} __tim9_egr_bits;

/* Capture/compare mode register 1 (TIM9_CCMR1) */
/* Capture/compare mode register 1 (TIM12_CCMR1) */
typedef union {
  /* TIM9_CCMR1*/
  /* TIM12_CCMR1*/
  struct {
  __REG32 IC1S            : 2;
  __REG32 IC1PSC          : 2;
  __REG32 IC1F            : 4;
  __REG32 IC2S            : 2;
  __REG32 IC2PSC          : 2;
  __REG32 IC2F            : 4;
  __REG32                 :16;
  };
  /* TIM9_OCMR1*/
  /* TIM12_OCMR1*/
  struct {
  __REG32 OC1S            : 2;
  __REG32 OC1FE           : 1;
  __REG32 OC1PE           : 1;
  __REG32 OC1M            : 3;
  __REG32 OC1CE           : 1;
  __REG32 OC2S            : 2;
  __REG32 OC2FE           : 1;
  __REG32 OC2PE           : 1;
  __REG32 OC2M            : 3;
  __REG32 OC2CE           : 1;
  __REG32                 :16;
  };
} __tim9_ccmr1_bits;

/* Capture/compare enable register (TIM9_CCER) */
/* Capture/compare enable register (TIM12_CCER) */
typedef struct {
  __REG32 CC1E            : 1;
  __REG32 CC1P            : 1;
  __REG32                 : 1;
  __REG32 CC1NP           : 1;
  __REG32 CC2E            : 1;
  __REG32 CC2P            : 1;
  __REG32                 : 1;
  __REG32 CC2NP           : 1;
  __REG32                 :24;
} __tim9_ccer_bits;

/* Counter (TIM9_CNT) */
/* Counter (TIM12_CNT) */
typedef struct {
  __REG32 CNT             :16;
  __REG32                 :16;
} __tim9_cnt_bits;

/* Prescaler (TIM9_PSC) */
/* Prescaler (TIM12_PSC) */
typedef struct {
  __REG32 PSC             :16;
  __REG32                 :16;
} __tim9_psc_bits;

/* Auto-reload register (TIM9_ARR) */
/* Auto-reload register (TIM12_ARR) */
typedef struct {
  __REG32 ARR             :16;
  __REG32                 :16;
} __tim9_arr_bits;

/* Capture/compare register (TIM9_CCRx) */
/* Capture/compare register (TIM12_CCR) */
typedef struct {
  __REG32 CCR             :16;
  __REG32                 :16;
} __tim9_ccr_bits;

/* Control register 1 (TIM10-14_CR1) */
typedef struct {
  __REG32 CEN             : 1;
  __REG32 UDIS            : 1;
  __REG32 URS             : 1;
  __REG32                 : 4;
  __REG32 ARPE            : 1;
  __REG32 CKD             : 2;
  __REG32                 :22;
} __tim10_cr1_bits;
/* Interrupt enable register (TIM10-14_DIER) */
typedef struct {
  __REG32 UIE             : 1;
  __REG32 CC1IE           : 1;
  __REG32                 :30;
} __tim10_dier_bits;

/* Status register (TIM10-14_SR) */
typedef struct {
  __REG32 UIF             : 1;
  __REG32 CC1IF           : 1;
  __REG32                 : 7;
  __REG32 CC1OF           : 1;
  __REG32                 :22;
} __tim10_sr_bits;

/* Event generation register (TIM10-14_EGR) */
typedef struct {
  __REG32 UG              : 1;
  __REG32 CC1G            : 1;
  __REG32                 :30;
} __tim10_egr_bits;

/* Capture/compare mode register 1 (TIM10-14_CCMR1) */
typedef union {
  /* TIM10_CCMR1*/
  /* TIM11_CCMR1*/
  /* TIM13_CCMR1*/
  /* TIM14_CCMR1*/
  struct {
  __REG32 IC1S            : 2;
  __REG32 IC1PSC          : 2;
  __REG32 IC1F            : 4;
  __REG32                 :24;
  };
  /* TIM10_OCMR1*/
  /* TIM11_OCMR1*/
  /* TIM13_OCMR1*/
  /* TIM14_OCMR1*/
  struct {
  __REG32 OC1S            : 2;
  __REG32                 : 1;
  __REG32 OC1PE           : 1;
  __REG32 OC1M            : 3;
  __REG32                 :25;
  };
} __tim10_ccmr1_bits;

/* Capture/compare enable register (TIM10-14_CCER) */
typedef struct {
  __REG32 CC1E            : 1;
  __REG32 CC1P            : 1;
  __REG32                 : 1;
  __REG32 CC1NP           : 1;
  __REG32                 :28;
} __tim10_ccer_bits;

/* Counter (TIM10-14_CNT) */
typedef struct {
  __REG32 CNT             :16;
  __REG32                 :16;
} __tim10_cnt_bits;

/* Prescaler (TIM10-14_PSC) */
typedef struct {
  __REG32 PSC             :16;
  __REG32                 :16;
} __tim10_psc_bits;

/* Auto-reload register (TIM10-14_ARR) */
typedef struct {
  __REG32 ARR             :16;
  __REG32                 :16;
} __tim10_arr_bits;

/* Capture/compare register (TIM10-14_CCRx) */
typedef struct {
  __REG32 CCR             :16;
  __REG32                 :16;
} __tim10_ccr_bits;

/* TIM11 option register (TIM11_OR) */
typedef struct {
  __REG32 TI1_RMP         : 2;
  __REG32                 :30;
} __tim11_or_bits;

/* Control register 1 (TIM6_CR1) */
/* Control register 1 (TIM7_CR1) */
typedef struct {
  __REG32 CEN             : 1;
  __REG32 UDIS            : 1;
  __REG32 URS             : 1;
  __REG32 OPM             : 1;
  __REG32                 : 3;
  __REG32 ARPE            : 1;
  __REG32                 : 2;
  __REG32                 :22;
} __tim6_cr1_bits;

/* Control register 2 (TIM6_CR2) */
/* Control register 2 (TIM7_CR2) */
typedef struct {
  __REG32                 : 4;
  __REG32 MMS             : 3;
  __REG32                 :25;
} __tim6_cr2_bits;

/* DMA/Interrupt enable register (TIM6_DIER) */
/* DMA/Interrupt enable register (TIM7_DIER) */
typedef struct {
  __REG32 UIE             : 1;
  __REG32                 : 7;
  __REG32 UDE             : 1;
  __REG32                 :23;
} __tim6_dier_bits;

/* Status register (TIM6_SR) */
/* Status register (TIM7_SR) */
typedef struct {
  __REG32 UIF             : 1;
  __REG32                 :31;
} __tim6_sr_bits;

/* Event generation register (TIM6_EGR) */
/* Event generation register (TIM7_EGR) */
typedef struct {
  __REG32 UG              : 1;
  __REG32                 :31;
} __tim6_egr_bits;

/* Counter (TIM6_CNT) */
/* Counter (TIM7_CNT) */
typedef struct {
  __REG32 CNT             :16;
  __REG32                 :16;
} __tim6_cnt_bits;

/* Prescaler (TIM6_PSC) */
/* Prescaler (TIM7_PSC) */
typedef struct {
  __REG32 PSC             :16;
  __REG32                 :16;
} __tim6_psc_bits;

/* Auto-reload register (TIM6_ARR) */
/* Auto-reload register (TIM7_ARR) */
typedef struct {
  __REG32 ARR             :16;
  __REG32                 :16;
} __tim6_arr_bits;

/***************************************************************************
 **
 ** TIM1
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM1_CR1,          IRS_TIM1_PWM1_BASE + TIM_CR1_S,__READ_WRITE ,__tim1_cr1_bits);
__IO_REG32_BIT(TIM1_CR2,          IRS_TIM1_PWM1_BASE + TIM_CR2_S,__READ_WRITE ,__tim1_cr2_bits);
__IO_REG32_BIT(TIM1_SMCR,         IRS_TIM1_PWM1_BASE + TIM_SMCR_S,__READ_WRITE ,__tim1_smcr_bits);
__IO_REG32_BIT(TIM1_DIER,         IRS_TIM1_PWM1_BASE + TIM_DIER_S,__READ_WRITE ,__tim1_dier_bits);
__IO_REG32_BIT(TIM1_SR,           IRS_TIM1_PWM1_BASE + TIM_SR_S,__READ_WRITE ,__tim1_sr_bits);
__IO_REG32_BIT(TIM1_EGR,          IRS_TIM1_PWM1_BASE + TIM_EGR_S,__READ_WRITE ,__tim1_egr_bits);
__IO_REG32_BIT(TIM1_CCMR1,        IRS_TIM1_PWM1_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim1_ccmr1_bits);
#define TIM1_OCMR1      TIM1_CCMR1
#define TIM1_OCMR1_BIT  TIM1_CCMR1_BIT
__IO_REG32_BIT(TIM1_CCMR2,        IRS_TIM1_PWM1_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim1_ccmr2_bits);
#define TIM1_OCMR2      TIM1_CCMR2
#define TIM1_OCMR2_BIT  TIM1_CCMR2_BIT
__IO_REG32_BIT(TIM1_CCER,         IRS_TIM1_PWM1_BASE + TIM_CCER_S,__READ_WRITE ,__tim1_ccer_bits);
__IO_REG32_BIT(TIM1_CNT,          IRS_TIM1_PWM1_BASE + TIM_CNT_S,__READ_WRITE ,__tim1_cnt_bits);
__IO_REG32_BIT(TIM1_PSC,          IRS_TIM1_PWM1_BASE + TIM_PSC_S,__READ_WRITE ,__tim1_psc_bits);
__IO_REG32_BIT(TIM1_ARR,          IRS_TIM1_PWM1_BASE + TIM_ARR_S,__READ_WRITE ,__tim1_arr_bits);
__IO_REG32_BIT(TIM1_RCR,          IRS_TIM1_PWM1_BASE + TIM_RCR_S,__READ_WRITE ,__tim1_rcr_bits);
__IO_REG32_BIT(TIM1_CCR1,         IRS_TIM1_PWM1_BASE + TIM_CCR1_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_CCR2,         IRS_TIM1_PWM1_BASE + TIM_CCR2_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_CCR3,         IRS_TIM1_PWM1_BASE + TIM_CCR3_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_CCR4,         IRS_TIM1_PWM1_BASE + TIM_CCR4_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_BDTR,         IRS_TIM1_PWM1_BASE + TIM_BDTR_S,__READ_WRITE ,__tim1_bdtr_bits);
__IO_REG32_BIT(TIM1_DCR,          IRS_TIM1_PWM1_BASE + TIM_DCR_S,__READ_WRITE ,__tim1_dcr_bits);
__IO_REG32_BIT(TIM1_DMAR,         IRS_TIM1_PWM1_BASE + TIM_DMAR_S,__READ_WRITE ,__tim1_dmar_bits);

/***************************************************************************
 **
 ** TIM8
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM8_CR1,          IRS_TIM8_PWM2_BASE + TIM_CR1_S,__READ_WRITE ,__tim1_cr1_bits);
__IO_REG32_BIT(TIM8_CR2,          IRS_TIM8_PWM2_BASE + TIM_CR2_S,__READ_WRITE ,__tim1_cr2_bits);
__IO_REG32_BIT(TIM8_SMCR,         IRS_TIM8_PWM2_BASE + TIM_SMCR_S,__READ_WRITE ,__tim1_smcr_bits);
__IO_REG32_BIT(TIM8_DIER,         IRS_TIM8_PWM2_BASE + TIM_DIER_S,__READ_WRITE ,__tim1_dier_bits);
__IO_REG32_BIT(TIM8_SR,           IRS_TIM8_PWM2_BASE + TIM_SR_S,__READ_WRITE ,__tim1_sr_bits);
__IO_REG32_BIT(TIM8_EGR,          IRS_TIM8_PWM2_BASE + TIM_EGR_S,__READ_WRITE ,__tim1_egr_bits);
__IO_REG32_BIT(TIM8_CCMR1,        IRS_TIM8_PWM2_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim1_ccmr1_bits);
#define TIM1_OCMR8      TIM8_CCMR1
#define TIM1_OCMR8_BIT  TIM8_CCMR1_BIT
__IO_REG32_BIT(TIM8_CCMR2,        IRS_TIM8_PWM2_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim1_ccmr2_bits);
#define TIM8_OCMR2      TIM8_CCMR2
#define TIM8_OCMR2_BIT  TIM8_CCMR2_BIT
__IO_REG32_BIT(TIM8_CCER,         IRS_TIM8_PWM2_BASE + TIM_CCER_S,__READ_WRITE ,__tim1_ccer_bits);
__IO_REG32_BIT(TIM8_CNT,          IRS_TIM8_PWM2_BASE + TIM_CNT_S,__READ_WRITE ,__tim1_cnt_bits);
__IO_REG32_BIT(TIM8_PSC,          IRS_TIM8_PWM2_BASE + TIM_PSC_S,__READ_WRITE ,__tim1_psc_bits);
__IO_REG32_BIT(TIM8_ARR,          IRS_TIM8_PWM2_BASE + TIM_ARR_S,__READ_WRITE ,__tim1_arr_bits);
__IO_REG32_BIT(TIM8_RCR,          IRS_TIM8_PWM2_BASE + TIM_RCR_S,__READ_WRITE ,__tim1_rcr_bits);
__IO_REG32_BIT(TIM8_CCR1,         IRS_TIM8_PWM2_BASE + TIM_CCR1_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_CCR2,         IRS_TIM8_PWM2_BASE + TIM_CCR2_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_CCR3,         IRS_TIM8_PWM2_BASE + TIM_CCR3_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_CCR4,         IRS_TIM8_PWM2_BASE + TIM_CCR4_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_BDTR,         IRS_TIM8_PWM2_BASE + TIM_BDTR_S,__READ_WRITE ,__tim1_bdtr_bits);
__IO_REG32_BIT(TIM8_DCR,          IRS_TIM8_PWM2_BASE + TIM_DCR_S,__READ_WRITE ,__tim1_dcr_bits);
__IO_REG32_BIT(TIM8_DMAR,         IRS_TIM8_PWM2_BASE + TIM_DMAR_S,__READ_WRITE ,__tim1_dmar_bits);

/***************************************************************************
 **
 ** TIM3
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM3_CR1,          IRS_TIM3_BASE + TIM_CR1_S,__READ_WRITE ,__tim_cr1_bits);
__IO_REG32_BIT(TIM3_CR2,          IRS_TIM3_BASE + TIM_CR2_S,__READ_WRITE ,__tim_cr2_bits);
__IO_REG32_BIT(TIM3_SMCR,         IRS_TIM3_BASE + TIM_SMCR_S,__READ_WRITE ,__tim_smcr_bits);
__IO_REG32_BIT(TIM3_DIER,         IRS_TIM3_BASE + TIM_DIER_S,__READ_WRITE ,__tim_dier_bits);
__IO_REG32_BIT(TIM3_SR,           IRS_TIM3_BASE + TIM_SR_S,__READ_WRITE ,__tim_sr_bits);
__IO_REG32_BIT(TIM3_EGR,          IRS_TIM3_BASE + TIM_EGR_S,__READ_WRITE ,__tim_egr_bits);
__IO_REG32_BIT(TIM3_CCMR1,        IRS_TIM3_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim_ccmr1_bits);
#define TIM3_OCMR1      TIM3_CCMR1
#define TIM3_OCMR1_BIT  TIM3_CCMR1_BIT
__IO_REG32_BIT(TIM3_CCMR2,        IRS_TIM3_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim_ccmr2_bits);
#define TIM3_OCMR2      TIM3_CCMR2
#define TIM3_OCMR2_BIT  TIM3_CCMR2_BIT
__IO_REG32_BIT(TIM3_CCER,         IRS_TIM3_BASE + TIM_CCER_S,__READ_WRITE ,__tim_ccer_bits);
__IO_REG32(    TIM3_CNT,          IRS_TIM3_BASE + TIM_CNT_S,__READ_WRITE );
__IO_REG32_BIT(TIM3_PSC,          IRS_TIM3_BASE + TIM_PSC_S,__READ_WRITE ,__tim_psc_bits);
__IO_REG32(    TIM3_ARR,          IRS_TIM3_BASE + TIM_ARR_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR1,         IRS_TIM3_BASE + TIM_CCR1_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR2,         IRS_TIM3_BASE + TIM_CCR2_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR3,         IRS_TIM3_BASE + TIM_CCR3_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR4,         IRS_TIM3_BASE + TIM_CCR4_S,__READ_WRITE );
__IO_REG32_BIT(TIM3_DCR,          IRS_TIM3_BASE + TIM_DCR_S,__READ_WRITE ,__tim_dcr_bits);
__IO_REG32_BIT(TIM3_DMAR,         IRS_TIM3_BASE + TIM_DMAR_S,__READ_WRITE ,__tim_dmar_bits);

/***************************************************************************
 **
 ** TIM4
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM4_CR1,          IRS_TIM4_BASE + TIM_CR1_S,__READ_WRITE ,__tim_cr1_bits);
__IO_REG32_BIT(TIM4_CR2,          IRS_TIM4_BASE + TIM_CR2_S,__READ_WRITE ,__tim_cr2_bits);
__IO_REG32_BIT(TIM4_SMCR,         IRS_TIM4_BASE + TIM_SMCR_S,__READ_WRITE ,__tim_smcr_bits);
__IO_REG32_BIT(TIM4_DIER,         IRS_TIM4_BASE + TIM_DIER_S,__READ_WRITE ,__tim_dier_bits);
__IO_REG32_BIT(TIM4_SR,           IRS_TIM4_BASE + TIM_SR_S,__READ_WRITE ,__tim_sr_bits);
__IO_REG32_BIT(TIM4_EGR,          IRS_TIM4_BASE + TIM_EGR_S,__READ_WRITE ,__tim_egr_bits);
__IO_REG32_BIT(TIM4_CCMR1,        IRS_TIM4_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim_ccmr1_bits);
#define TIM4_OCMR1      TIM4_CCMR1
#define TIM4_OCMR1_BIT  TIM4_CCMR1_BIT
__IO_REG32_BIT(TIM4_CCMR2,        IRS_TIM4_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim_ccmr2_bits);
#define TIM4_OCMR2      TIM4_CCMR2
#define TIM4_OCMR2_BIT  TIM4_CCMR2_BIT
__IO_REG32_BIT(TIM4_CCER,         IRS_TIM4_BASE + TIM_CCER_S,__READ_WRITE ,__tim_ccer_bits);
__IO_REG32(    TIM4_CNT,          IRS_TIM4_BASE + TIM_CNT_S,__READ_WRITE );
__IO_REG32_BIT(TIM4_PSC,          IRS_TIM4_BASE + TIM_PSC_S,__READ_WRITE ,__tim_psc_bits);
__IO_REG32(    TIM4_ARR,          IRS_TIM4_BASE + TIM_ARR_S,__READ_WRITE );
__IO_REG32(    TIM4_CCR1,         IRS_TIM4_BASE + TIM_CCR1_S,__READ_WRITE );
__IO_REG32(    TIM4_CCR2,         IRS_TIM4_BASE + TIM_CCR2_S,__READ_WRITE );
__IO_REG32_BIT(TIM4_DCR,          IRS_TIM4_BASE + TIM_DCR_S,__READ_WRITE ,__tim_dcr_bits);
__IO_REG32_BIT(TIM4_DMAR,         IRS_TIM4_BASE + TIM_DMAR_S,__READ_WRITE ,__tim_dmar_bits);

/***************************************************************************
 **
 ** TIM9
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM9_CR1,          IRS_TIM9_BASE + TIM_CR1_S,__READ_WRITE ,__tim9_cr1_bits);
__IO_REG32_BIT(TIM9_CR2,          IRS_TIM9_BASE + TIM_CR2_S,__READ_WRITE ,__tim9_cr2_bits);
__IO_REG32_BIT(TIM9_SMCR,         IRS_TIM9_BASE + TIM_SMCR_S,__READ_WRITE ,__tim9_smcr_bits);
__IO_REG32_BIT(TIM9_DIER,         IRS_TIM9_BASE + TIM_DIER_S,__READ_WRITE ,__tim9_dier_bits);
__IO_REG32_BIT(TIM9_SR,           IRS_TIM9_BASE + TIM_SR_S,__READ_WRITE ,__tim9_sr_bits);
__IO_REG32_BIT(TIM9_EGR,          IRS_TIM9_BASE + TIM_EGR_S,__READ_WRITE ,__tim9_egr_bits);
__IO_REG32_BIT(TIM9_CCMR1,        IRS_TIM9_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim9_ccmr1_bits);
#define TIM9_OCMR1      TIM9_CCMR1
#define TIM9_OCMR1_BIT  TIM9_CCMR1_BIT
__IO_REG32_BIT(TIM9_CCER,         IRS_TIM9_BASE + TIM_CCER_S,__READ_WRITE ,__tim9_ccer_bits);
__IO_REG32_BIT(TIM9_CNT,          IRS_TIM9_BASE + TIM_CNT_S,__READ_WRITE ,__tim9_cnt_bits);
__IO_REG32_BIT(TIM9_PSC,          IRS_TIM9_BASE + TIM_PSC_S,__READ_WRITE ,__tim9_psc_bits);
__IO_REG32_BIT(TIM9_ARR,          IRS_TIM9_BASE + TIM_ARR_S,__READ_WRITE ,__tim9_arr_bits);
__IO_REG32_BIT(TIM9_CCR1,         IRS_TIM9_BASE + TIM_CCR1_S,__READ_WRITE ,__tim9_ccr_bits);
__IO_REG32_BIT(TIM9_CCR2,         IRS_TIM9_BASE + TIM_CCR2_S,__READ_WRITE ,__tim9_ccr_bits);

/***************************************************************************
 **
 ** TIM12
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM12_CR1,         IRS_TIM12_BASE + TIM_CR1_S,__READ_WRITE ,__tim9_cr1_bits);
__IO_REG32_BIT(TIM12_CR2,         IRS_TIM12_BASE + TIM_CR2_S,__READ_WRITE ,__tim9_cr2_bits);
__IO_REG32_BIT(TIM12_SMCR,        IRS_TIM12_BASE + TIM_SMCR_S,__READ_WRITE ,__tim9_smcr_bits);
__IO_REG32_BIT(TIM12_DIER,        IRS_TIM12_BASE + TIM_DIER_S,__READ_WRITE ,__tim9_dier_bits);
__IO_REG32_BIT(TIM12_SR,          IRS_TIM12_BASE + TIM_SR_S,__READ_WRITE ,__tim9_sr_bits);
__IO_REG32_BIT(TIM12_EGR,         IRS_TIM12_BASE + TIM_EGR_S,__READ_WRITE ,__tim9_egr_bits);
__IO_REG32_BIT(TIM12_CCMR1,       IRS_TIM12_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim9_ccmr1_bits);
#define TIM12_OCMR1      TIM12_CCMR1
#define TIM12_OCMR1_BIT  TIM12_CCMR1_BIT
__IO_REG32_BIT(TIM12_CCER,        IRS_TIM12_BASE + TIM_CCER_S,__READ_WRITE ,__tim9_ccer_bits);
__IO_REG32_BIT(TIM12_CNT,         IRS_TIM12_BASE + TIM_CNT_S,__READ_WRITE ,__tim9_cnt_bits);
__IO_REG32_BIT(TIM12_PSC,         IRS_TIM12_BASE + TIM_PSC_S,__READ_WRITE ,__tim9_psc_bits);
__IO_REG32_BIT(TIM12_ARR,         IRS_TIM12_BASE + TIM_ARR_S,__READ_WRITE ,__tim9_arr_bits);
__IO_REG32_BIT(TIM12_CCR1,        IRS_TIM12_BASE + TIM_CCR1_S,__READ_WRITE ,__tim9_ccr_bits);
__IO_REG32_BIT(TIM12_CCR2,        IRS_TIM12_BASE + TIM_CCR2_S,__READ_WRITE ,__tim9_ccr_bits);

/***************************************************************************
 **
 ** TIM10
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM10_CR1,         IRS_TIM10_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM10_DIER,        IRS_TIM10_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM10_SR,          IRS_TIM10_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM10_EGR,         IRS_TIM10_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM10_CCMR1,       IRS_TIM10_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM10_OCMR1      TIM10_CCMR1
#define TIM10_OCMR1_BIT  TIM10_CCMR1_BIT
__IO_REG32_BIT(TIM10_CCER,        IRS_TIM10_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM10_CNT,         IRS_TIM10_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM10_PSC,         IRS_TIM10_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM10_ARR,         IRS_TIM10_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM10_CCR1,        IRS_TIM10_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);

/***************************************************************************
 **
 ** TIM11
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM11_CR1,         IRS_TIM11_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM11_DIER,        IRS_TIM11_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM11_SR,          IRS_TIM11_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM11_EGR,         IRS_TIM11_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM11_CCMR1,       IRS_TIM11_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM11_OCMR1      TIM11_CCMR1
#define TIM11_OCMR1_BIT  TIM11_CCMR1_BIT
__IO_REG32_BIT(TIM11_CCER,        IRS_TIM11_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM11_CNT,         IRS_TIM11_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM11_PSC,         IRS_TIM11_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM11_ARR,         IRS_TIM11_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM11_CCR1,        IRS_TIM11_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);
__IO_REG32_BIT(TIM11_OR,          IRS_TIM11_BASE + TIM_OR_S,__READ_WRITE ,__tim11_or_bits);

/***************************************************************************
 **
 ** TIM13
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM13_CR1,         IRS_TIM13_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM13_DIER,        IRS_TIM13_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM13_SR,          IRS_TIM13_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM13_EGR,         IRS_TIM13_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM13_CCMR1,       IRS_TIM13_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM13_OCMR1      TIM13_CCMR1
#define TIM13_OCMR1_BIT  TIM13_CCMR1_BIT
__IO_REG32_BIT(TIM13_CCER,        IRS_TIM13_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM13_CNT,         IRS_TIM13_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM13_PSC,         IRS_TIM13_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM13_ARR,         IRS_TIM13_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM13_CCR1,        IRS_TIM13_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);

/***************************************************************************
 **
 ** TIM14
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM14_CR1,         IRS_TIM14_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM14_DIER,        IRS_TIM14_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM14_SR,          IRS_TIM14_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM14_EGR,         IRS_TIM14_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM14_CCMR1,       IRS_TIM14_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM14_OCMR1      TIM14_CCMR1
#define TIM14_OCMR1_BIT  TIM14_CCMR1_BIT
__IO_REG32_BIT(TIM14_CCER,        IRS_TIM14_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM14_CNT,         IRS_TIM14_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM14_PSC,         IRS_TIM14_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM14_ARR,         IRS_TIM14_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM14_CCR1,        IRS_TIM14_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);

struct tim_regs_t
{
  IRS_IO_REG32_BIT(TIM_CR1, __READ_WRITE ,__tim1_cr1_bits);
  IRS_IO_REG32_BIT(TIM_CR2, __READ_WRITE ,__tim1_cr2_bits);
  IRS_IO_REG32_BIT(TIM_SMCR, __READ_WRITE ,__tim1_smcr_bits);
  IRS_IO_REG32_BIT(TIM_DIER, __READ_WRITE ,__tim1_dier_bits);
  IRS_IO_REG32_BIT(TIM_SR, __READ_WRITE ,__tim1_sr_bits);
  IRS_IO_REG32_BIT(TIM_EGR, __READ_WRITE ,__tim1_egr_bits);
  IRS_IO_REG32_BIT(TIM_CCMR1, __READ_WRITE ,__tim1_ccmr1_bits);
  IRS_IO_REG32_BIT(TIM_CCMR2, __READ_WRITE ,__tim1_ccmr2_bits);
  IRS_IO_REG32_BIT(TIM_CCER, __READ_WRITE ,__tim1_ccer_bits);
  IRS_IO_REG32_BIT(TIM_CNT, __READ_WRITE ,__tim1_cnt_bits);
  IRS_IO_REG32_BIT(TIM_PSC, __READ_WRITE ,__tim1_psc_bits);
  IRS_IO_REG32_BIT(TIM_ARR, __READ_WRITE ,__tim1_arr_bits);
  IRS_IO_REG32_BIT(TIM_RCR, __READ_WRITE ,__tim1_rcr_bits);
  IRS_IO_REG32_BIT(TIM_CCR1, __READ_WRITE ,__tim1_ccr_bits);
  IRS_IO_REG32_BIT(TIM_CCR2, __READ_WRITE ,__tim1_ccr_bits);
  IRS_IO_REG32_BIT(TIM_CCR3, __READ_WRITE ,__tim1_ccr_bits);
  IRS_IO_REG32_BIT(TIM_CCR4, __READ_WRITE ,__tim1_ccr_bits);
  IRS_IO_REG32_BIT(TIM_BDTR, __READ_WRITE ,__tim1_bdtr_bits);
  IRS_IO_REG32_BIT(TIM_DCR, __READ_WRITE ,__tim1_dcr_bits);
  IRS_IO_REG32_BIT(TIM_DMAR, __READ_WRITE ,__tim1_dmar_bits);
};


#define FLASH_ACR_S        0x0

/* Flash Access Control Register (FLASH_ACR) */
typedef struct {
  __REG32  LATENCY        : 3;
  __REG32                 : 5;
  __REG32  PRFTEN         : 1;
  __REG32  ICEN           : 1;
  __REG32  DCEN           : 1;
  __REG32  ICRST          : 1;
  __REG32  DCRST          : 1;
  __REG32                 :19;
} __flash_acr_bits;

__IO_REG32_BIT(FLASH_ACR, IRS_FLASH_INTERFACE_BASE + FLASH_ACR_S,__READ_WRITE ,__flash_acr_bits);

//  NVIC
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
  __REG32  SETENA_EXTI3   : 1;
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
  __REG32  SETENA_TIM1_UP_TIM10       : 1;
  __REG32  SETENA26       : 1;
  __REG32  SETENA27       : 1;
  __REG32  SETENA28       : 1;
  __REG32  SETENA_TIM3    : 1;
  __REG32  SETENA_TIM4    : 1;
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
  __REG32  SETENA42       : 1;
  __REG32  SETENA_TIM8_BRK_TIM12: 1;
  __REG32  SETENA_TIM8_UP_TIM13 : 1;
  __REG32  SETENA45       : 1;
  __REG32  SETENA46       : 1;
  __REG32  SETENA47       : 1;
  __REG32  SETENA48       : 1;
  __REG32  SETENA49       : 1;
  __REG32  SETENA50       : 1;
  __REG32  SETENA51       : 1;
  __REG32  SETENA52       : 1;
  __REG32  SETENA53       : 1;
  __REG32  SETENA54       : 1;
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

// SPI

#define SPI_CR1_S 0x00
#define SPI_CR2_S 0x04
#define SPI_SR_S 0x08
#define SPI_DR_S 0x0C
#define SPI_CRCPR_S 0x10
#define SPI_RXCRCR_S 0x14
#define SPI_TXCRCR_S 0x18
#define SPI_I2SCFGR_S 0x1C
#define SPI_I2SPR_S 0x20

/* SPI control register 1 (SPI_CR1)*/
typedef struct {
  __REG32 CPHA            : 1;
  __REG32 CPOL            : 1;
  __REG32 MSTR            : 1;
  __REG32 BR              : 3;
  __REG32 SPE             : 1;
  __REG32 LSBFIRST        : 1;
  __REG32 SSI             : 1;
  __REG32 SSM             : 1;
  __REG32 RXONLY          : 1;
  __REG32 DFF             : 1;
  __REG32 CRCNEXT         : 1;
  __REG32 CRCEN           : 1;
  __REG32 BIDIOE          : 1;
  __REG32 BIDIMODE        : 1;
  __REG32                 :16;
} __spi_cr1_bits;

/* SPI control register 2 (SPI_CR2)*/
typedef struct {
  __REG32 RXDMAEN         : 1;
  __REG32 TXDMAEN         : 1;
  __REG32 SSOE            : 1;
  __REG32                 : 1;
  __REG32 FRF             : 1;
  __REG32 ERRIE           : 1;
  __REG32 RXNEIE          : 1;
  __REG32 TXEIE           : 1;
  __REG32                 :24;
} __spi_cr2_bits;

/* SPI status register (SPI_SR)*/
typedef struct {
  __REG32 RXNE            : 1;
  __REG32 TXE             : 1;
  __REG32 CHSIDE          : 1;
  __REG32 UDR             : 1;
  __REG32 CRCERR          : 1;
  __REG32 MODF            : 1;
  __REG32 OVR             : 1;
  __REG32 BSY             : 1;
  __REG32 TIFRFE          : 1;
  __REG32                 :23;
} __spi_sr_bits;

/* SPI data register (SPI_DR) */
typedef struct {
  __REG32 DR              :16;
  __REG32                 :16;
} __spi_dr_bits;

/* SPI CRC polynomial register (SPI_CRCPR) */
typedef struct {
  __REG32 CRCPOLY         :16;
  __REG32                 :16;
} __spi_crcpr_bits;

/* SPI Rx CRC register (SPI_RXCRCR) */
typedef struct {
  __REG32 RxCRC           :16;
  __REG32                 :16;
} __spi_rxcrcr_bits;

/* SPI Tx CRC register (SPI_TXCRCR) */
typedef struct {
  __REG32 TxCRC           :16;
  __REG32                 :16;
} __spi_txcrcr_bits;

/* SPI_I2S configuration register (SPI_I2SCFGR) */
typedef struct {
  __REG32 CHLEN           : 1;
  __REG32 DATLEN          : 2;
  __REG32 CKPOL           : 1;
  __REG32 I2SSTD          : 2;
  __REG32                 : 1;
  __REG32 PCMSYNC         : 1;
  __REG32 I2SCFG          : 2;
  __REG32 I2SE            : 1;
  __REG32 I2SMOD          : 1;
  __REG32                 :20;
} __spi_i2scfgr_bits;

/* SPI_I2S Prescaler register (SPI_I2SPR) */
typedef struct {
  __REG32 I2SDIV          : 8;
  __REG32 ODD             : 1;
  __REG32 MCKOE           : 1;
  __REG32                 :22;
} __spi_i2spr_bits;

/***************************************************************************
 **
 ** SPI1
 **
 ***************************************************************************/
__IO_REG32_BIT(SPI1_CR1,          IRS_SPI1_BASE + SPI_CR1_S,__READ_WRITE ,__spi_cr1_bits);
__IO_REG32_BIT(SPI1_CR2,          IRS_SPI1_BASE + SPI_CR2_S,__READ_WRITE ,__spi_cr2_bits);
__IO_REG32_BIT(SPI1_SR,           IRS_SPI1_BASE + SPI_SR_S,__READ_WRITE ,__spi_sr_bits);
__IO_REG32_BIT(SPI1_DR,           IRS_SPI1_BASE + SPI_DR_S,__READ_WRITE ,__spi_dr_bits);
__IO_REG32_BIT(SPI1_CRCPR,        IRS_SPI1_BASE + SPI_CRCPR_S,__READ_WRITE ,__spi_crcpr_bits);
__IO_REG32_BIT(SPI1_RXCRCR,       IRS_SPI1_BASE + SPI_RXCRCR_S,__READ       ,__spi_rxcrcr_bits);
__IO_REG32_BIT(SPI1_TXCRCR,       IRS_SPI1_BASE + SPI_TXCRCR_S,__READ       ,__spi_txcrcr_bits);

/***************************************************************************
 **
 ** SPI2
 **
 ***************************************************************************/
__IO_REG32_BIT(SPI2_CR1,          IRS_SPI2_I2S2_BASE + SPI_CR1_S,__READ_WRITE ,__spi_cr1_bits);
__IO_REG32_BIT(SPI2_CR2,          IRS_SPI2_I2S2_BASE + SPI_CR2_S,__READ_WRITE ,__spi_cr2_bits);
__IO_REG32_BIT(SPI2_SR,           IRS_SPI2_I2S2_BASE + SPI_SR_S,__READ_WRITE ,__spi_sr_bits);
__IO_REG32_BIT(SPI2_DR,           IRS_SPI2_I2S2_BASE + SPI_DR_S,__READ_WRITE ,__spi_dr_bits);
__IO_REG32_BIT(SPI2_CRCPR,        IRS_SPI2_I2S2_BASE + SPI_CRCPR_S,__READ_WRITE ,__spi_crcpr_bits);
__IO_REG32_BIT(SPI2_RXCRCR,       IRS_SPI2_I2S2_BASE + SPI_RXCRCR_S,__READ       ,__spi_rxcrcr_bits);
__IO_REG32_BIT(SPI2_TXCRCR,       IRS_SPI2_I2S2_BASE + SPI_TXCRCR_S,__READ       ,__spi_txcrcr_bits);
__IO_REG32_BIT(SPI2_I2SCFGR,      IRS_SPI2_I2S2_BASE + SPI_I2SCFGR_S,__READ_WRITE ,__spi_i2scfgr_bits);
__IO_REG32_BIT(SPI2_I2SPR,        IRS_SPI2_I2S2_BASE + SPI_I2SPR_S,__READ_WRITE ,__spi_i2spr_bits);

/***************************************************************************
 **
 ** SPI3
 **
 ***************************************************************************/
__IO_REG32_BIT(SPI3_CR1,          IRS_SPI3_I2S3_BASE + SPI_CR1_S,__READ_WRITE ,__spi_cr1_bits);
__IO_REG32_BIT(SPI3_CR2,          IRS_SPI3_I2S3_BASE + SPI_CR2_S,__READ_WRITE ,__spi_cr2_bits);
__IO_REG32_BIT(SPI3_SR,           IRS_SPI3_I2S3_BASE + SPI_SR_S,__READ_WRITE ,__spi_sr_bits);
__IO_REG32_BIT(SPI3_DR,           IRS_SPI3_I2S3_BASE + SPI_DR_S,__READ_WRITE ,__spi_dr_bits);
__IO_REG32_BIT(SPI3_CRCPR,        IRS_SPI3_I2S3_BASE + SPI_CRCPR_S,__READ_WRITE ,__spi_crcpr_bits);
__IO_REG32_BIT(SPI3_RXCRCR,       IRS_SPI3_I2S3_BASE + SPI_RXCRCR_S,__READ       ,__spi_rxcrcr_bits);
__IO_REG32_BIT(SPI3_TXCRCR,       IRS_SPI3_I2S3_BASE + SPI_TXCRCR_S,__READ       ,__spi_txcrcr_bits);
__IO_REG32_BIT(SPI3_I2SCFGR,      IRS_SPI3_I2S3_BASE + SPI_TXCRCR_S,__READ_WRITE ,__spi_i2scfgr_bits);
__IO_REG32_BIT(SPI3_I2SPR,        IRS_SPI3_I2S3_BASE + SPI_I2SPR_S,__READ_WRITE ,__spi_i2spr_bits);

struct spi_regs_t
{
  IRS_IO_REG32_BIT(SPI_CR1, __READ_WRITE ,__spi_cr1_bits);
  IRS_IO_REG32_BIT(SPI_CR2, __READ_WRITE ,__spi_cr2_bits);
  IRS_IO_REG32_BIT(SPI_SR, __READ_WRITE ,__spi_sr_bits);
  IRS_IO_REG32_BIT(SPI_DR, __READ_WRITE ,__spi_dr_bits);
  IRS_IO_REG32_BIT(SPI_CRCPR, __READ_WRITE ,__spi_crcpr_bits);
  IRS_IO_REG32_BIT(SPI_RXCRCR, __READ       ,__spi_rxcrcr_bits);
  IRS_IO_REG32_BIT(SPI_TXCRCR, __READ       ,__spi_txcrcr_bits);
  IRS_IO_REG32_BIT(SPI_I2SCFGR, __READ_WRITE ,__spi_i2scfgr_bits);
  IRS_IO_REG32_BIT(SPI_I2SPR, __READ_WRITE ,__spi_i2spr_bits);
};

// USART

#define USART_SR_S 0x00
#define USART_DR_S 0x04
#define USART_BRR_S 0x08
#define USART_CR1_S 0x0C
#define USART_CR2_S 0x10
#define USART_CR3_S 0x14
#define USART_GTPR_S 0x18

/* Status register (USART_SR) */
typedef struct {
  __REG32 PE              : 1;
  __REG32 FE              : 1;
  __REG32 NE              : 1;
  __REG32 ORE             : 1;
  __REG32 IDLE            : 1;
  __REG32 RXNE            : 1;
  __REG32 TC              : 1;
  __REG32 TXE             : 1;
  __REG32 LBD             : 1;
  __REG32 CTS             : 1;
  __REG32                 :22;
} __usart_sr_bits;

/* Data register (USART_DR) */
typedef struct {
  __REG32 DR              : 9;
  __REG32                 :23;
} __usart_dr_bits;

/* Baud rate register (USART_BRR) */
typedef struct {
  __REG32 DIV_Fraction    : 4;
  __REG32 DIV_Mantissa    :12;
  __REG32                 :16;
} __usart_brr_bits;

/* Control register 1 (USART_CR1) */
typedef struct {
  __REG32 SBK             : 1;
  __REG32 RWU             : 1;
  __REG32 RE              : 1;
  __REG32 TE              : 1;
  __REG32 IDLEIE          : 1;
  __REG32 RXNEIE          : 1;
  __REG32 TCIE            : 1;
  __REG32 TXEIE           : 1;
  __REG32 PEIE            : 1;
  __REG32 PS              : 1;
  __REG32 PCE             : 1;
  __REG32 WAKE            : 1;
  __REG32 M               : 1;
  __REG32 UE              : 1;
  __REG32 OVER8           : 1;
  __REG32                 :18;
} __usart_cr1_bits;

/* Control register 2 (USART_CR2) */
typedef struct {
  __REG32 ADD             : 4;
  __REG32                 : 1;
  __REG32 LBDL            : 1;
  __REG32 LBDIE           : 1;
  __REG32                 : 1;
  __REG32 LBCL            : 1;
  __REG32 CPHA            : 1;
  __REG32 CPOL            : 1;
  __REG32 CLKEN           : 1;
  __REG32 STOP            : 2;
  __REG32 LINEN           : 1;
  __REG32                 :17;
} __usart_cr2_bits;

/* Control register 3 (USART_CR3) */
typedef struct {
  __REG32 EIE             : 1;
  __REG32 IREN            : 1;
  __REG32 IRLP            : 1;
  __REG32 HDSEL           : 1;
  __REG32 NACK            : 1;
  __REG32 SCEN            : 1;
  __REG32 DMAR            : 1;
  __REG32 DMAT            : 1;
  __REG32 RTSE            : 1;
  __REG32 CTSE            : 1;
  __REG32 CTSIE           : 1;
  __REG32                 :21;
} __usart_cr3_bits;

/* Guard time and prescaler register (USART_GTPR) */
typedef struct {
  __REG32 PSC             : 8;
  __REG32 GT              : 8;
  __REG32                 :16;
} __usart_gtpr_bits;

/* Status register (UART_SR) */
typedef struct {
  __REG32 PE              : 1;
  __REG32 FE              : 1;
  __REG32 NE              : 1;
  __REG32 ORE             : 1;
  __REG32 IDLE            : 1;
  __REG32 RXNE            : 1;
  __REG32 TC              : 1;
  __REG32 TXE             : 1;
  __REG32 LBD             : 1;
  __REG32                 :23;
} __uart_sr_bits;

/* Data register (UART_DR) */
typedef struct {
  __REG32 DR              : 9;
  __REG32                 :23;
} __uart_dr_bits;

/* Baud rate register (UART_BRR) */
typedef struct {
  __REG32 DIV_Fraction    : 4;
  __REG32 DIV_Mantissa    :12;
  __REG32                 :16;
} __uart_brr_bits;

/* Control register 1 (UART_CR1) */
typedef struct {
  __REG32 SBK             : 1;
  __REG32 RWU             : 1;
  __REG32 RE              : 1;
  __REG32 TE              : 1;
  __REG32 IDLEIE          : 1;
  __REG32 RXNEIE          : 1;
  __REG32 TCIE            : 1;
  __REG32 TXEIE           : 1;
  __REG32 PEIE            : 1;
  __REG32 PS              : 1;
  __REG32 PCE             : 1;
  __REG32 WAKE            : 1;
  __REG32 M               : 1;
  __REG32 UE              : 1;
  __REG32                 :18;
} __uart_cr1_bits;

/* Control register 2 (UART_CR2) */
typedef struct {
  __REG32 ADD             : 4;
  __REG32                 : 1;
  __REG32 LBDL            : 1;
  __REG32 LBDIE           : 1;
  __REG32                 : 5;
  __REG32 STOP            : 2;
  __REG32 LINEN           : 1;
  __REG32                 :17;
} __uart_cr2_bits;

/* Control register 3 (UART4_CR3) */
typedef struct {
  __REG32 EIE             : 1;
  __REG32 IREN            : 1;
  __REG32 IRLP            : 1;
  __REG32 HDSEL           : 1;
  __REG32                 : 2;
  __REG32 DMAR            : 1;
  __REG32 DMAT            : 1;
  __REG32                 :24;
} __uart4_cr3_bits;

/* Control register 3 (UART5_CR3) */
typedef struct {
  __REG32 EIE             : 1;
  __REG32 IREN            : 1;
  __REG32 IRLP            : 1;
  __REG32 HDSEL           : 1;
  __REG32                 : 2;
  __REG32                 : 1;
  __REG32                 : 1;
  __REG32                 :24;
} __uart5_cr3_bits;

/***************************************************************************
 **
 ** USART1
 **
 ***************************************************************************/

__IO_REG32_BIT(USART1_SR, IRS_USART1_BASE + USART_SR_S,__READ_WRITE ,__usart_sr_bits);
__IO_REG32_BIT(USART1_DR, IRS_USART1_BASE + USART_DR_S,__READ_WRITE ,__usart_dr_bits);
__IO_REG32_BIT(USART1_BRR, IRS_USART1_BASE + USART_BRR_S,__READ_WRITE ,__usart_brr_bits);
__IO_REG32_BIT(USART1_CR1, IRS_USART1_BASE + USART_CR1_S,__READ_WRITE ,__usart_cr1_bits);
__IO_REG32_BIT(USART1_CR2, IRS_USART1_BASE + USART_CR2_S,__READ_WRITE ,__usart_cr2_bits);
__IO_REG32_BIT(USART1_CR3, IRS_USART1_BASE + USART_CR3_S,__READ_WRITE ,__usart_cr3_bits);
__IO_REG32_BIT(USART1_GTPR, IRS_USART1_BASE + USART_GTPR_S,__READ_WRITE ,__usart_gtpr_bits);

struct usart_regs_t
{
  IRS_IO_REG32_BIT(USART_SR, __READ_WRITE, __usart_sr_bits);
  /*union
  {
    unsigned long USART_SR;
    __usart_sr_bits USART_SR_bit;
  };*/
  IRS_IO_REG32_BIT(USART_DR, __READ_WRITE, __usart_dr_bits);
  IRS_IO_REG32_BIT(USART_BRR, __READ_WRITE ,__usart_brr_bits);
  IRS_IO_REG32_BIT(USART_CR1, __READ_WRITE ,__usart_cr1_bits);
  IRS_IO_REG32_BIT(USART_CR2, __READ_WRITE ,__usart_cr2_bits);
  IRS_IO_REG32_BIT(USART_CR3, __READ_WRITE ,__usart_cr3_bits);
  IRS_IO_REG32_BIT(USART_GTPR, __READ_WRITE ,__usart_gtpr_bits);
};

/* ADC status register (ADC_SR) */
typedef struct {
  __REG32 AWD             : 1;
  __REG32 EOC             : 1;
  __REG32 JEOC            : 1;
  __REG32 JSTRT           : 1;
  __REG32 STRT            : 1;
  __REG32 OVR             : 1;
  __REG32                 :26;
} __adc_sr_bits;

/* ADC control register 1 (ADC_CR1) */
typedef struct {
  __REG32 AWDCH           : 5;
  __REG32 EOCIE           : 1;
  __REG32 AWDIE           : 1;
  __REG32 JEOCIE          : 1;
  __REG32 SCAN            : 1;
  __REG32 AWDSGL          : 1;
  __REG32 JAUTO           : 1;
  __REG32 DISCEN          : 1;
  __REG32 JDISCEN         : 1;
  __REG32 DISCNUM         : 3;
  __REG32                 : 6;
  __REG32 JAWDEN          : 1;
  __REG32 AWDEN           : 1;
  __REG32 RES             : 2;
  __REG32 OVRIE           : 1;
  __REG32                 : 5;
} __adc_cr1_bits;

/* ADC control register 2 (ADC_CR2) */
typedef struct {
  __REG32 ADON            : 1;
  __REG32 CONT            : 1;
  __REG32                 : 6;
  __REG32 DMA             : 1;
  __REG32 DDS             : 1;
  __REG32 EOCS            : 1;
  __REG32 ALIGN           : 1;
  __REG32                 : 4;
  __REG32 JEXTSEL         : 4;
  __REG32 JEXTEN          : 2;
  __REG32 JSWSTART        : 1;
  __REG32                 : 1;
  __REG32 EXTSEL          : 4;
  __REG32 EXTEN           : 2;
  __REG32 SWSTART         : 1;
  __REG32                 : 1;
} __adc_cr2_bits;

/* ADC sample time register 1 (ADC_SMPR1) */
typedef struct {
  __REG32 SMP10           : 3;
  __REG32 SMP11           : 3;
  __REG32 SMP12           : 3;
  __REG32 SMP13           : 3;
  __REG32 SMP14           : 3;
  __REG32 SMP15           : 3;
  __REG32 SMP16           : 3;
  __REG32 SMP17           : 3;
  __REG32 SMP18           : 3;
  __REG32                 : 5;
} __adc_smpr1_bits;

/* ADC sample time register 2 (ADC_SMPR2) */
typedef struct {
  __REG32 SMP0            : 3;
  __REG32 SMP1            : 3;
  __REG32 SMP2            : 3;
  __REG32 SMP3            : 3;
  __REG32 SMP4            : 3;
  __REG32 SMP5            : 3;
  __REG32 SMP6            : 3;
  __REG32 SMP7            : 3;
  __REG32 SMP8            : 3;
  __REG32 SMP9            : 3;
  __REG32                 : 2;
} __adc_smpr2_bits;

/* ADC injected channel data offset register x (ADC_JOFRx)(x=1..4) */
typedef struct {
  __REG32 JOFFSET         :12;
  __REG32                 :20;
} __adc_jofr_bits;

/* ADC watchdog high threshold register (ADC_HTR) */
typedef struct {
  __REG32 HT              :12;
  __REG32                 :20;
} __adc_htr_bits;

/* ADC watchdog low threshold register (ADC_LTR) */
typedef struct {
  __REG32 LT              :12;
  __REG32                 :20;
} __adc_ltr_bits;

/* ADC regular sequence register 1 (ADC_SQR1) */
typedef struct {
  __REG32 SQ13            : 5;
  __REG32 SQ14            : 5;
  __REG32 SQ15            : 5;
  __REG32 SQ16            : 5;
  __REG32 L               : 4;
  __REG32                 : 8;
} __adc_sqr1_bits;

/* ADC regular sequence register 2 (ADC_SQR2) */
typedef struct {
  __REG32 SQ7             : 5;
  __REG32 SQ8             : 5;
  __REG32 SQ9             : 5;
  __REG32 SQ10            : 5;
  __REG32 SQ11            : 5;
  __REG32 SQ12            : 5;
  __REG32                 : 2;
} __adc_sqr2_bits;

/* ADC regular sequence register 3 (ADC_SQR3) */
typedef struct {
  __REG32 SQ1             : 5;
  __REG32 SQ2             : 5;
  __REG32 SQ3             : 5;
  __REG32 SQ4             : 5;
  __REG32 SQ5             : 5;
  __REG32 SQ6             : 5;
  __REG32                 : 2;
} __adc_sqr3_bits;

/* ADC injected sequence register (ADC_JSQR) */
typedef struct {
  __REG32 JSQ1            : 5;
  __REG32 JSQ2            : 5;
  __REG32 JSQ3            : 5;
  __REG32 JSQ4            : 5;
  __REG32 JL              : 2;
  __REG32                 :10;
} __adc_jsqr_bits;

/* ADC injected data register x (ADC_JDRx) (x= 1..4) */
typedef struct {
  __REG32 JDATA           :16;
  __REG32                 :16;
} __adc_jdr_bits;

/* ADC regular data register (ADC_DR) */
typedef struct {
  __REG32 DATA            :16;
  __REG32 ADC2DATA        :16;
} __adc_dr_bits;

/* ADC Common status register (ADC_CSR) */
typedef struct {
  __REG32 AWD1            : 1;
  __REG32 EOC1            : 1;
  __REG32 JEOC1           : 1;
  __REG32 JSTRT1          : 1;
  __REG32 STRT1           : 1;
  __REG32 OVR1            : 1;
  __REG32                 : 2;
  __REG32 AWD2            : 1;
  __REG32 EOC2            : 1;
  __REG32 JEOC2           : 1;
  __REG32 JSTRT2          : 1;
  __REG32 STRT2           : 1;
  __REG32 OVR2            : 1;
  __REG32                 : 2;
  __REG32 AWD3            : 1;
  __REG32 EOC3            : 1;
  __REG32 JEOC3           : 1;
  __REG32 JSTRT3          : 1;
  __REG32 STRT3           : 1;
  __REG32 OVR3            : 1;
  __REG32                 :10;
} __adc_csr_bits;

/* ADC common control register (ADC_CCR) */
typedef struct {
  __REG32 MULTI           : 5;
  __REG32                 : 3;
  __REG32 DELAY           : 4;
  __REG32                 : 1;
  __REG32 DDS             : 1;
  __REG32 DMA             : 2;
  __REG32 ADCPRE          : 2;
  __REG32                 : 4;
  __REG32 VBATE           : 1;
  __REG32 TSVREFE         : 1;
  __REG32                 : 8;
} __adc_ccr_bits;

/* ADC common regular data register for dual and triple modes (ADC_CDR) */
typedef struct {
  __REG32 DATA1           :16;
  __REG32 DATA2           :16;
} __adc_cdr_bits;


/***************************************************************************
 **
 ** ADC
 **
 ***************************************************************************/
__IO_REG32_BIT(ADC_CSR,           0x40012300,__READ       ,__adc_csr_bits);
__IO_REG32_BIT(ADC_CCR,           0x40012304,__READ_WRITE ,__adc_ccr_bits);
__IO_REG32_BIT(ADC_CDR,           0x40012308,__READ       ,__adc_cdr_bits);

#define ADC1_SR_S 0X00
#define ADC1_CR1_S 0X04
#define ADC1_CR2_S 0X08
#define ADC1_SMPR1_S 0X0C
#define ADC1_SMPR2_S 0X10
#define ADC1_JOFR1_S 0X14
#define ADC1_JOFR2_S 0X18
#define ADC1_JOFR3_S 0X1C
#define ADC1_JOFR4_S 0X20
#define ADC1_HTR_S 0X24
#define ADC1_LTR_S 0X28
#define ADC1_SQR1_S 0X2C
#define ADC1_SQR2_S 0X30
#define ADC1_SQR3_S 0X34
#define ADC1_JSQR_S 0X38
#define ADC1_JDR1_S 0X3C
#define ADC1_JDR2_S 0X40
#define ADC1_JDR3_S 0X44
#define ADC1_JDR4_S 0X48
#define ADC1_DR_S 0X4C

/***************************************************************************
 **
 ** ADC1
 **
 ***************************************************************************/
__IO_REG32_BIT(ADC1_SR,           IRS_ADC1_BASE + ADC1_SR_S,__READ_WRITE ,__adc_sr_bits);
__IO_REG32_BIT(ADC1_CR1,          IRS_ADC1_BASE + ADC1_CR1_S,__READ_WRITE ,__adc_cr1_bits);
__IO_REG32_BIT(ADC1_CR2,          IRS_ADC1_BASE + ADC1_CR2_S,__READ_WRITE ,__adc_cr2_bits);
__IO_REG32_BIT(ADC1_SMPR1,        IRS_ADC1_BASE + ADC1_SMPR1_S,__READ_WRITE ,__adc_smpr1_bits);
__IO_REG32_BIT(ADC1_SMPR2,        IRS_ADC1_BASE + ADC1_SMPR2_S,__READ_WRITE ,__adc_smpr2_bits);
__IO_REG32_BIT(ADC1_JOFR1,        IRS_ADC1_BASE + ADC1_JOFR1_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_JOFR2,        IRS_ADC1_BASE + ADC1_JOFR2_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_JOFR3,        IRS_ADC1_BASE + ADC1_JOFR3_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_JOFR4,        IRS_ADC1_BASE + ADC1_JOFR4_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_HTR,          IRS_ADC1_BASE + ADC1_HTR_S,__READ_WRITE ,__adc_htr_bits);
__IO_REG32_BIT(ADC1_LTR,          IRS_ADC1_BASE + ADC1_LTR_S,__READ_WRITE ,__adc_ltr_bits);
__IO_REG32_BIT(ADC1_SQR1,         IRS_ADC1_BASE + ADC1_SQR1_S,__READ_WRITE ,__adc_sqr1_bits);
__IO_REG32_BIT(ADC1_SQR2,         IRS_ADC1_BASE + ADC1_SQR2_S,__READ_WRITE ,__adc_sqr2_bits);
__IO_REG32_BIT(ADC1_SQR3,         IRS_ADC1_BASE + ADC1_SQR3_S,__READ_WRITE ,__adc_sqr3_bits);
__IO_REG32_BIT(ADC1_JSQR,         IRS_ADC1_BASE + ADC1_JSQR_S,__READ_WRITE ,__adc_jsqr_bits);
__IO_REG32_BIT(ADC1_JDR1,         IRS_ADC1_BASE + ADC1_JDR1_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_JDR2,         IRS_ADC1_BASE + ADC1_JDR2_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_JDR3,         IRS_ADC1_BASE + ADC1_JDR3_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_JDR4,         IRS_ADC1_BASE + ADC1_JDR4_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_DR,           IRS_ADC1_BASE + ADC1_DR_S,__READ       ,__adc_dr_bits);

/***************************************************************************
 **
 ** ADC2
 **
 ***************************************************************************/
__IO_REG32_BIT(ADC2_SR,           IRS_ADC2_BASE + ADC1_SR_S,__READ_WRITE ,__adc_sr_bits);
__IO_REG32_BIT(ADC2_CR1,          IRS_ADC2_BASE + ADC1_CR1_S,__READ_WRITE ,__adc_cr1_bits);
__IO_REG32_BIT(ADC2_CR2,          IRS_ADC2_BASE + ADC1_CR2_S,__READ_WRITE ,__adc_cr2_bits);
__IO_REG32_BIT(ADC2_SMPR1,        IRS_ADC2_BASE + ADC1_SMPR1_S,__READ_WRITE ,__adc_smpr1_bits);
__IO_REG32_BIT(ADC2_SMPR2,        IRS_ADC2_BASE + ADC1_SMPR2_S,__READ_WRITE ,__adc_smpr2_bits);
__IO_REG32_BIT(ADC2_JOFR1,        IRS_ADC2_BASE + ADC1_JOFR1_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_JOFR2,        IRS_ADC2_BASE + ADC1_JOFR2_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_JOFR3,        IRS_ADC2_BASE + ADC1_JOFR3_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_JOFR4,        IRS_ADC2_BASE + ADC1_JOFR4_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_HTR,          IRS_ADC2_BASE + ADC1_HTR_S,__READ_WRITE ,__adc_htr_bits);
__IO_REG32_BIT(ADC2_LTR,          IRS_ADC2_BASE + ADC1_LTR_S,__READ_WRITE ,__adc_ltr_bits);
__IO_REG32_BIT(ADC2_SQR1,         IRS_ADC2_BASE + ADC1_SQR1_S,__READ_WRITE ,__adc_sqr1_bits);
__IO_REG32_BIT(ADC2_SQR2,         IRS_ADC2_BASE + ADC1_SQR2_S,__READ_WRITE ,__adc_sqr2_bits);
__IO_REG32_BIT(ADC2_SQR3,         IRS_ADC2_BASE + ADC1_SQR3_S,__READ_WRITE ,__adc_sqr3_bits);
__IO_REG32_BIT(ADC2_JSQR,         IRS_ADC2_BASE + ADC1_JSQR_S,__READ_WRITE ,__adc_jsqr_bits);
__IO_REG32_BIT(ADC2_JDR1,         IRS_ADC2_BASE + ADC1_JDR1_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_JDR2,         IRS_ADC2_BASE + ADC1_JDR2_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_JDR3,         IRS_ADC2_BASE + ADC1_JDR3_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_JDR4,         IRS_ADC2_BASE + ADC1_JDR4_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_DR,           IRS_ADC2_BASE + ADC1_DR_S,__READ       ,__adc_dr_bits);

/***************************************************************************
 **
 ** ADC3
 **
 ***************************************************************************/
__IO_REG32_BIT(ADC3_SR,           IRS_ADC3_BASE + ADC1_SR_S,__READ_WRITE ,__adc_sr_bits);
__IO_REG32_BIT(ADC3_CR1,          IRS_ADC3_BASE + ADC1_CR1_S,__READ_WRITE ,__adc_cr1_bits);
__IO_REG32_BIT(ADC3_CR2,          IRS_ADC3_BASE + ADC1_CR2_S,__READ_WRITE ,__adc_cr2_bits);
__IO_REG32_BIT(ADC3_SMPR1,        IRS_ADC3_BASE + ADC1_SMPR1_S,__READ_WRITE ,__adc_smpr1_bits);
__IO_REG32_BIT(ADC3_SMPR2,        IRS_ADC3_BASE + ADC1_SMPR2_S,__READ_WRITE ,__adc_smpr2_bits);
__IO_REG32_BIT(ADC3_JOFR1,        IRS_ADC3_BASE + ADC1_JOFR1_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_JOFR2,        IRS_ADC3_BASE + ADC1_JOFR2_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_JOFR3,        IRS_ADC3_BASE + ADC1_JOFR3_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_JOFR4,        IRS_ADC3_BASE + ADC1_JOFR4_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_HTR,          IRS_ADC3_BASE + ADC1_HTR_S,__READ_WRITE ,__adc_htr_bits);
__IO_REG32_BIT(ADC3_LTR,          IRS_ADC3_BASE + ADC1_LTR_S,__READ_WRITE ,__adc_ltr_bits);
__IO_REG32_BIT(ADC3_SQR1,         IRS_ADC3_BASE + ADC1_SQR1_S,__READ_WRITE ,__adc_sqr1_bits);
__IO_REG32_BIT(ADC3_SQR2,         IRS_ADC3_BASE + ADC1_SQR2_S,__READ_WRITE ,__adc_sqr2_bits);
__IO_REG32_BIT(ADC3_SQR3,         IRS_ADC3_BASE + ADC1_JDR3_S,__READ_WRITE ,__adc_sqr3_bits);
__IO_REG32_BIT(ADC3_JSQR,         IRS_ADC3_BASE + ADC1_JSQR_S,__READ_WRITE ,__adc_jsqr_bits);
__IO_REG32_BIT(ADC3_JDR1,         IRS_ADC3_BASE + ADC1_JDR1_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_JDR2,         IRS_ADC3_BASE + ADC1_JDR2_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_JDR3,         IRS_ADC3_BASE + ADC1_JDR3_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_JDR4,         IRS_ADC3_BASE + ADC1_JDR4_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_DR,           IRS_ADC3_BASE + ADC1_DR_S,__READ       ,__adc_dr_bits);

struct adc_regs_t
{
  IRS_IO_REG32_BIT(ADC_SR, __READ_WRITE ,__adc_sr_bits);
  IRS_IO_REG32_BIT(ADC_CR1, __READ_WRITE ,__adc_cr1_bits);
  IRS_IO_REG32_BIT(ADC_CR2, __READ_WRITE ,__adc_cr2_bits);
  IRS_IO_REG32_BIT(ADC_SMPR1, __READ_WRITE ,__adc_smpr1_bits);
  IRS_IO_REG32_BIT(ADC_SMPR2, __READ_WRITE ,__adc_smpr2_bits);
  IRS_IO_REG32_BIT(ADC_JOFR1, __READ_WRITE ,__adc_jofr_bits);
  IRS_IO_REG32_BIT(ADC_JOFR2, __READ_WRITE ,__adc_jofr_bits);
  IRS_IO_REG32_BIT(ADC_JOFR3, __READ_WRITE ,__adc_jofr_bits);
  IRS_IO_REG32_BIT(ADC_JOFR4, __READ_WRITE ,__adc_jofr_bits);
  IRS_IO_REG32_BIT(ADC_HTR, __READ_WRITE ,__adc_htr_bits);
  IRS_IO_REG32_BIT(ADC_LTR, __READ_WRITE ,__adc_ltr_bits);
  IRS_IO_REG32_BIT(ADC_SQR1, __READ_WRITE ,__adc_sqr1_bits);
  IRS_IO_REG32_BIT(ADC_SQR2, __READ_WRITE ,__adc_sqr2_bits);
  IRS_IO_REG32_BIT(ADC_SQR3, __READ_WRITE ,__adc_sqr3_bits);
  IRS_IO_REG32_BIT(ADC_JSQR, __READ_WRITE ,__adc_jsqr_bits);
  IRS_IO_REG32_BIT(ADC_JDR1, __READ       ,__adc_jdr_bits);
  IRS_IO_REG32_BIT(ADC_JDR2, __READ       ,__adc_jdr_bits);
  IRS_IO_REG32_BIT(ADC_JDR3, __READ       ,__adc_jdr_bits);
  IRS_IO_REG32_BIT(ADC_JDR4, __READ       ,__adc_jdr_bits);
  IRS_IO_REG32_BIT(ADC_DR, __READ       ,__adc_dr_bits);
};

/* DMA low interrupt status register (DMA_LISR) */
typedef struct {
  __REG32  FEIF0          : 1;
  __REG32                 : 1;
  __REG32  DMEIF0         : 1;
  __REG32  TEIF0          : 1;
  __REG32  HTIF0          : 1;
  __REG32  TCIF0          : 1;
  __REG32  FEIF1          : 1;
  __REG32                 : 1;
  __REG32  DMEIF1         : 1;
  __REG32  TEIF1          : 1;
  __REG32  HTIF1          : 1;
  __REG32  TCIF1          : 1;
  __REG32                 : 4;
  __REG32  FEIF2          : 1;
  __REG32                 : 1;
  __REG32  DMEIF2         : 1;
  __REG32  TEIF2          : 1;
  __REG32  HTIF2          : 1;
  __REG32  TCIF2          : 1;
  __REG32  FEIF3          : 1;
  __REG32                 : 1;
  __REG32  DMEIF3         : 1;
  __REG32  TEIF3          : 1;
  __REG32  HTIF3          : 1;
  __REG32  TCIF3          : 1;
  __REG32                 : 4;
} __dma_lisr_bits;

/* DMA high interrupt status register (DMA_HISR) */
typedef struct {
  __REG32  FEIF0          : 1;
  __REG32                 : 1;
  __REG32  DMEIF0         : 1;
  __REG32  TEIF0          : 1;
  __REG32  HTIF0          : 1;
  __REG32  TCIF0          : 1;
  __REG32  FEIF1          : 1;
  __REG32                 : 1;
  __REG32  DMEIF1         : 1;
  __REG32  TEIF1          : 1;
  __REG32  HTIF1          : 1;
  __REG32  TCIF1          : 1;
  __REG32                 : 4;
  __REG32  FEIF2          : 1;
  __REG32                 : 1;
  __REG32  DMEIF2         : 1;
  __REG32  TEIF2          : 1;
  __REG32  HTIF2          : 1;
  __REG32  TCIF2          : 1;
  __REG32  FEIF3          : 1;
  __REG32                 : 1;
  __REG32  DMEIF3         : 1;
  __REG32  TEIF3          : 1;
  __REG32  HTIF3          : 1;
  __REG32  TCIF3          : 1;
  __REG32                 : 4;
} __dma_hisr_bits;

/* DMA low interrupt flag clear register (DMA_LIFCR) */
typedef struct {
  __REG32  CFEIF0         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF0        : 1;
  __REG32  CTEIF0         : 1;
  __REG32  CHTIF0         : 1;
  __REG32  CTCIF0         : 1;
  __REG32  CFEIF1         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF1        : 1;
  __REG32  CTEIF1         : 1;
  __REG32  CHTIF1         : 1;
  __REG32  CTCIF1         : 1;
  __REG32                 : 4;
  __REG32  CFEIF2         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF2        : 1;
  __REG32  CTEIF2         : 1;
  __REG32  CHTIF2         : 1;
  __REG32  CTCIF2         : 1;
  __REG32  CFEIF3         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF3        : 1;
  __REG32  CTEIF3         : 1;
  __REG32  CHTIF3         : 1;
  __REG32  CTCIF3         : 1;
  __REG32                 : 4;
} __dma_lifcr_bits;

/* DMA high interrupt flag clear register (DMA_HIFCR) */
typedef struct {
  __REG32  CFEIF0         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF0        : 1;
  __REG32  CTEIF0         : 1;
  __REG32  CHTIF0         : 1;
  __REG32  CTCIF0         : 1;
  __REG32  CFEIF1         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF1        : 1;
  __REG32  CTEIF1         : 1;
  __REG32  CHTIF1         : 1;
  __REG32  CTCIF1         : 1;
  __REG32                 : 4;
  __REG32  CFEIF2         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF2        : 1;
  __REG32  CTEIF2         : 1;
  __REG32  CHTIF2         : 1;
  __REG32  CTCIF2         : 1;
  __REG32  CFEIF3         : 1;
  __REG32                 : 1;
  __REG32  CDMEIF3        : 1;
  __REG32  CTEIF3         : 1;
  __REG32  CHTIF3         : 1;
  __REG32  CTCIF3         : 1;
  __REG32                 : 4;
} __dma_hifcr_bits;

/* DMA stream x configuration register (DMA_SxCR) (x = 0..7) */
typedef struct {
  __REG32  EN             : 1;
  __REG32  DMEIE          : 1;
  __REG32  TEIE           : 1;
  __REG32  HTIE           : 1;
  __REG32  TCIE           : 1;
  __REG32  PFCTRL         : 1;
  __REG32  DIR            : 2;
  __REG32  CIRC           : 1;
  __REG32  PINC           : 1;
  __REG32  MINC           : 1;
  __REG32  PSIZE          : 2;
  __REG32  MSIZE          : 2;
  __REG32  PINCOS         : 1;
  __REG32  PL             : 2;
  __REG32  DBM            : 1;
  __REG32  CT             : 1;
  __REG32                 : 1;
  __REG32  PBURST         : 2;
  __REG32  MBURST         : 2;
  __REG32  CHSEL          : 3;
  __REG32                 : 4;
} __dma_sxcr_bits;

/* DMA stream x number of data register (DMA_SxNDTR) (x = 0..7) */
typedef struct {
  __REG32  NDT            :16;
  __REG32                 :16;
} __dma_sxndtr_bits;

/* DMA stream x FIFO control register (DMA_SxFCR) (x = 0..7) */
typedef struct {
  __REG32  FTH            : 2;
  __REG32  DMDIS          : 1;
  __REG32  FS             : 3;
  __REG32                 : 1;
  __REG32  FEIE           : 1;
  __REG32                 :24;
} __dma_sxfcr_bits;


/***************************************************************************
 **
 ** DMA
 **
 ***************************************************************************/
#define DMA_LISR_S 0X00
#define DMA_HISR_S 0X04
#define DMA_LIFCR_S 0X08
#define DMA_HIFCR_S 0X0C
#define DMA_SxCR 0X10
#define DMA_SxNDTR 0X14
#define DMA_SxPAR 0X18
#define DMA_SxM0AR 0X1C
#define DMA_SxM1AR 0X20
#define DMA_SxFCR 0X24
#define DMA_stream_shift 0X18

/***************************************************************************
 **
 ** DMA1
 **
 ***************************************************************************/
__IO_REG32_BIT(DMA1_LISR,         IRS_DMA1_BASE + DMA_LISR_S,__READ       ,
  __dma_lisr_bits);
__IO_REG32_BIT(DMA1_HISR,         IRS_DMA1_BASE + DMA_HISR_S,__READ       ,
  __dma_hisr_bits);
__IO_REG32_BIT(DMA1_LIFCR,        IRS_DMA1_BASE + DMA_LIFCR_S,__READ_WRITE ,
  __dma_lifcr_bits);
__IO_REG32_BIT(DMA1_HIFCR,        IRS_DMA1_BASE + DMA_HIFCR_S,__READ_WRITE ,
  __dma_hifcr_bits);

__IO_REG32_BIT(DMA1_S0CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*0,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S0NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*0,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S0PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*0,
  __READ_WRITE );
__IO_REG32(    DMA1_S0M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*0,
  __READ_WRITE );
__IO_REG32(    DMA1_S0M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*0,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S0FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*0,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S1CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*1,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S1NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*1,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S1PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*1,
  __READ_WRITE );
__IO_REG32(    DMA1_S1M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*1,
  __READ_WRITE );
__IO_REG32(    DMA1_S1M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*1,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S1FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*1,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S2CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*2,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S2NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*2,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S2PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*2,
  __READ_WRITE );
__IO_REG32(    DMA1_S2M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*2,
  __READ_WRITE );
__IO_REG32(    DMA1_S2M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*2,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S2FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*2,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S3CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*3,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S3NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*3,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S3PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*3,
  __READ_WRITE );
__IO_REG32(    DMA1_S3M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*3,
  __READ_WRITE );
__IO_REG32(    DMA1_S3M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*3,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S3FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*3,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S4CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*4,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S4NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*4,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S4PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*4,
  __READ_WRITE );
__IO_REG32(    DMA1_S4M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*4,
  __READ_WRITE );
__IO_REG32(    DMA1_S4M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*4,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S4FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*4,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S5CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*5,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S5NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*5,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S5PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*5,
  __READ_WRITE );
__IO_REG32(    DMA1_S5M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*5,
  __READ_WRITE );
__IO_REG32(    DMA1_S5M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*5,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S5FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*5,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S6CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*6,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S6NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*6,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S6PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*6,
  __READ_WRITE );
__IO_REG32(    DMA1_S6M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*6,
  __READ_WRITE );
__IO_REG32(    DMA1_S6M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*6,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S6FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*6,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA1_S7CR,         IRS_DMA1_BASE + DMA_SxCR +   DMA_stream_shift*7,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA1_S7NDTR,       IRS_DMA1_BASE + DMA_SxNDTR + DMA_stream_shift*7,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA1_S7PAR,        IRS_DMA1_BASE + DMA_SxPAR +  DMA_stream_shift*7,
  __READ_WRITE );
__IO_REG32(    DMA1_S7M0AR,       IRS_DMA1_BASE + DMA_SxM0AR + DMA_stream_shift*7,
  __READ_WRITE );
__IO_REG32(    DMA1_S7M1AR,       IRS_DMA1_BASE + DMA_SxM1AR + DMA_stream_shift*7,
  __READ_WRITE );
__IO_REG32_BIT(DMA1_S7FCR,        IRS_DMA1_BASE + DMA_SxFCR +  DMA_stream_shift*7,
  __READ_WRITE ,__dma_sxfcr_bits);

/***************************************************************************
 **
 ** DMA2
 **
 ***************************************************************************/

__IO_REG32_BIT(DMA2_LISR,         IRS_DMA2_BASE + DMA_LISR_S,__READ       ,
  __dma_lisr_bits);
__IO_REG32_BIT(DMA2_HISR,         IRS_DMA2_BASE + DMA_HISR_S,__READ       ,
  __dma_hisr_bits);
__IO_REG32_BIT(DMA2_LIFCR,        IRS_DMA2_BASE + DMA_LIFCR_S,__READ_WRITE ,
  __dma_lifcr_bits);
__IO_REG32_BIT(DMA2_HIFCR,        IRS_DMA2_BASE + DMA_HIFCR_S,__READ_WRITE ,
  __dma_hifcr_bits);

__IO_REG32_BIT(DMA2_S0CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*0,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S0NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*0,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S0PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*0,
  __READ_WRITE );
__IO_REG32(    DMA2_S0M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*0,
  __READ_WRITE );
__IO_REG32(    DMA2_S0M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*0,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S0FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*0,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S1CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*1,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S1NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*1,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S1PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*1,
  __READ_WRITE );
__IO_REG32(    DMA2_S1M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*1,
  __READ_WRITE );
__IO_REG32(    DMA2_S1M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*1,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S1FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*1,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S2CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*2,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S2NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*2,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S2PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*2,
  __READ_WRITE );
__IO_REG32(    DMA2_S2M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*2,
  __READ_WRITE );
__IO_REG32(    DMA2_S2M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*2,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S2FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*2,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S3CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*3,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S3NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*3,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S3PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*3,
  __READ_WRITE );
__IO_REG32(    DMA2_S3M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*3,
  __READ_WRITE );
__IO_REG32(    DMA2_S3M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*3,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S3FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*3,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S4CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*4,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S4NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*4,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S4PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*4,
  __READ_WRITE );
__IO_REG32(    DMA2_S4M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*4,
  __READ_WRITE );
__IO_REG32(    DMA2_S4M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*4,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S4FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*4,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S5CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*5,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S5NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*5,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S5PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*5,
  __READ_WRITE );
__IO_REG32(    DMA2_S5M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*5,
  __READ_WRITE );
__IO_REG32(    DMA2_S5M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*5,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S5FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*5,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S6CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*6,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S6NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*6,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S6PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*6,
  __READ_WRITE );
__IO_REG32(    DMA2_S6M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*6,
  __READ_WRITE );
__IO_REG32(    DMA2_S6M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*6,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S6FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*6,
  __READ_WRITE ,__dma_sxfcr_bits);

__IO_REG32_BIT(DMA2_S7CR,         IRS_DMA2_BASE + DMA_SxCR +   DMA_stream_shift*7,
  __READ_WRITE ,__dma_sxcr_bits);
__IO_REG32_BIT(DMA2_S7NDTR,       IRS_DMA2_BASE + DMA_SxNDTR + DMA_stream_shift*7,
  __READ_WRITE ,__dma_sxndtr_bits);
__IO_REG32(    DMA2_S7PAR,        IRS_DMA2_BASE + DMA_SxPAR +  DMA_stream_shift*7,
  __READ_WRITE );
__IO_REG32(    DMA2_S7M0AR,       IRS_DMA2_BASE + DMA_SxM0AR + DMA_stream_shift*7,
  __READ_WRITE );
__IO_REG32(    DMA2_S7M1AR,       IRS_DMA2_BASE + DMA_SxM1AR + DMA_stream_shift*7,
  __READ_WRITE );
__IO_REG32_BIT(DMA2_S7FCR,        IRS_DMA2_BASE + DMA_SxFCR +  DMA_stream_shift*7,
  __READ_WRITE ,__dma_sxfcr_bits);

#define IRS_DMA_ISR 0x00
#define IRS_DMA_IFCR 0x08

enum { dma_stream_count = 8 };

struct dma_regs_t
{
  IRS_IO_REG32_BIT(DMA_LISR, __READ ,__dma_lisr_bits);
  IRS_IO_REG32_BIT(DMA_HISR, __READ ,__dma_hisr_bits);
  IRS_IO_REG32_BIT(DMA_LIFCR, __READ_WRITE ,__dma_lifcr_bits);
  IRS_IO_REG32_BIT(DMA_HIFCR, __READ_WRITE ,__dma_hifcr_bits);

  struct dma_stream_regs_t {
    IRS_IO_REG32_BIT(DMA_SCR, __READ_WRITE, __dma_sxcr_bits);
    IRS_IO_REG32_BIT(DMA_SNDTR, __READ_WRITE, __dma_sxndtr_bits);
    IRS_IO_REG32(DMA_SPAR, __READ_WRITE);
    IRS_IO_REG32(DMA_SM0AR, __READ_WRITE);
    IRS_IO_REG32(DMA_SM1AR, __READ_WRITE);
    IRS_IO_REG32_BIT(DMA_SFCR, __READ_WRITE, __dma_sxfcr_bits);
  } stream[dma_stream_count];
};
  
enum {
  FEIF = 0,
  DMEIF = 2,
  TEIF = 3,
  HTIF = 4,
  TCIF = 5
};

enum {
  CFEIF = 0,
  CDMEIF = 2,
  CTEIF = 3,
  CHTIF = 4,
  CTCIF = 5
};

inline void dma_set_bit(unsigned long a_dma_address, unsigned long a_reg, 
  int a_stream, int a_bit_shift, int a_bit_value)
{
  int shift_stream = (a_stream/2)*2;
  unsigned long address = a_dma_address + a_reg + shift_stream;
  int bit_shift = a_bit_shift + 6*(a_stream%2);
  IRS_SET_BITS(address, bit_shift, 1, a_bit_value);
}

inline int dma_get_bit(unsigned long a_dma_address, unsigned long a_reg, 
  int a_stream, int a_bit_shift)
{
  int shift_stream = (a_stream/2)*2;
  unsigned long address = a_dma_address + a_reg + shift_stream;
  int bit_shift = a_bit_shift + 6*(a_stream%2);
  int bit_value = 0;
  IRS_GET_BITS(address, bit_shift, 1, bit_value);
  return bit_value;
}

/* DAC control register (DAC_CR) */
typedef struct {
  __REG32 EN1             : 1;
  __REG32 BOFF1           : 1;
  __REG32 TEN1            : 1;
  __REG32 TSEL1           : 3;
  __REG32 WAVE1           : 2;
  __REG32 MAMP1           : 4;
  __REG32 DMAEN1          : 1;
  __REG32 DMAUDRIE1       : 1;
  __REG32                 : 2;
  __REG32 EN2             : 1;
  __REG32 BOFF2           : 1;
  __REG32 TEN2            : 1;
  __REG32 TSEL2           : 3;
  __REG32 WAVE2           : 2;
  __REG32 MAMP2           : 4;
  __REG32 DMAEN2          : 1;
  __REG32 DMAUDRIE2       : 1;
  __REG32                 : 2;
} __dac_cr_bits;

/* DAC Software Trigger Register (DAC_SWTRIGR) */
typedef struct {
  __REG32 SWTRIG1         : 1;
  __REG32 SWTRIG2         : 1;
  __REG32                 :30;
} __dac_swtrigr_bits;

/* DAC channel1 12-bit Right-aligned Data Holding Register (DAC_DHR12R1) */
typedef struct {
  __REG32 DACC1DHR        :12;
  __REG32                 :20;
} __dac_dhr12r1_bits;

/* DAC channel1 12-bit Left aligned Data Holding Register (DAC_DHR12L1) */
typedef struct {
  __REG32                 : 4;
  __REG32 DACC1DHR        :12;
  __REG32                 :16;
} __dac_dhr12l1_bits;

/* DAC channel1 8-bit Right aligned Data Holding Register (DAC_DHR8R1) */
typedef struct {
  __REG32 DACC1DHR        : 8;
  __REG32                 :24;
} __dac_dhr8r1_bits;

/* DAC channel2 12-bit Right aligned Data Holding Register (DAC_DHR12R2) */
typedef struct {
  __REG32 DACC2DHR        :12;
  __REG32                 :20;
} __dac_dhr12r2_bits;

/* DAC channel2 12-bit Left aligned Data Holding Register (DAC_DHR12L2) */
typedef struct {
  __REG32                 : 4;
  __REG32 DACC2DHR        :12;
  __REG32                 :16;
} __dac_dhr12l2_bits;

/* DAC channel2 8-bit Right-aligned Data Holding Register (DAC_DHR8R2) */
typedef struct {
  __REG32 DACC2DHR        : 8;
  __REG32                 :24;
} __dac_dhr8r2_bits;

/* Dual DAC 12-bit Right-aligned Data Holding Register (DAC_DHR12RD) */
typedef struct {
  __REG32 DACC1DHR        :12;
  __REG32                 : 4;
  __REG32 DACC2DHR        :12;
  __REG32                 : 4;
} __dac_dhr12rd_bits;

/* DUAL DAC 12-bit Left aligned Data Holding Register (DAC_DHR12LD) */
typedef struct {
  __REG32                 : 4;
  __REG32 DACC1DHR        :12;
  __REG32                 : 4;
  __REG32 DACC2DHR        :12;
} __dac_dhr12ld_bits;

/* DUAL DAC 8-bit Right aligned Data Holding Register (DAC_DHR8RD) */
typedef struct {
  __REG32 DACC1DHR        : 8;
  __REG32 DACC2DHR        : 8;
  __REG32                 :16;
} __dac_dhr8rd_bits;

/* DAC channel1 Data Output Register (DAC_DOR1) */
typedef struct {
  __REG32 DACC1DOR        :12;
  __REG32                 :20;
} __dac_dor1_bits;

/* DAC channel2 Data Output Register (DAC_DOR2) */
typedef struct {
  __REG32 DACC2DOR        :12;
  __REG32                 :20;
} __dac_dor2_bits;

/* DAC status register (DAC_SR) */
typedef struct {
  __REG32                 :13;
  __REG32 DMAUDR1         : 1;
  __REG32                 :15;
  __REG32 DMAUDR2         : 1;
  __REG32                 : 2;
} __dac_sr_bits;

/***************************************************************************
 **
 ** DAC
 **
 ***************************************************************************/
__IO_REG32_BIT(DAC_CR,            0x40007400,__READ_WRITE ,__dac_cr_bits     );
__IO_REG32_BIT(DAC_SWTRIGR,       0x40007404,__READ_WRITE ,__dac_swtrigr_bits);
__IO_REG32_BIT(DAC_DHR12R1,       0x40007408,__READ_WRITE ,__dac_dhr12r1_bits);
__IO_REG32_BIT(DAC_DHR12L1,       0x4000740C,__READ_WRITE ,__dac_dhr12l1_bits);
__IO_REG32_BIT(DAC_DHR8R1,        0x40007410,__READ_WRITE ,__dac_dhr8r1_bits );
__IO_REG32_BIT(DAC_DHR12R2,       0x40007414,__READ_WRITE ,__dac_dhr12r2_bits);
__IO_REG32_BIT(DAC_DHR12L2,       0x40007418,__READ_WRITE ,__dac_dhr12l2_bits);
__IO_REG32_BIT(DAC_DHR8R2,        0x4000741C,__READ_WRITE ,__dac_dhr8r2_bits );
__IO_REG32_BIT(DAC_DHR12RD,       0x40007420,__READ_WRITE ,__dac_dhr12rd_bits);
__IO_REG32_BIT(DAC_DHR12LD,       0x40007424,__READ_WRITE ,__dac_dhr12ld_bits);
__IO_REG32_BIT(DAC_DHR8RD,        0x40007428,__READ_WRITE ,__dac_dhr8rd_bits );
__IO_REG32_BIT(DAC_DOR1,          0x4000742C,__READ_WRITE ,__dac_dor1_bits   );
__IO_REG32_BIT(DAC_DOR2,          0x40007430,__READ_WRITE ,__dac_dor2_bits   );
__IO_REG32_BIT(DAC_SR,            0x40007434,__READ_WRITE ,__dac_sr_bits   );

/* Interrupt mask register (EXTI_IMR) */
typedef struct {
  __REG32  MR0            : 1;
  __REG32  MR1            : 1;
  __REG32  MR2            : 1;
  __REG32  MR3            : 1;
  __REG32  MR4            : 1;
  __REG32  MR5            : 1;
  __REG32  MR6            : 1;
  __REG32  MR7            : 1;
  __REG32  MR8            : 1;
  __REG32  MR9            : 1;
  __REG32  MR10           : 1;
  __REG32  MR11           : 1;
  __REG32  MR12           : 1;
  __REG32  MR13           : 1;
  __REG32  MR14           : 1;
  __REG32  MR15           : 1;
  __REG32  MR16           : 1;
  __REG32  MR17           : 1;
  __REG32  MR18           : 1;
  __REG32  MR19           : 1;
  __REG32  MR20           : 1;
  __REG32  MR21           : 1;
  __REG32  MR22           : 1;
  __REG32                 : 9;
} __exti_imr_bits;

/* Event mask register (EXTI_EMR) */
typedef struct {
  __REG32  MR0            : 1;
  __REG32  MR1            : 1;
  __REG32  MR2            : 1;
  __REG32  MR3            : 1;
  __REG32  MR4            : 1;
  __REG32  MR5            : 1;
  __REG32  MR6            : 1;
  __REG32  MR7            : 1;
  __REG32  MR8            : 1;
  __REG32  MR9            : 1;
  __REG32  MR10           : 1;
  __REG32  MR11           : 1;
  __REG32  MR12           : 1;
  __REG32  MR13           : 1;
  __REG32  MR14           : 1;
  __REG32  MR15           : 1;
  __REG32  MR16           : 1;
  __REG32  MR17           : 1;
  __REG32  MR18           : 1;
  __REG32  MR19           : 1;
  __REG32  MR20           : 1;
  __REG32  MR21           : 1;
  __REG32  MR22           : 1;
  __REG32                 : 9;
} __exti_emr_bits;

/* Rising Trigger selection register (EXTI_RTSR) */
typedef struct {
  __REG32  TR0            : 1;
  __REG32  TR1            : 1;
  __REG32  TR2            : 1;
  __REG32  TR3            : 1;
  __REG32  TR4            : 1;
  __REG32  TR5            : 1;
  __REG32  TR6            : 1;
  __REG32  TR7            : 1;
  __REG32  TR8            : 1;
  __REG32  TR9            : 1;
  __REG32  TR10           : 1;
  __REG32  TR11           : 1;
  __REG32  TR12           : 1;
  __REG32  TR13           : 1;
  __REG32  TR14           : 1;
  __REG32  TR15           : 1;
  __REG32  TR16           : 1;
  __REG32  TR17           : 1;
  __REG32  TR18           : 1;
  __REG32  TR19           : 1;
  __REG32  TR20           : 1;
  __REG32  TR21           : 1;
  __REG32  TR22           : 1;
  __REG32                 : 9;
} __exti_rtsr_bits;

/* Falling Trigger selection register (EXTI_FTSR) */
typedef struct {
  __REG32  TR0            : 1;
  __REG32  TR1            : 1;
  __REG32  TR2            : 1;
  __REG32  TR3            : 1;
  __REG32  TR4            : 1;
  __REG32  TR5            : 1;
  __REG32  TR6            : 1;
  __REG32  TR7            : 1;
  __REG32  TR8            : 1;
  __REG32  TR9            : 1;
  __REG32  TR10           : 1;
  __REG32  TR11           : 1;
  __REG32  TR12           : 1;
  __REG32  TR13           : 1;
  __REG32  TR14           : 1;
  __REG32  TR15           : 1;
  __REG32  TR16           : 1;
  __REG32  TR17           : 1;
  __REG32  TR18           : 1;
  __REG32  TR19           : 1;
  __REG32  TR20           : 1;
  __REG32  TR21           : 1;
  __REG32  TR22           : 1;
  __REG32                 : 9;
} __exti_ftsr_bits;

/* Software interrupt event register (EXTI_SWIER) */
typedef struct {
  __REG32  SWIER0         : 1;
  __REG32  SWIER1         : 1;
  __REG32  SWIER2         : 1;
  __REG32  SWIER3         : 1;
  __REG32  SWIER4         : 1;
  __REG32  SWIER5         : 1;
  __REG32  SWIER6         : 1;
  __REG32  SWIER7         : 1;
  __REG32  SWIER8         : 1;
  __REG32  SWIER9         : 1;
  __REG32  SWIER10        : 1;
  __REG32  SWIER11        : 1;
  __REG32  SWIER12        : 1;
  __REG32  SWIER13        : 1;
  __REG32  SWIER14        : 1;
  __REG32  SWIER15        : 1;
  __REG32  SWIER16        : 1;
  __REG32  SWIER17        : 1;
  __REG32  SWIER18        : 1;
  __REG32  SWIER19        : 1;
  __REG32  SWIER20        : 1;
  __REG32  SWIER21        : 1;
  __REG32  SWIER22        : 1;
  __REG32                 : 9;
} __exti_swier_bits;

/* Pending register (EXTI_PR) */
typedef struct {
  __REG32  PR0            : 1;
  __REG32  PR1            : 1;
  __REG32  PR2            : 1;
  __REG32  PR3            : 1;
  __REG32  PR4            : 1;
  __REG32  PR5            : 1;
  __REG32  PR6            : 1;
  __REG32  PR7            : 1;
  __REG32  PR8            : 1;
  __REG32  PR9            : 1;
  __REG32  PR10           : 1;
  __REG32  PR11           : 1;
  __REG32  PR12           : 1;
  __REG32  PR13           : 1;
  __REG32  PR14           : 1;
  __REG32  PR15           : 1;
  __REG32  PR16           : 1;
  __REG32  PR17           : 1;
  __REG32  PR18           : 1;
  __REG32  PR19           : 1;
  __REG32  PR20           : 1;
  __REG32  PR21           : 1;
  __REG32  PR22           : 1;
  __REG32                 : 9;
} __exti_pr_bits;

/***************************************************************************
 **
 ** EXTI
 **
 ***************************************************************************/
__IO_REG32_BIT(EXTI_IMR,          0x40013C00,__READ_WRITE ,__exti_imr_bits);
__IO_REG32_BIT(EXTI_EMR,          0x40013C04,__READ_WRITE ,__exti_emr_bits);
__IO_REG32_BIT(EXTI_RTSR,         0x40013C08,__READ_WRITE ,__exti_rtsr_bits);
__IO_REG32_BIT(EXTI_FTSR,         0x40013C0C,__READ_WRITE ,__exti_ftsr_bits);
__IO_REG32_BIT(EXTI_SWIER,        0x40013C10,__READ_WRITE ,__exti_swier_bits);
__IO_REG32_BIT(EXTI_PR,           0x40013C14,__READ_WRITE ,__exti_pr_bits);

/* SYSCFG memory remap register (SYSCFG_MEMRMP) */
typedef struct {
  __REG32  MEM_MODE       : 2;
  __REG32                 :30;
} __syscfg_memrmp_bits;

/* SYSCFG peripheral mode configuration register (SYSCFG_PMC) */
typedef struct {
  __REG32                 :23;
  __REG32  MII_RMII_SEL   : 1;
  __REG32                 : 8;
} __syscfg_pmc_bits;

/* SYSCFG external interrupt configuration register 1 (SYSCFG_EXTICR1) */
typedef struct {
  __REG32  EXTI0          : 4;
  __REG32  EXTI1          : 4;
  __REG32  EXTI2          : 4;
  __REG32  EXTI3          : 4;
  __REG32                 :16;
} __syscfg_exticr1_bits;

/* SYSCFG external interrupt configuration register 2 (SYSCFG_EXTICR2) */
typedef struct {
  __REG32  EXTI4          : 4;
  __REG32  EXTI5          : 4;
  __REG32  EXTI6          : 4;
  __REG32  EXTI7          : 4;
  __REG32                 :16;
} __syscfg_exticr2_bits;

/* SYSCFG external interrupt configuration register 3 (SYSCFG_EXTICR3) */
typedef struct {
  __REG32  EXTI8          : 4;
  __REG32  EXTI9          : 4;
  __REG32  EXTI10         : 4;
  __REG32  EXTI11         : 4;
  __REG32                 :16;
} __syscfg_exticr3_bits;

/* SYSCFG external interrupt configuration register 4 (SYSCFG_EXTICR4) */
typedef struct {
  __REG32  EXTI4          : 4;
  __REG32  EXTI5          : 4;
  __REG32  EXTI6          : 4;
  __REG32  EXTI7          : 4;
  __REG32                 :16;
} __syscfg_exticr4_bits;

/***************************************************************************
 **
 ** SYSCFG
 **
 ***************************************************************************/
__IO_REG32_BIT(SYSCFG_MEMRMP,     0x40013800,__READ_WRITE ,__syscfg_memrmp_bits);
__IO_REG32_BIT(SYSCFG_PMC,        0x40013804,__READ_WRITE ,__syscfg_pmc_bits);
__IO_REG32_BIT(SYSCFG_EXTICR1,    0x40013808,__READ_WRITE ,__syscfg_exticr1_bits);
__IO_REG32_BIT(SYSCFG_EXTICR2,    0x4001380C,__READ_WRITE ,__syscfg_exticr2_bits);
__IO_REG32_BIT(SYSCFG_EXTICR3,    0x40013810,__READ_WRITE ,__syscfg_exticr3_bits);
__IO_REG32_BIT(SYSCFG_EXTICR4,    0x40013814,__READ_WRITE ,__syscfg_exticr4_bits);

/* Ethernet MAC configuration register (ETH_MACCR) */
typedef struct {
  __REG32                 : 2;
  __REG32  RE             : 1;
  __REG32  TE             : 1;
  __REG32  DC             : 1;
  __REG32  BL             : 2;
  __REG32  APCS           : 1;
  __REG32                 : 1;
  __REG32  RD             : 1;
  __REG32  IPCO           : 1;
  __REG32  DM             : 1;
  __REG32  LM             : 1;
  __REG32  ROD            : 1;
  __REG32  FES            : 1;
  __REG32                 : 1;
  __REG32  CSD            : 1;
  __REG32  IFG            : 3;
  __REG32                 : 2;
  __REG32  JD             : 1;
  __REG32  WD             : 1;
  __REG32                 : 1;
  __REG32  CSTF           : 1;
  __REG32                 : 6;
} __eth_maccr_bits;

/* Ethernet MAC frame filter register (ETH_MACFFR) */
typedef struct {
  __REG32  PM             : 1;
  __REG32  HU             : 1;
  __REG32  HM             : 1;
  __REG32  DAIF           : 1;
  __REG32  PAM            : 1;
  __REG32  BFD            : 1;
  __REG32  PCF            : 2;
  __REG32  SAIF           : 1;
  __REG32  SAF            : 1;
  __REG32  HPF            : 1;
  __REG32                 :20;
  __REG32  RA             : 1;
} __eth_macffr_bits;

/* Ethernet MAC MII address register (ETH_MACMIIAR) */
typedef struct {
  __REG32  MB             : 1;
  __REG32  MW             : 1;
  __REG32  CR             : 3;
  __REG32                 : 1;
  __REG32  MR             : 5;
  __REG32  PA             : 5;
  __REG32                 :16;
} __eth_macmiiar_bits;

/* Ethernet MAC MII data register (ETH_MACMIIDR) */
typedef struct {
  __REG32  MD             :16;
  __REG32                 :16;
} __eth_macmiidr_bits;

/* Ethernet MAC flow control register (ETH_MACFCR) */
typedef struct {
  __REG32  FCB_BPA        : 1;
  __REG32  TFCE           : 1;
  __REG32  RFCE           : 1;
  __REG32  UPFD           : 1;
  __REG32  PLT            : 2;
  __REG32                 : 1;
  __REG32  ZQPD           : 1;
  __REG32                 : 8;
  __REG32  PT             :16;
} __eth_macfcr_bits;

/* Ethernet MAC VLAN tag register (ETH_MACVLANTR) */
typedef struct {
  __REG32  VLANTI         :16;
  __REG32  VLANTC         : 1;
  __REG32                 :15;
} __eth_macvlantr_bits;

/* Ethernet MAC PMT control and status register (ETH_MACPMTCSR) */
typedef struct {
  __REG32  PD             : 1;
  __REG32  MPE            : 1;
  __REG32  WFE            : 1;
  __REG32                 : 2;
  __REG32  MPR            : 1;
  __REG32  WFR            : 1;
  __REG32                 : 2;
  __REG32  GU             : 1;
  __REG32                 :21;
  __REG32  WFFRPR         : 1;
} __eth_macpmtcsr_bits;

/* Ethernet MAC debug register (ETH_MACDBGR) */
typedef struct {
  __REG32  MMRPEA         : 1;
  __REG32  MSFRWCS        : 2;
  __REG32                 : 1;
  __REG32  RFWRA          : 1;
  __REG32  RFRCS          : 2;
  __REG32                 : 1;
  __REG32  RFFL           : 2;
  __REG32                 : 6;
  __REG32  MMTEA          : 1;
  __REG32  MTFCS          : 2;
  __REG32  MTP            : 1;
  __REG32  TFRS           : 2;
  __REG32  TFWA           : 1;
  __REG32                 : 1;
  __REG32  TFNE           : 1;
  __REG32  TFF            : 1;
  __REG32                 : 6;
} __eth_macdbgr_bits;

/* Ethernet MAC interrupt status register (ETH_MACSR) */
typedef struct {
  __REG32                 : 3;
  __REG32  PMTS           : 1;
  __REG32  MMCS           : 1;
  __REG32  MMCRS          : 1;
  __REG32  MMCTS          : 1;
  __REG32                 : 2;
  __REG32  TSTS           : 1;
  __REG32                 :22;
} __eth_macsr_bits;

/* Ethernet MAC interrupt mask register (ETH_MACIMR) */
typedef struct {
  __REG32                 : 3;
  __REG32  PMTIM          : 1;
  __REG32                 : 5;
  __REG32  TSTIM          : 1;
  __REG32                 :22;
} __eth_macimr_bits;

/* Ethernet MAC address 0 high register (ETH_MACA0HR) */
typedef struct {
  __REG32  MACA0H         :16;
  __REG32                 :15;
  __REG32  MO             : 1;
} __eth_maca0hr_bits;

/* Ethernet MAC address 1 high register (ETH_MACA1HR) */
typedef struct {
  __REG32  MACA1H         :16;
  __REG32                 : 8;
  __REG32  MBC            : 6;
  __REG32  SA             : 1;
  __REG32  AE             : 1;
} __eth_maca1hr_bits;

/* Ethernet MAC address 2 high register (ETH_MACA2HR) */
typedef struct {
  __REG32  MACA2H         :16;
  __REG32                 : 8;
  __REG32  MBC            : 6;
  __REG32  SA             : 1;
  __REG32  AE             : 1;
} __eth_maca2hr_bits;

/* Ethernet MAC address 3 high register (ETH_MACA3HR) */
typedef struct {
  __REG32  MACA3H         :16;
  __REG32                 : 8;
  __REG32  MBC            : 6;
  __REG32  SA             : 1;
  __REG32  AE             : 1;
} __eth_maca3hr_bits;

/* Ethernet MMC control register (ETH_MMCCR) */
typedef struct {
  __REG32  CR             : 1;
  __REG32  CSR            : 1;
  __REG32  ROR            : 1;
  __REG32  MCF            : 1;
  __REG32  MCP            : 1;
  __REG32  MCFHP          : 1;
  __REG32                 :26;
} __eth_mmccr_bits;

/* Ethernet MMC receive interrupt register (ETH_MMCRIR) */
typedef struct {
  __REG32                 : 5;
  __REG32  RFCES          : 1;
  __REG32  RFAES          : 1;
  __REG32                 :10;
  __REG32  RGUFS          : 1;
  __REG32                 :14;
} __eth_mmcrir_bits;

/* Ethernet MMC transmit interrupt register (ETH_MMCTIR) */
typedef struct {
  __REG32                 :14;
  __REG32  TGFSCS         : 1;
  __REG32  TGFMSCS        : 1;
  __REG32                 : 5;
  __REG32  TGFS           : 1;
  __REG32                 :10;
} __eth_mmctir_bits;

/* Ethernet MMC receive interrupt mask register (ETH_MMCRIMR) */
typedef struct {
  __REG32                 : 5;
  __REG32  RFCEM          : 1;
  __REG32  RFAEM          : 1;
  __REG32                 :10;
  __REG32  RGUFM          : 1;
  __REG32                 :14;
} __eth_mmcrimr_bits;

/* Ethernet MMC transmit interrupt mask register (ETH_MMCTIMR) */
typedef struct {
  __REG32                 :14;
  __REG32  TGFSCM         : 1;
  __REG32  TGFMSCM        : 1;
  __REG32                 : 5;
  __REG32  TGFM           : 1;
  __REG32                 :10;
} __eth_mmctimr_bits;

/* Ethernet PTP time stamp control register (ETH_PTPTSCR) */
typedef struct {
  __REG32  TSE            : 1;
  __REG32  TSFCU          : 1;
  __REG32  TSSTI          : 1;
  __REG32  TSSTU          : 1;
  __REG32  TSITE          : 1;
  __REG32  TTSARU         : 1;
  __REG32                 : 2;
  __REG32  TSSARFE        : 1;
  __REG32  TSSSR          : 1;
  __REG32  TSPTPPSV2E     : 1;
  __REG32  TSSPTPOEFE     : 1;
  __REG32  TSSIPV6FE      : 1;
  __REG32  TSSIPV4FE      : 1;
  __REG32  TSSEME         : 1;
  __REG32  TSSMRME        : 1;
  __REG32  TSCNT          : 2;
  __REG32  TSPFFMAE       : 1;
  __REG32                 :13;
} __eth_ptptscr_bits;

/* Ethernet PTP subsecond increment register (ETH_PTPSSIR) */
typedef struct {
  __REG32  STSSI          : 8;
  __REG32                 :24;
} __eth_ptpssir_bits;

/* Ethernet PTP time stamp low register (ETH_PTPTSLR) */
typedef struct {
  __REG32  STSS           :31;
  __REG32  STPNS          : 1;
} __eth_ptptslr_bits;

/* Ethernet PTP time stamp low update register (ETH_PTPTSLUR) */
typedef struct {
  __REG32  TSUSS          :31;
  __REG32  TSUPNS         : 1;
} __eth_ptptslur_bits;

/* Ethernet PTP time stamp status register (ETH_PTPTSSR) */
/* Ethernet PTP PPS control register (ETH_PTPPPSCR) */
typedef struct {
  __REG32  TSSO           : 1;
  __REG32  TSTTR          : 1;
  __REG32                 :30;
} __eth_ptptssr_bits;

/* Ethernet DMA bus mode register (ETH_DMABMR) */
typedef struct {
  __REG32  SR             : 1;
  __REG32  DA             : 1;
  __REG32  DSL            : 5;
  __REG32                 : 1;
  __REG32  PBL            : 6;
  __REG32  RTPR           : 2;
  __REG32  FB             : 1;
  __REG32  RDP            : 6;
  __REG32  USP            : 1;
  __REG32  FPM            : 1;
  __REG32  AAB            : 1;
  __REG32  MB             : 1;
  __REG32                 : 5;
} __eth_dmabmr_bits;

/* Ethernet DMA status register (ETH_DMASR) */
typedef struct {
  __REG32  TS             : 1;
  __REG32  TPSS           : 1;
  __REG32  TBUS           : 1;
  __REG32  TJTS           : 1;
  __REG32  ROS            : 1;
  __REG32  TUS            : 1;
  __REG32  RS             : 1;
  __REG32  RBUS           : 1;
  __REG32  RPSS           : 1;
  __REG32  RWTS           : 1;
  __REG32  ETS            : 1;
  __REG32                 : 2;
  __REG32  FBES           : 1;
  __REG32  ERS            : 1;
  __REG32  AIS            : 1;
  __REG32  NIS            : 1;
  __REG32  RPS            : 3;
  __REG32  TPS            : 3;
  __REG32  EBS            : 3;
  __REG32                 : 1;
  __REG32  MMCS           : 1;
  __REG32  PMTS           : 1;
  __REG32  TSTS           : 1;
  __REG32                 : 2;
} __eth_dmasr_bits;

/* Ethernet DMA operation mode register (ETH_DMAOMR) */

typedef struct {
  __REG32                 : 1;
  __REG32  SR             : 1;
  __REG32  OSF            : 1;
  __REG32  RTC            : 2;
  __REG32                 : 1;
  __REG32  FUGF           : 1;
  __REG32  FEF            : 1;
  __REG32                 : 5;
  __REG32  ST             : 1;
  __REG32  TTC            : 3;
  __REG32                 : 3;
  __REG32  FTF            : 1;
  __REG32  TSF            : 1;
  __REG32                 : 2;
  __REG32  DFRF           : 1;
  __REG32  RSF            : 1;
  __REG32  DTCEFD         : 1;
  __REG32                 : 5;
} __eth_dmaomr_bits;

/* Ethernet DMA interrupt enable register (ETH_DMAIER) */
typedef struct {
  __REG32  TIE            : 1;
  __REG32  TPSIE          : 1;
  __REG32  TBUIE          : 1;
  __REG32  TJTIE          : 1;
  __REG32  ROIE           : 1;
  __REG32  TUIE           : 1;
  __REG32  RIE            : 1;
  __REG32  RBUIE          : 1;
  __REG32  RPSIE          : 1;
  __REG32  RWTIE          : 1;
  __REG32  ETIE           : 1;
  __REG32                 : 2;
  __REG32  FBEIE          : 1;
  __REG32  ERIE           : 1;
  __REG32  AISE           : 1;
  __REG32  NISE           : 1;
  __REG32                 :15;
} __eth_dmaier_bits;

/* Ethernet DMA missed frame and buffer overflow counter register */
/* (ETH_DMAMFBOCR) */
typedef struct {
  __REG32  MFC            :16;
  __REG32  OMFC           : 1;
  __REG32  MFA            :11;
  __REG32  OFOC           : 1;
  __REG32                 : 3;
} __eth_dmamfbocr_bits;

/* Ethernet DMA receive status watchdog timer register (ETH_DMARSWTR) */
typedef struct {
  __REG32  RSWTC          : 8;
  __REG32                 :24;
} __eth_dmarswtr_bits;

/***************************************************************************
 **
 ** Ethernet
 **
 ***************************************************************************/
__IO_REG32_BIT(ETH_MACCR,         0x40028000,__READ_WRITE ,__eth_maccr_bits);
__IO_REG32_BIT(ETH_MACFFR,        0x40028004,__READ_WRITE ,__eth_macffr_bits);
__IO_REG32(    ETH_MACHTHR,       0x40028008,__READ_WRITE );
__IO_REG32(    ETH_MACHTLR,       0x4002800C,__READ_WRITE );
__IO_REG32_BIT(ETH_MACMIIAR,      0x40028010,__READ_WRITE ,__eth_macmiiar_bits);
__IO_REG32_BIT(ETH_MACMIIDR,      0x40028014,__READ_WRITE ,__eth_macmiidr_bits);
__IO_REG32_BIT(ETH_MACFCR,        0x40028018,__READ_WRITE ,__eth_macfcr_bits);
__IO_REG32_BIT(ETH_MACVLANTR,     0x4002801C,__READ_WRITE ,__eth_macvlantr_bits);
__IO_REG32(    ETH_MACRWUFFR,     0x40028028,__READ_WRITE );
__IO_REG32_BIT(ETH_MACPMTCSR,     0x4002802C,__READ_WRITE ,__eth_macpmtcsr_bits);
__IO_REG32_BIT(ETH_MACDBGR,       0x40028034,__READ       ,__eth_macdbgr_bits);
__IO_REG32_BIT(ETH_MACSR,         0x40028038,__READ       ,__eth_macsr_bits);
__IO_REG32_BIT(ETH_MACIMR,        0x4002803C,__READ_WRITE ,__eth_macimr_bits);
__IO_REG32_BIT(ETH_MACA0HR,       0x40028040,__READ_WRITE ,__eth_maca0hr_bits);
__IO_REG32(    ETH_MACA0LR,       0x40028044,__READ_WRITE );
__IO_REG32_BIT(ETH_MACA1HR,       0x40028048,__READ_WRITE ,__eth_maca1hr_bits);
__IO_REG32(    ETH_MACA1LR,       0x4002804C,__READ_WRITE );
__IO_REG32_BIT(ETH_MACA2HR,       0x40028050,__READ_WRITE ,__eth_maca2hr_bits);
__IO_REG32(    ETH_MACA2LR,       0x40028054,__READ_WRITE );
__IO_REG32_BIT(ETH_MACA3HR,       0x40028058,__READ_WRITE ,__eth_maca3hr_bits);
__IO_REG32(    ETH_MACA3LR,       0x4002805C,__READ_WRITE );
__IO_REG32_BIT(ETH_MMCCR,         0x40028100,__READ_WRITE ,__eth_mmccr_bits);
__IO_REG32_BIT(ETH_MMCRIR,        0x40028104,__READ       ,__eth_mmcrir_bits);
__IO_REG32_BIT(ETH_MMCTIR,        0x40028108,__READ       ,__eth_mmctir_bits);
__IO_REG32_BIT(ETH_MMCRIMR,       0x4002810C,__READ_WRITE ,__eth_mmcrimr_bits);
__IO_REG32_BIT(ETH_MMCTIMR,       0x40028110,__READ_WRITE ,__eth_mmctimr_bits);
__IO_REG32(    ETH_MMCTGFSCCR,    0x4002814C,__READ       );
__IO_REG32(    ETH_MMCTGFMSCCR,   0x40028150,__READ       );
__IO_REG32(    ETH_MMCTGFCR,      0x40028168,__READ       );
__IO_REG32(    ETH_MMCRFCECR,     0x40028194,__READ       );
__IO_REG32(    ETH_MMCRFAECR,     0x40028198,__READ       );
__IO_REG32(    ETH_MMCRGUFCR,     0x400281C4,__READ       );
__IO_REG32_BIT(ETH_PTPTSCR,       0x40028700,__READ_WRITE ,__eth_ptptscr_bits);
__IO_REG32_BIT(ETH_PTPSSIR,       0x40028704,__READ_WRITE ,__eth_ptpssir_bits);
__IO_REG32(    ETH_PTPTSHR,       0x40028708,__READ       );
__IO_REG32_BIT(ETH_PTPTSLR,       0x4002870C,__READ       ,__eth_ptptslr_bits);
__IO_REG32(    ETH_PTPTSHUR,      0x40028710,__READ_WRITE );
__IO_REG32_BIT(ETH_PTPTSLUR,      0x40028714,__READ_WRITE ,__eth_ptptslur_bits);
__IO_REG32(    ETH_PTPTSAR,       0x40028718,__READ_WRITE );
__IO_REG32(    ETH_PTPTTHR,       0x4002871C,__READ_WRITE );
__IO_REG32(    ETH_PTPTTLR,       0x40028720,__READ_WRITE );
__IO_REG32_BIT(ETH_PTPTSSR,       0x40028728,__READ       ,__eth_ptptssr_bits);
__IO_REG32_BIT(ETH_PTPPPSCR,      0x4002872C,__READ       ,__eth_ptptssr_bits);
__IO_REG32_BIT(ETH_DMABMR,        0x40029000,__READ_WRITE ,__eth_dmabmr_bits);
__IO_REG32(    ETH_DMATPDR,       0x40029004,__READ_WRITE );
__IO_REG32(    ETH_DMARPDR,       0x40029008,__READ_WRITE );
__IO_REG32(    ETH_DMARDLAR,      0x4002900C,__READ_WRITE );
__IO_REG32(    ETH_DMATDLAR,      0x40029010,__READ_WRITE );
__IO_REG32_BIT(ETH_DMASR,         0x40029014,__READ_WRITE ,__eth_dmasr_bits);
__IO_REG32_BIT(ETH_DMAOMR,        0x40029018,__READ_WRITE ,__eth_dmaomr_bits);
__IO_REG32_BIT(ETH_DMAIER,        0x4002901C,__READ_WRITE ,__eth_dmaier_bits);
__IO_REG32_BIT(ETH_DMAMFBOCR,     0x40029020,__READ_WRITE ,__eth_dmamfbocr_bits);
__IO_REG32_BIT(ETH_DMARSWTR,      0x40029024,__READ_WRITE ,__eth_dmarswtr_bits);
__IO_REG32(    ETH_DMACHTDR,      0x40029048,__READ       );
__IO_REG32(    ETH_DMACHRDR,      0x4002904C,__READ       );
__IO_REG32(    ETH_DMACHTBAR,     0x40029050,__READ       );
__IO_REG32(    ETH_DMACHRBAR,     0x40029054,__READ       );

#endif  //  IRS_STM32F2xx

#endif // armregs_stm32f2xxH
