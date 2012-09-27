#ifndef armregs_stm32f2xxH
#define armregs_stm32f2xxH

#include <irsdefs.h>

#include <armiomacros.h>

#include <irsfinal.h>

#ifdef IRS_STM32F2xx

#define RNG_BASE              0x50060800
#define DCMI_BASE             0x50050000
#define USB_OTG_FS_BASE       0x50000000
#define USB_OTG_HS_BASE       0x40040000
#define ETHERNET_BASE         0x40028000
#define DMA2_BASE             0x40026400
#define DMA1_BASE             0x40026000
#define BKPSRAM_BASE          0x40024000
#define FLASH_INTERFACE_BASE  0x40023C00
#define RCC_BASE              0x40023800
#define CRC_BASE              0x40023000
#define PORTI_BASE            0x40022000
#define PORTH_BASE            0x40021C00
#define PORTG_BASE            0x40021800
#define PORTF_BASE            0x40021400
#define PORTE_BASE            0x40021000
#define PORTD_BASE            0x40020C00
#define PORTC_BASE            0x40020800
#define PORTB_BASE            0x40020400
#define PORTA_BASE            0x40020000
#define TIM11_BASE            0x40014800
#define TIM10_BASE            0x40014400
#define TIM9_BASE             0x40014000
#define EXTI_BASE             0x40013C00
#define SYSCFG_BASE           0x40013800
#define SPI1_BASE             0x40013000
#define SDIO_BASE             0x40012C00
#define ADC1_ADC2_ADC3_BASE   0x40012000
#define ADC1_BASE             0x40012000
#define ADC2_BASE             0x40012100
#define ADC3_BASE             0x40012200
#define USART6_BASE           0x40011400
#define USART1_BASE           0x40011000
#define TIM8_PWM2_BASE        0x40010400
#define TIM1_PWM1_BASE        0x40010000
#define DAC1_DAC2_BASE        0x40007400
#define PWR_BASE              0x40007000
#define BxCAN2_BASE           0x40006800
#define BxCAN1_BASE           0x40006400
#define I2C3_BASE             0x40005C00
#define I2C2_BASE             0x40005800
#define I2C1_BASE             0x40005400
#define UART5_BASE            0x40005000
#define UART4_BASE            0x40004C00
#define USART3_BASE           0x40004800
#define USART2_BASE           0x40004400
#define SPI3_I2S3_BASE        0x40003C00
#define SPI2_I2S2_BASE        0x40003800
#define IWDG_BASE             0x40003000
#define WWDG_BASE             0x40002C00
#define RTC_BKP_BASE          0x40002800
#define TIM14_BASE            0x40002000
#define TIM13_BASE            0x40001C00
#define TIM12_BASE            0x40001800
#define TIM7_BASE             0x40001400
#define TIM6_BASE             0x40001000
#define TIM5_BASE             0x40000C00
#define TIM4_BASE             0x40000800
#define TIM3_BASE             0x40000400
#define TIM2_BASE             0x40000000

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
    case stn_tim1: return TIM1_PWM1_BASE;
    case stn_tim2: return TIM2_BASE;
    case stn_tim3: return TIM3_BASE;
    case stn_tim4: return TIM4_BASE;
    case stn_tim5: return TIM5_BASE;
    case stn_tim6: return TIM6_BASE;
    case stn_tim7: return TIM7_BASE;
    case stn_tim8: return TIM8_PWM2_BASE;
    case stn_tim9: return TIM9_BASE;
    case stn_tim10: return TIM10_BASE;
    case stn_tim11: return TIM11_BASE;
    case stn_tim12: return TIM12_BASE;
    case stn_tim13: return TIM13_BASE;
    case stn_tim14: return TIM14_BASE;
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

__IO_REG32_BIT(RCC_CR,            RCC_BASE + RCC_CR_S,__READ_WRITE ,__rcc_cr_bits);
__IO_REG32_BIT(RCC_PLLCFGR,       RCC_BASE + RCC_PLLCFGR_S,__READ_WRITE ,__rcc_pllcfgr_bits);
__IO_REG32_BIT(RCC_CFGR,          RCC_BASE + RCC_CFGR_S,__READ_WRITE ,__rcc_cfgr_bits);
__IO_REG32_BIT(RCC_CIR,           RCC_BASE + RCC_CIR_S,__READ_WRITE ,__rcc_cir_bits);
__IO_REG32_BIT(RCC_AHB1RSTR,      RCC_BASE + RCC_AHB1RSTR_S,__READ_WRITE ,__rcc_ahb1rstr_bits);
__IO_REG32_BIT(RCC_AHB2RSTR,      RCC_BASE + RCC_AHB2RSTR_S,__READ_WRITE ,__rcc_ahb2rstr_bits);
__IO_REG32_BIT(RCC_AHB3RSTR,      RCC_BASE + RCC_AHB3RSTR_S,__READ_WRITE ,__rcc_ahb3rstr_bits);
__IO_REG32_BIT(RCC_APB1RSTR,      RCC_BASE + RCC_APB1RSTR_S,__READ_WRITE ,__rcc_apb1rstr_bits);
__IO_REG32_BIT(RCC_APB2RSTR,      RCC_BASE + RCC_APB2RSTR_S,__READ_WRITE ,__rcc_apb2rstr_bits);
__IO_REG32_BIT(RCC_AHB1ENR,       RCC_BASE + RCC_AHB1ENR_S,__READ_WRITE ,__rcc_ahb1enr_bits);
__IO_REG32_BIT(RCC_AHB2ENR,       RCC_BASE + RCC_AHB2ENR_S,__READ_WRITE ,__rcc_ahb2enr_bits);
__IO_REG32_BIT(RCC_AHB3ENR,       RCC_BASE + RCC_AHB3ENR_S,__READ_WRITE ,__rcc_ahb3enr_bits);
__IO_REG32_BIT(RCC_APB1ENR,       RCC_BASE + RCC_APB1ENR_S,__READ_WRITE ,__rcc_apb1enr_bits);
__IO_REG32_BIT(RCC_APB2ENR,       RCC_BASE + RCC_APB2ENR_S,__READ_WRITE ,__rcc_apb2enr_bits);
__IO_REG32_BIT(RCC_AHB1LPENR,     RCC_BASE + RCC_AHB1LPENR_S,__READ_WRITE ,__rcc_ahb1lpenr_bits);
__IO_REG32_BIT(RCC_AHB2LPENR,     RCC_BASE + RCC_AHB2LPENR_S,__READ_WRITE ,__rcc_ahb2lpenr_bits);
__IO_REG32_BIT(RCC_AHB3LPENR,     RCC_BASE + RCC_AHB3LPENR_S,__READ_WRITE ,__rcc_ahb3lpenr_bits);
__IO_REG32_BIT(RCC_APB1LPENR,     RCC_BASE + RCC_APB1LPENR_S,__READ_WRITE ,__rcc_apb1lpenr_bits);
__IO_REG32_BIT(RCC_APB2LPENR,     RCC_BASE + RCC_APB2LPENR_S,__READ_WRITE ,__rcc_apb2lpenr_bits);
__IO_REG32_BIT(RCC_BDCR,          RCC_BASE + RCC_BDCR_S,__READ_WRITE ,__rcc_bdcr_bits);
__IO_REG32_BIT(RCC_CSR,           RCC_BASE + RCC_CSR_S,__READ_WRITE ,__rcc_csr_bits);
__IO_REG32_BIT(RCC_SSCGR,         RCC_BASE + RCC_SSCGR_S,__READ_WRITE ,__rcc_sscgr_bits);
__IO_REG32_BIT(RCC_PLLI2SCFGR,    RCC_BASE + RCC_PLLI2SCFGR_S,__READ_WRITE ,__rcc_plli2scfgr_bits);

#define GPIO_PORTA (*((volatile irs_u32 *) PORTA_BASE))
#define GPIO_PORTB (*((volatile irs_u32 *) PORTB_BASE))
#define GPIO_PORTC (*((volatile irs_u32 *) PORTC_BASE))
#define GPIO_PORTD (*((volatile irs_u32 *) PORTD_BASE))
#define GPIO_PORTE (*((volatile irs_u32 *) PORTE_BASE))
#define GPIO_PORTF (*((volatile irs_u32 *) PORTF_BASE))
#define GPIO_PORTG (*((volatile irs_u32 *) PORTG_BASE))
#define GPIO_PORTH (*((volatile irs_u32 *) PORTH_BASE))
#define GPIO_PORTI (*((volatile irs_u32 *) PORTI_BASE))

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
__IO_REG32_BIT(GPIOA_MODER,       PORTA_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOA_OTYPER,      PORTA_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOA_OSPEEDR,     PORTA_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOA_PUPDR,       PORTA_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOA_IDR,         PORTA_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOA_ODR,         PORTA_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOA_BSRR,        PORTA_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOA_LCKR,        PORTA_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOA_AFRL,        PORTA_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOA_AFRH,        PORTA_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOB
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOB_MODER,       PORTB_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOB_OTYPER,      PORTB_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOB_OSPEEDR,     PORTB_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOB_PUPDR,       PORTB_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOB_IDR,         PORTB_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOB_ODR,         PORTB_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOB_BSRR,        PORTB_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOB_LCKR,        PORTB_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOB_AFRL,        PORTB_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOB_AFRH,        PORTB_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOC
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOC_MODER,       PORTC_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOC_OTYPER,      PORTC_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOC_OSPEEDR,     PORTC_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOC_PUPDR,       PORTC_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOC_IDR,         PORTC_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOC_ODR,         PORTC_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOC_BSRR,        PORTC_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOC_LCKR,        PORTC_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOC_AFRL,        PORTC_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOC_AFRH,        PORTC_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOD
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOD_MODER,       PORTD_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOD_OTYPER,      PORTD_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOD_OSPEEDR,     PORTD_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOD_PUPDR,       PORTD_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOD_IDR,         PORTD_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOD_ODR,         PORTD_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOD_BSRR,        PORTD_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOD_LCKR,        PORTD_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOD_AFRL,        PORTD_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOD_AFRH,        PORTD_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOE
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOE_MODER,       PORTE_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOE_OTYPER,      PORTE_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOE_OSPEEDR,     PORTE_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOE_PUPDR,       PORTE_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOE_IDR,         PORTE_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOE_ODR,         PORTE_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOE_BSRR,        PORTE_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOE_LCKR,        PORTE_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOE_AFRL,        PORTE_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOE_AFRH,        PORTE_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOF
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOF_MODER,       PORTF_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOF_OTYPER,      PORTF_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOF_OSPEEDR,     PORTF_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOF_PUPDR,       PORTF_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOF_IDR,         PORTF_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOF_ODR,         PORTF_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOF_BSRR,        PORTF_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOF_LCKR,        PORTF_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOF_AFRL,        PORTF_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOF_AFRH,        PORTF_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOG
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOG_MODER,       PORTG_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOG_OTYPER,      PORTG_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOG_OSPEEDR,     PORTG_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOG_PUPDR,       PORTG_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOG_IDR,         PORTG_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOG_ODR,         PORTG_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOG_BSRR,        PORTG_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOG_LCKR,        PORTG_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOG_AFRL,        PORTG_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOG_AFRH,        PORTG_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOH
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOH_MODER,       PORTH_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOH_OTYPER,      PORTH_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOH_OSPEEDR,     PORTH_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOH_PUPDR,       PORTH_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOH_IDR,         PORTH_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOH_ODR,         PORTH_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOH_BSRR,        PORTH_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOH_LCKR,        PORTH_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOH_AFRL,        PORTH_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOH_AFRH,        PORTH_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

/***************************************************************************
 **
 ** GPIOI
 **
 ***************************************************************************/
__IO_REG32_BIT(GPIOI_MODER,       PORTI_BASE + GPIO_MODER_S,__READ_WRITE ,__gpio_moder_bits);
__IO_REG32_BIT(GPIOI_OTYPER,      PORTI_BASE + GPIO_OTYPER_S,__READ_WRITE ,__gpio_otyper_bits);
__IO_REG32_BIT(GPIOI_OSPEEDR,     PORTI_BASE + GPIO_OSPEEDR_S,__READ_WRITE ,__gpio_ospeedr_bits);
__IO_REG32_BIT(GPIOI_PUPDR,       PORTI_BASE + GPIO_PUPDR_S,__READ_WRITE ,__gpio_pupdr_bits);
__IO_REG32_BIT(GPIOI_IDR,         PORTI_BASE + GPIO_IDR_S,__READ       ,__gpio_idr_bits);
__IO_REG32_BIT(GPIOI_ODR,         PORTI_BASE + GPIO_ODR_S,__READ_WRITE ,__gpio_odr_bits);
__IO_REG32_BIT(GPIOI_BSRR,        PORTI_BASE + GPIO_BSRR_S,__WRITE      ,__gpio_bsrr_bits);
__IO_REG32_BIT(GPIOI_LCKR,        PORTI_BASE + GPIO_LCKR_S,__READ_WRITE ,__gpio_lckr_bits);
__IO_REG32_BIT(GPIOI_AFRL,        PORTI_BASE + GPIO_AFRL_S,__READ_WRITE ,__gpio_afrl_bits);
__IO_REG32_BIT(GPIOI_AFRH,        PORTI_BASE + GPIO_AFRH_S,__READ_WRITE ,__gpio_afrh_bits);

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
__IO_REG32_BIT(TIM1_CR1,          TIM1_PWM1_BASE + TIM_CR1_S,__READ_WRITE ,__tim1_cr1_bits);
__IO_REG32_BIT(TIM1_CR2,          TIM1_PWM1_BASE + TIM_CR2_S,__READ_WRITE ,__tim1_cr2_bits);
__IO_REG32_BIT(TIM1_SMCR,         TIM1_PWM1_BASE + TIM_SMCR_S,__READ_WRITE ,__tim1_smcr_bits);
__IO_REG32_BIT(TIM1_DIER,         TIM1_PWM1_BASE + TIM_DIER_S,__READ_WRITE ,__tim1_dier_bits);
__IO_REG32_BIT(TIM1_SR,           TIM1_PWM1_BASE + TIM_SR_S,__READ_WRITE ,__tim1_sr_bits);
__IO_REG32_BIT(TIM1_EGR,          TIM1_PWM1_BASE + TIM_EGR_S,__READ_WRITE ,__tim1_egr_bits);
__IO_REG32_BIT(TIM1_CCMR1,        TIM1_PWM1_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim1_ccmr1_bits);
#define TIM1_OCMR1      TIM1_CCMR1
#define TIM1_OCMR1_BIT  TIM1_CCMR1_BIT
__IO_REG32_BIT(TIM1_CCMR2,        TIM1_PWM1_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim1_ccmr2_bits);
#define TIM1_OCMR2      TIM1_CCMR2
#define TIM1_OCMR2_BIT  TIM1_CCMR2_BIT
__IO_REG32_BIT(TIM1_CCER,         TIM1_PWM1_BASE + TIM_CCER_S,__READ_WRITE ,__tim1_ccer_bits);
__IO_REG32_BIT(TIM1_CNT,          TIM1_PWM1_BASE + TIM_CNT_S,__READ_WRITE ,__tim1_cnt_bits);
__IO_REG32_BIT(TIM1_PSC,          TIM1_PWM1_BASE + TIM_PSC_S,__READ_WRITE ,__tim1_psc_bits);
__IO_REG32_BIT(TIM1_ARR,          TIM1_PWM1_BASE + TIM_ARR_S,__READ_WRITE ,__tim1_arr_bits);
__IO_REG32_BIT(TIM1_RCR,          TIM1_PWM1_BASE + TIM_RCR_S,__READ_WRITE ,__tim1_rcr_bits);
__IO_REG32_BIT(TIM1_CCR1,         TIM1_PWM1_BASE + TIM_CCR1_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_CCR2,         TIM1_PWM1_BASE + TIM_CCR2_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_CCR3,         TIM1_PWM1_BASE + TIM_CCR3_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_CCR4,         TIM1_PWM1_BASE + TIM_CCR4_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM1_BDTR,         TIM1_PWM1_BASE + TIM_BDTR_S,__READ_WRITE ,__tim1_bdtr_bits);
__IO_REG32_BIT(TIM1_DCR,          TIM1_PWM1_BASE + TIM_DCR_S,__READ_WRITE ,__tim1_dcr_bits);
__IO_REG32_BIT(TIM1_DMAR,         TIM1_PWM1_BASE + TIM_DMAR_S,__READ_WRITE ,__tim1_dmar_bits);

/***************************************************************************
 **
 ** TIM8
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM8_CR1,          TIM8_PWM2_BASE + TIM_CR1_S,__READ_WRITE ,__tim1_cr1_bits);
__IO_REG32_BIT(TIM8_CR2,          TIM8_PWM2_BASE + TIM_CR2_S,__READ_WRITE ,__tim1_cr2_bits);
__IO_REG32_BIT(TIM8_SMCR,         TIM8_PWM2_BASE + TIM_SMCR_S,__READ_WRITE ,__tim1_smcr_bits);
__IO_REG32_BIT(TIM8_DIER,         TIM8_PWM2_BASE + TIM_DIER_S,__READ_WRITE ,__tim1_dier_bits);
__IO_REG32_BIT(TIM8_SR,           TIM8_PWM2_BASE + TIM_SR_S,__READ_WRITE ,__tim1_sr_bits);
__IO_REG32_BIT(TIM8_EGR,          TIM8_PWM2_BASE + TIM_EGR_S,__READ_WRITE ,__tim1_egr_bits);
__IO_REG32_BIT(TIM8_CCMR1,        TIM8_PWM2_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim1_ccmr1_bits);
#define TIM1_OCMR8      TIM8_CCMR1
#define TIM1_OCMR8_BIT  TIM8_CCMR1_BIT
__IO_REG32_BIT(TIM8_CCMR2,        TIM8_PWM2_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim1_ccmr2_bits);
#define TIM8_OCMR2      TIM8_CCMR2
#define TIM8_OCMR2_BIT  TIM8_CCMR2_BIT
__IO_REG32_BIT(TIM8_CCER,         TIM8_PWM2_BASE + TIM_CCER_S,__READ_WRITE ,__tim1_ccer_bits);
__IO_REG32_BIT(TIM8_CNT,          TIM8_PWM2_BASE + TIM_CNT_S,__READ_WRITE ,__tim1_cnt_bits);
__IO_REG32_BIT(TIM8_PSC,          TIM8_PWM2_BASE + TIM_PSC_S,__READ_WRITE ,__tim1_psc_bits);
__IO_REG32_BIT(TIM8_ARR,          TIM8_PWM2_BASE + TIM_ARR_S,__READ_WRITE ,__tim1_arr_bits);
__IO_REG32_BIT(TIM8_RCR,          TIM8_PWM2_BASE + TIM_RCR_S,__READ_WRITE ,__tim1_rcr_bits);
__IO_REG32_BIT(TIM8_CCR1,         TIM8_PWM2_BASE + TIM_CCR1_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_CCR2,         TIM8_PWM2_BASE + TIM_CCR2_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_CCR3,         TIM8_PWM2_BASE + TIM_CCR3_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_CCR4,         TIM8_PWM2_BASE + TIM_CCR4_S,__READ_WRITE ,__tim1_ccr_bits);
__IO_REG32_BIT(TIM8_BDTR,         TIM8_PWM2_BASE + TIM_BDTR_S,__READ_WRITE ,__tim1_bdtr_bits);
__IO_REG32_BIT(TIM8_DCR,          TIM8_PWM2_BASE + TIM_DCR_S,__READ_WRITE ,__tim1_dcr_bits);
__IO_REG32_BIT(TIM8_DMAR,         TIM8_PWM2_BASE + TIM_DMAR_S,__READ_WRITE ,__tim1_dmar_bits);

/***************************************************************************
 **
 ** TIM3
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM3_CR1,          TIM3_BASE + TIM_CR1_S,__READ_WRITE ,__tim_cr1_bits);
__IO_REG32_BIT(TIM3_CR2,          TIM3_BASE + TIM_CR2_S,__READ_WRITE ,__tim_cr2_bits);
__IO_REG32_BIT(TIM3_SMCR,         TIM3_BASE + TIM_SMCR_S,__READ_WRITE ,__tim_smcr_bits);
__IO_REG32_BIT(TIM3_DIER,         TIM3_BASE + TIM_DIER_S,__READ_WRITE ,__tim_dier_bits);
__IO_REG32_BIT(TIM3_SR,           TIM3_BASE + TIM_SR_S,__READ_WRITE ,__tim_sr_bits);
__IO_REG32_BIT(TIM3_EGR,          TIM3_BASE + TIM_EGR_S,__READ_WRITE ,__tim_egr_bits);
__IO_REG32_BIT(TIM3_CCMR1,        TIM3_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim_ccmr1_bits);
#define TIM3_OCMR1      TIM3_CCMR1
#define TIM3_OCMR1_BIT  TIM3_CCMR1_BIT
__IO_REG32_BIT(TIM3_CCMR2,        TIM3_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim_ccmr2_bits);
#define TIM3_OCMR2      TIM3_CCMR2
#define TIM3_OCMR2_BIT  TIM3_CCMR2_BIT
__IO_REG32_BIT(TIM3_CCER,         TIM3_BASE + TIM_CCER_S,__READ_WRITE ,__tim_ccer_bits);
__IO_REG32(    TIM3_CNT,          TIM3_BASE + TIM_CNT_S,__READ_WRITE );
__IO_REG32_BIT(TIM3_PSC,          TIM3_BASE + TIM_PSC_S,__READ_WRITE ,__tim_psc_bits);
__IO_REG32(    TIM3_ARR,          TIM3_BASE + TIM_ARR_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR1,         TIM3_BASE + TIM_CCR1_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR2,         TIM3_BASE + TIM_CCR2_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR3,         TIM3_BASE + TIM_CCR3_S,__READ_WRITE );
__IO_REG32(    TIM3_CCR4,         TIM3_BASE + TIM_CCR4_S,__READ_WRITE );
__IO_REG32_BIT(TIM3_DCR,          TIM3_BASE + TIM_DCR_S,__READ_WRITE ,__tim_dcr_bits);
__IO_REG32_BIT(TIM3_DMAR,         TIM3_BASE + TIM_DMAR_S,__READ_WRITE ,__tim_dmar_bits);

/***************************************************************************
 **
 ** TIM4
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM4_CR1,          TIM4_BASE + TIM_CR1_S,__READ_WRITE ,__tim_cr1_bits);
__IO_REG32_BIT(TIM4_CR2,          TIM4_BASE + TIM_CR2_S,__READ_WRITE ,__tim_cr2_bits);
__IO_REG32_BIT(TIM4_SMCR,         TIM4_BASE + TIM_SMCR_S,__READ_WRITE ,__tim_smcr_bits);
__IO_REG32_BIT(TIM4_DIER,         TIM4_BASE + TIM_DIER_S,__READ_WRITE ,__tim_dier_bits);
__IO_REG32_BIT(TIM4_SR,           TIM4_BASE + TIM_SR_S,__READ_WRITE ,__tim_sr_bits);
__IO_REG32_BIT(TIM4_EGR,          TIM4_BASE + TIM_EGR_S,__READ_WRITE ,__tim_egr_bits);
__IO_REG32_BIT(TIM4_CCMR1,        TIM4_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim_ccmr1_bits);
#define TIM4_OCMR1      TIM4_CCMR1
#define TIM4_OCMR1_BIT  TIM4_CCMR1_BIT
__IO_REG32_BIT(TIM4_CCMR2,        TIM4_BASE + TIM_CCMR2_S,__READ_WRITE ,__tim_ccmr2_bits);
#define TIM4_OCMR2      TIM4_CCMR2
#define TIM4_OCMR2_BIT  TIM4_CCMR2_BIT
__IO_REG32_BIT(TIM4_CCER,         TIM4_BASE + TIM_CCER_S,__READ_WRITE ,__tim_ccer_bits);
__IO_REG32(    TIM4_CNT,          TIM4_BASE + TIM_CNT_S,__READ_WRITE );
__IO_REG32_BIT(TIM4_PSC,          TIM4_BASE + TIM_PSC_S,__READ_WRITE ,__tim_psc_bits);
__IO_REG32(    TIM4_ARR,          TIM4_BASE + TIM_ARR_S,__READ_WRITE );
__IO_REG32(    TIM4_CCR1,         TIM4_BASE + TIM_CCR1_S,__READ_WRITE );
__IO_REG32(    TIM4_CCR2,         TIM4_BASE + TIM_CCR2_S,__READ_WRITE );
__IO_REG32_BIT(TIM4_DCR,          TIM4_BASE + TIM_DCR_S,__READ_WRITE ,__tim_dcr_bits);
__IO_REG32_BIT(TIM4_DMAR,         TIM4_BASE + TIM_DMAR_S,__READ_WRITE ,__tim_dmar_bits);

/***************************************************************************
 **
 ** TIM9
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM9_CR1,          TIM9_BASE + TIM_CR1_S,__READ_WRITE ,__tim9_cr1_bits);
__IO_REG32_BIT(TIM9_CR2,          TIM9_BASE + TIM_CR2_S,__READ_WRITE ,__tim9_cr2_bits);
__IO_REG32_BIT(TIM9_SMCR,         TIM9_BASE + TIM_SMCR_S,__READ_WRITE ,__tim9_smcr_bits);
__IO_REG32_BIT(TIM9_DIER,         TIM9_BASE + TIM_DIER_S,__READ_WRITE ,__tim9_dier_bits);
__IO_REG32_BIT(TIM9_SR,           TIM9_BASE + TIM_SR_S,__READ_WRITE ,__tim9_sr_bits);
__IO_REG32_BIT(TIM9_EGR,          TIM9_BASE + TIM_EGR_S,__READ_WRITE ,__tim9_egr_bits);
__IO_REG32_BIT(TIM9_CCMR1,        TIM9_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim9_ccmr1_bits);
#define TIM9_OCMR1      TIM9_CCMR1
#define TIM9_OCMR1_BIT  TIM9_CCMR1_BIT
__IO_REG32_BIT(TIM9_CCER,         TIM9_BASE + TIM_CCER_S,__READ_WRITE ,__tim9_ccer_bits);
__IO_REG32_BIT(TIM9_CNT,          TIM9_BASE + TIM_CNT_S,__READ_WRITE ,__tim9_cnt_bits);
__IO_REG32_BIT(TIM9_PSC,          TIM9_BASE + TIM_PSC_S,__READ_WRITE ,__tim9_psc_bits);
__IO_REG32_BIT(TIM9_ARR,          TIM9_BASE + TIM_ARR_S,__READ_WRITE ,__tim9_arr_bits);
__IO_REG32_BIT(TIM9_CCR1,         TIM9_BASE + TIM_CCR1_S,__READ_WRITE ,__tim9_ccr_bits);
__IO_REG32_BIT(TIM9_CCR2,         TIM9_BASE + TIM_CCR2_S,__READ_WRITE ,__tim9_ccr_bits);

/***************************************************************************
 **
 ** TIM12
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM12_CR1,         TIM12_BASE + TIM_CR1_S,__READ_WRITE ,__tim9_cr1_bits);
__IO_REG32_BIT(TIM12_CR2,         TIM12_BASE + TIM_CR2_S,__READ_WRITE ,__tim9_cr2_bits);
__IO_REG32_BIT(TIM12_SMCR,        TIM12_BASE + TIM_SMCR_S,__READ_WRITE ,__tim9_smcr_bits);
__IO_REG32_BIT(TIM12_DIER,        TIM12_BASE + TIM_DIER_S,__READ_WRITE ,__tim9_dier_bits);
__IO_REG32_BIT(TIM12_SR,          TIM12_BASE + TIM_SR_S,__READ_WRITE ,__tim9_sr_bits);
__IO_REG32_BIT(TIM12_EGR,         TIM12_BASE + TIM_EGR_S,__READ_WRITE ,__tim9_egr_bits);
__IO_REG32_BIT(TIM12_CCMR1,       TIM12_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim9_ccmr1_bits);
#define TIM12_OCMR1      TIM12_CCMR1
#define TIM12_OCMR1_BIT  TIM12_CCMR1_BIT
__IO_REG32_BIT(TIM12_CCER,        TIM12_BASE + TIM_CCER_S,__READ_WRITE ,__tim9_ccer_bits);
__IO_REG32_BIT(TIM12_CNT,         TIM12_BASE + TIM_CNT_S,__READ_WRITE ,__tim9_cnt_bits);
__IO_REG32_BIT(TIM12_PSC,         TIM12_BASE + TIM_PSC_S,__READ_WRITE ,__tim9_psc_bits);
__IO_REG32_BIT(TIM12_ARR,         TIM12_BASE + TIM_ARR_S,__READ_WRITE ,__tim9_arr_bits);
__IO_REG32_BIT(TIM12_CCR1,        TIM12_BASE + TIM_CCR1_S,__READ_WRITE ,__tim9_ccr_bits);
__IO_REG32_BIT(TIM12_CCR2,        TIM12_BASE + TIM_CCR2_S,__READ_WRITE ,__tim9_ccr_bits);

/***************************************************************************
 **
 ** TIM10
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM10_CR1,         TIM10_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM10_DIER,        TIM10_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM10_SR,          TIM10_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM10_EGR,         TIM10_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM10_CCMR1,       TIM10_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM10_OCMR1      TIM10_CCMR1
#define TIM10_OCMR1_BIT  TIM10_CCMR1_BIT
__IO_REG32_BIT(TIM10_CCER,        TIM10_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM10_CNT,         TIM10_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM10_PSC,         TIM10_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM10_ARR,         TIM10_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM10_CCR1,        TIM10_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);

/***************************************************************************
 **
 ** TIM11
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM11_CR1,         TIM11_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM11_DIER,        TIM11_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM11_SR,          TIM11_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM11_EGR,         TIM11_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM11_CCMR1,       TIM11_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM11_OCMR1      TIM11_CCMR1
#define TIM11_OCMR1_BIT  TIM11_CCMR1_BIT
__IO_REG32_BIT(TIM11_CCER,        TIM11_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM11_CNT,         TIM11_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM11_PSC,         TIM11_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM11_ARR,         TIM11_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM11_CCR1,        TIM11_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);
__IO_REG32_BIT(TIM11_OR,          TIM11_BASE + TIM_OR_S,__READ_WRITE ,__tim11_or_bits);

/***************************************************************************
 **
 ** TIM13
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM13_CR1,         TIM13_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM13_DIER,        TIM13_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM13_SR,          TIM13_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM13_EGR,         TIM13_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM13_CCMR1,       TIM13_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM13_OCMR1      TIM13_CCMR1
#define TIM13_OCMR1_BIT  TIM13_CCMR1_BIT
__IO_REG32_BIT(TIM13_CCER,        TIM13_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM13_CNT,         TIM13_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM13_PSC,         TIM13_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM13_ARR,         TIM13_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM13_CCR1,        TIM13_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);

/***************************************************************************
 **
 ** TIM14
 **
 ***************************************************************************/
__IO_REG32_BIT(TIM14_CR1,         TIM14_BASE + TIM_CR1_S,__READ_WRITE ,__tim10_cr1_bits);
__IO_REG32_BIT(TIM14_DIER,        TIM14_BASE + TIM_DIER_S,__READ_WRITE ,__tim10_dier_bits);
__IO_REG32_BIT(TIM14_SR,          TIM14_BASE + TIM_SR_S,__READ_WRITE ,__tim10_sr_bits);
__IO_REG32_BIT(TIM14_EGR,         TIM14_BASE + TIM_EGR_S,__READ_WRITE ,__tim10_egr_bits);
__IO_REG32_BIT(TIM14_CCMR1,       TIM14_BASE + TIM_CCMR1_S,__READ_WRITE ,__tim10_ccmr1_bits);
#define TIM14_OCMR1      TIM14_CCMR1
#define TIM14_OCMR1_BIT  TIM14_CCMR1_BIT
__IO_REG32_BIT(TIM14_CCER,        TIM14_BASE + TIM_CCER_S,__READ_WRITE ,__tim10_ccer_bits);
__IO_REG32_BIT(TIM14_CNT,         TIM14_BASE + TIM_CNT_S,__READ_WRITE ,__tim10_cnt_bits);
__IO_REG32_BIT(TIM14_PSC,         TIM14_BASE + TIM_PSC_S,__READ_WRITE ,__tim10_psc_bits);
__IO_REG32_BIT(TIM14_ARR,         TIM14_BASE + TIM_ARR_S,__READ_WRITE ,__tim10_arr_bits);
__IO_REG32_BIT(TIM14_CCR1,        TIM14_BASE + TIM_CCR1_S,__READ_WRITE ,__tim10_ccr_bits);

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

__IO_REG32_BIT(FLASH_ACR, FLASH_INTERFACE_BASE + FLASH_ACR_S,__READ_WRITE ,__flash_acr_bits);

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
__IO_REG32_BIT(SPI1_CR1,          SPI1_BASE + SPI_CR1_S,__READ_WRITE ,__spi_cr1_bits);
__IO_REG32_BIT(SPI1_CR2,          SPI1_BASE + SPI_CR2_S,__READ_WRITE ,__spi_cr2_bits);
__IO_REG32_BIT(SPI1_SR,           SPI1_BASE + SPI_SR_S,__READ_WRITE ,__spi_sr_bits);
__IO_REG32_BIT(SPI1_DR,           SPI1_BASE + SPI_DR_S,__READ_WRITE ,__spi_dr_bits);
__IO_REG32_BIT(SPI1_CRCPR,        SPI1_BASE + SPI_CRCPR_S,__READ_WRITE ,__spi_crcpr_bits);
__IO_REG32_BIT(SPI1_RXCRCR,       SPI1_BASE + SPI_RXCRCR_S,__READ       ,__spi_rxcrcr_bits);
__IO_REG32_BIT(SPI1_TXCRCR,       SPI1_BASE + SPI_TXCRCR_S,__READ       ,__spi_txcrcr_bits);

/***************************************************************************
 **
 ** SPI2
 **
 ***************************************************************************/
__IO_REG32_BIT(SPI2_CR1,          SPI2_I2S2_BASE + SPI_CR1_S,__READ_WRITE ,__spi_cr1_bits);
__IO_REG32_BIT(SPI2_CR2,          SPI2_I2S2_BASE + SPI_CR2_S,__READ_WRITE ,__spi_cr2_bits);
__IO_REG32_BIT(SPI2_SR,           SPI2_I2S2_BASE + SPI_SR_S,__READ_WRITE ,__spi_sr_bits);
__IO_REG32_BIT(SPI2_DR,           SPI2_I2S2_BASE + SPI_DR_S,__READ_WRITE ,__spi_dr_bits);
__IO_REG32_BIT(SPI2_CRCPR,        SPI2_I2S2_BASE + SPI_CRCPR_S,__READ_WRITE ,__spi_crcpr_bits);
__IO_REG32_BIT(SPI2_RXCRCR,       SPI2_I2S2_BASE + SPI_RXCRCR_S,__READ       ,__spi_rxcrcr_bits);
__IO_REG32_BIT(SPI2_TXCRCR,       SPI2_I2S2_BASE + SPI_TXCRCR_S,__READ       ,__spi_txcrcr_bits);
__IO_REG32_BIT(SPI2_I2SCFGR,      SPI2_I2S2_BASE + SPI_I2SCFGR_S,__READ_WRITE ,__spi_i2scfgr_bits);
__IO_REG32_BIT(SPI2_I2SPR,        SPI2_I2S2_BASE + SPI_I2SPR_S,__READ_WRITE ,__spi_i2spr_bits);

/***************************************************************************
 **
 ** SPI3
 **
 ***************************************************************************/
__IO_REG32_BIT(SPI3_CR1,          SPI3_I2S3_BASE + SPI_CR1_S,__READ_WRITE ,__spi_cr1_bits);
__IO_REG32_BIT(SPI3_CR2,          SPI3_I2S3_BASE + SPI_CR2_S,__READ_WRITE ,__spi_cr2_bits);
__IO_REG32_BIT(SPI3_SR,           SPI3_I2S3_BASE + SPI_SR_S,__READ_WRITE ,__spi_sr_bits);
__IO_REG32_BIT(SPI3_DR,           SPI3_I2S3_BASE + SPI_DR_S,__READ_WRITE ,__spi_dr_bits);
__IO_REG32_BIT(SPI3_CRCPR,        SPI3_I2S3_BASE + SPI_CRCPR_S,__READ_WRITE ,__spi_crcpr_bits);
__IO_REG32_BIT(SPI3_RXCRCR,       SPI3_I2S3_BASE + SPI_RXCRCR_S,__READ       ,__spi_rxcrcr_bits);
__IO_REG32_BIT(SPI3_TXCRCR,       SPI3_I2S3_BASE + SPI_TXCRCR_S,__READ       ,__spi_txcrcr_bits);
__IO_REG32_BIT(SPI3_I2SCFGR,      SPI3_I2S3_BASE + SPI_TXCRCR_S,__READ_WRITE ,__spi_i2scfgr_bits);
__IO_REG32_BIT(SPI3_I2SPR,        SPI3_I2S3_BASE + SPI_I2SPR_S,__READ_WRITE ,__spi_i2spr_bits);

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

__IO_REG32_BIT(USART1_SR, USART1_BASE + USART_SR_S,__READ_WRITE ,__usart_sr_bits);
__IO_REG32_BIT(USART1_DR, USART1_BASE + USART_DR_S,__READ_WRITE ,__usart_dr_bits);
__IO_REG32_BIT(USART1_BRR, USART1_BASE + USART_BRR_S,__READ_WRITE ,__usart_brr_bits);
__IO_REG32_BIT(USART1_CR1, USART1_BASE + USART_CR1_S,__READ_WRITE ,__usart_cr1_bits);
__IO_REG32_BIT(USART1_CR2, USART1_BASE + USART_CR2_S,__READ_WRITE ,__usart_cr2_bits);
__IO_REG32_BIT(USART1_CR3, USART1_BASE + USART_CR3_S,__READ_WRITE ,__usart_cr3_bits);
__IO_REG32_BIT(USART1_GTPR, USART1_BASE + USART_GTPR_S,__READ_WRITE ,__usart_gtpr_bits);

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
__IO_REG32_BIT(ADC1_SR,           ADC1_BASE + ADC1_SR_S,__READ_WRITE ,__adc_sr_bits);
__IO_REG32_BIT(ADC1_CR1,          ADC1_BASE + ADC1_CR1_S,__READ_WRITE ,__adc_cr1_bits);
__IO_REG32_BIT(ADC1_CR2,          ADC1_BASE + ADC1_CR2_S,__READ_WRITE ,__adc_cr2_bits);
__IO_REG32_BIT(ADC1_SMPR1,        ADC1_BASE + ADC1_SMPR1_S,__READ_WRITE ,__adc_smpr1_bits);
__IO_REG32_BIT(ADC1_SMPR2,        ADC1_BASE + ADC1_SMPR2_S,__READ_WRITE ,__adc_smpr2_bits);
__IO_REG32_BIT(ADC1_JOFR1,        ADC1_BASE + ADC1_JOFR1_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_JOFR2,        ADC1_BASE + ADC1_JOFR2_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_JOFR3,        ADC1_BASE + ADC1_JOFR3_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_JOFR4,        ADC1_BASE + ADC1_JOFR4_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC1_HTR,          ADC1_BASE + ADC1_HTR_S,__READ_WRITE ,__adc_htr_bits);
__IO_REG32_BIT(ADC1_LTR,          ADC1_BASE + ADC1_LTR_S,__READ_WRITE ,__adc_ltr_bits);
__IO_REG32_BIT(ADC1_SQR1,         ADC1_BASE + ADC1_SQR1_S,__READ_WRITE ,__adc_sqr1_bits);
__IO_REG32_BIT(ADC1_SQR2,         ADC1_BASE + ADC1_SQR2_S,__READ_WRITE ,__adc_sqr2_bits);
__IO_REG32_BIT(ADC1_SQR3,         ADC1_BASE + ADC1_SQR3_S,__READ_WRITE ,__adc_sqr3_bits);
__IO_REG32_BIT(ADC1_JSQR,         ADC1_BASE + ADC1_JSQR_S,__READ_WRITE ,__adc_jsqr_bits);
__IO_REG32_BIT(ADC1_JDR1,         ADC1_BASE + ADC1_JDR1_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_JDR2,         ADC1_BASE + ADC1_JDR2_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_JDR3,         ADC1_BASE + ADC1_JDR3_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_JDR4,         ADC1_BASE + ADC1_JDR4_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC1_DR,           ADC1_BASE + ADC1_DR_S,__READ       ,__adc_dr_bits);

/***************************************************************************
 **
 ** ADC2
 **
 ***************************************************************************/
__IO_REG32_BIT(ADC2_SR,           ADC2_BASE + ADC1_SR_S,__READ_WRITE ,__adc_sr_bits);
__IO_REG32_BIT(ADC2_CR1,          ADC2_BASE + ADC1_CR1_S,__READ_WRITE ,__adc_cr1_bits);
__IO_REG32_BIT(ADC2_CR2,          ADC2_BASE + ADC1_CR2_S,__READ_WRITE ,__adc_cr2_bits);
__IO_REG32_BIT(ADC2_SMPR1,        ADC2_BASE + ADC1_SMPR1_S,__READ_WRITE ,__adc_smpr1_bits);
__IO_REG32_BIT(ADC2_SMPR2,        ADC2_BASE + ADC1_SMPR2_S,__READ_WRITE ,__adc_smpr2_bits);
__IO_REG32_BIT(ADC2_JOFR1,        ADC2_BASE + ADC1_JOFR1_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_JOFR2,        ADC2_BASE + ADC1_JOFR2_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_JOFR3,        ADC2_BASE + ADC1_JOFR3_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_JOFR4,        ADC2_BASE + ADC1_JOFR4_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC2_HTR,          ADC2_BASE + ADC1_HTR_S,__READ_WRITE ,__adc_htr_bits);
__IO_REG32_BIT(ADC2_LTR,          ADC2_BASE + ADC1_LTR_S,__READ_WRITE ,__adc_ltr_bits);
__IO_REG32_BIT(ADC2_SQR1,         ADC2_BASE + ADC1_SQR1_S,__READ_WRITE ,__adc_sqr1_bits);
__IO_REG32_BIT(ADC2_SQR2,         ADC2_BASE + ADC1_SQR2_S,__READ_WRITE ,__adc_sqr2_bits);
__IO_REG32_BIT(ADC2_SQR3,         ADC2_BASE + ADC1_SQR3_S,__READ_WRITE ,__adc_sqr3_bits);
__IO_REG32_BIT(ADC2_JSQR,         ADC2_BASE + ADC1_JSQR_S,__READ_WRITE ,__adc_jsqr_bits);
__IO_REG32_BIT(ADC2_JDR1,         ADC2_BASE + ADC1_JDR1_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_JDR2,         ADC2_BASE + ADC1_JDR2_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_JDR3,         ADC2_BASE + ADC1_JDR3_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_JDR4,         ADC2_BASE + ADC1_JDR4_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC2_DR,           ADC2_BASE + ADC1_DR_S,__READ       ,__adc_dr_bits);

/***************************************************************************
 **
 ** ADC3
 **
 ***************************************************************************/
__IO_REG32_BIT(ADC3_SR,           ADC3_BASE + ADC1_SR_S,__READ_WRITE ,__adc_sr_bits);
__IO_REG32_BIT(ADC3_CR1,          ADC3_BASE + ADC1_CR1_S,__READ_WRITE ,__adc_cr1_bits);
__IO_REG32_BIT(ADC3_CR2,          ADC3_BASE + ADC1_CR2_S,__READ_WRITE ,__adc_cr2_bits);
__IO_REG32_BIT(ADC3_SMPR1,        ADC3_BASE + ADC1_SMPR1_S,__READ_WRITE ,__adc_smpr1_bits);
__IO_REG32_BIT(ADC3_SMPR2,        ADC3_BASE + ADC1_SMPR2_S,__READ_WRITE ,__adc_smpr2_bits);
__IO_REG32_BIT(ADC3_JOFR1,        ADC3_BASE + ADC1_JOFR1_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_JOFR2,        ADC3_BASE + ADC1_JOFR2_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_JOFR3,        ADC3_BASE + ADC1_JOFR3_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_JOFR4,        ADC3_BASE + ADC1_JOFR4_S,__READ_WRITE ,__adc_jofr_bits);
__IO_REG32_BIT(ADC3_HTR,          ADC3_BASE + ADC1_HTR_S,__READ_WRITE ,__adc_htr_bits);
__IO_REG32_BIT(ADC3_LTR,          ADC3_BASE + ADC1_LTR_S,__READ_WRITE ,__adc_ltr_bits);
__IO_REG32_BIT(ADC3_SQR1,         ADC3_BASE + ADC1_SQR1_S,__READ_WRITE ,__adc_sqr1_bits);
__IO_REG32_BIT(ADC3_SQR2,         ADC3_BASE + ADC1_SQR2_S,__READ_WRITE ,__adc_sqr2_bits);
__IO_REG32_BIT(ADC3_SQR3,         ADC3_BASE + ADC1_JDR3_S,__READ_WRITE ,__adc_sqr3_bits);
__IO_REG32_BIT(ADC3_JSQR,         ADC3_BASE + ADC1_JSQR_S,__READ_WRITE ,__adc_jsqr_bits);
__IO_REG32_BIT(ADC3_JDR1,         ADC3_BASE + ADC1_JDR1_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_JDR2,         ADC3_BASE + ADC1_JDR2_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_JDR3,         ADC3_BASE + ADC1_JDR3_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_JDR4,         ADC3_BASE + ADC1_JDR4_S,__READ       ,__adc_jdr_bits);
__IO_REG32_BIT(ADC3_DR,           ADC3_BASE + ADC1_DR_S,__READ       ,__adc_dr_bits);

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

#endif  //  IRS_STM32F2xx

#endif // armregs_stm32f2xxH
