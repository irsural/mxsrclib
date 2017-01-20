#ifndef armregs_stm32f7xxH
#define armregs_stm32f7xxH

#include <irsdefs.h>

#include <armiomacros.h>

#include <irsfinal.h>

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
#define IRS_PORTK_BASE            0x40022800
#define IRS_PORTJ_BASE            0x40022400
#define IRS_PORTI_BASE            0x40022000
#define IRS_PORTH_BASE            0x40021C00
#define IRS_PORTG_BASE            0x40021800
#define IRS_PORTF_BASE            0x40021400
#define IRS_PORTE_BASE            0x40021000
#define IRS_PORTD_BASE            0x40020C00
#define IRS_PORTC_BASE            0x40020800
#define IRS_PORTB_BASE            0x40020400
#define IRS_PORTA_BASE            0x40020000
#define IRS_LCD_TFT_BASE          0x40016800
#define IRS_SAI2_BASE             0x40015C00
#define IRS_SAI1_BASE             0x40015800
#define IRS_SPI6_BASE             0x40015400
#define IRS_SPI5_BASE             0x40015000
#define IRS_TIM11_BASE            0x40014800
#define IRS_TIM10_BASE            0x40014400
#define IRS_TIM9_BASE             0x40014000
#define IRS_EXTI_BASE             0x40013C00
#define IRS_SYSCFG_BASE           0x40013800
#define IRS_SPI4_BASE             0x40013400
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

#ifdef IRS_STM32F7xx

#define UNIQUE_DEVICE_ID_BEGIN (reinterpret_cast<const irs_u8*>(0x1FF0F420))
#define UNIQUE_DEVICE_ID_SIZE 12


/******************************************************************************/
/*                                                                            */
/*      Universal Synchronous Asynchronous Receiver Transmitter (USART)       */
/*                                                                            */
/******************************************************************************/

#define USART_CR1_S 0x0
#define USART_CR2_S 0x04
#define USART_CR3_S 0x08
#define USART_BRR_S 0x0C
#define USART_GTPR_S 0x10
#define USART_ISR_S 0x1C
#define USART_RDR_S 0x24
#define USART_TDR_S 0x28

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
  __REG32 LBDF            : 1;
  __REG32 CTSIF           : 1;
  __REG32 CTS             : 1;
  __REG32 RTOF            : 1;
  __REG32 EOBF            : 1;
  __REG32                 : 1;
  __REG32 ABRE            : 1;
  __REG32 ABRF            : 1;
  __REG32 BUSY            : 1;
  __REG32 CMF             : 1;
  __REG32 SBKF            : 1;
  __REG32                 : 1;
  __REG32                 : 1;
  __REG32 TEACK           : 1;
  __REG32                 :10;
} __usart_isr_bits;

/* Receive data register (USART_TDR) */
typedef struct {
  __REG32 RDR             : 9;
  __REG32                 :23;
} __usart_rdr_bits;

/* Transmit data register (USART_TDR) */
typedef struct {
  __REG32 TDR             : 9;
  __REG32                 :23;
} __usart_tdr_bits;

/* Baud rate register (USART_BRR) */
typedef struct {
  __REG32 DIV_Fraction    : 4;
  __REG32 DIV_Mantissa    :12;
  __REG32                 :16;
} __usart_brr_bits;

/* Control register 1 (USART_CR1) */
typedef struct {
  __REG32 UE              : 1;
  __REG32                 : 1;
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
  __REG32 M0              : 1;
  __REG32 MME             : 1;
  __REG32 CMIE            : 1;
  __REG32 OVER8           : 1;
  __REG32 DEDT            : 5;
  __REG32 DEAT            : 5;
  __REG32 RTOIE           : 1;
  __REG32 EOBIE           : 1;
  __REG32 M1              : 1;
  __REG32                 : 3;
} __usart_cr1_bits;

/* Control register 2 (USART_CR2) */
typedef struct {
  __REG32                 : 1;
  __REG32                 : 1;
  __REG32                 : 1;
  __REG32                 : 1;
  __REG32 ADDM7           : 1;
  __REG32 LBDL            : 1;
  __REG32 LBDIE           : 1;
  __REG32                 : 1;
  __REG32 LBCL            : 1;
  __REG32 CPHA            : 1;
  __REG32 CPOL            : 1;
  __REG32 CLKEN           : 1;
  __REG32 STOP            : 2;
  __REG32 LINEN           : 1;
  __REG32 SWAP            : 1;
  __REG32 RXINV           : 1;
  __REG32 TXINV           : 1;
  __REG32 DATAINV         : 1;
  __REG32 MSBFIRST        : 1;
  __REG32 ABREN           : 1;
  __REG32 ABRMOD          : 2;
  __REG32 RTOEN           : 1;
  __REG32 ADD             : 8;
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
  __REG32 ONEBIT          : 1;
  __REG32 OVRDIS          : 1;
  __REG32 DDRE            : 1;
  __REG32 DEM             : 1;
  __REG32 DEP             : 1;
  __REG32                 : 1;
  __REG32 SCARCNT         : 3;
  __REG32                 :12;
} __usart_cr3_bits;

/* Guard time and prescaler register (USART_GTPR) */
typedef struct {
  __REG32 PSC             : 8;
  __REG32 GT              : 8;
  __REG32                 :16;
} __usart_gtpr_bits;


__IO_REG32_BIT(USART1_CR1, IRS_USART1_BASE + USART_CR1_S,__READ_WRITE ,__usart_cr1_bits);
__IO_REG32_BIT(USART1_CR2, IRS_USART1_BASE + USART_CR2_S,__READ_WRITE ,__usart_cr2_bits);
__IO_REG32_BIT(USART1_CR3, IRS_USART1_BASE + USART_CR3_S,__READ_WRITE ,__usart_cr3_bits);
__IO_REG32_BIT(USART1_ISR, IRS_USART1_BASE + USART_ISR_S,__READ_WRITE ,__usart_isr_bits);
__IO_REG32_BIT(USART1_RDR, IRS_USART1_BASE + USART_RDR_S,__READ_WRITE ,__usart_rdr_bits);
__IO_REG32_BIT(USART1_TDR, IRS_USART1_BASE + USART_TDR_S,__READ_WRITE ,__usart_tdr_bits);
__IO_REG32_BIT(USART1_BRR, IRS_USART1_BASE + USART_BRR_S,__READ_WRITE ,__usart_brr_bits);

__IO_REG32_BIT(USART1_GTPR, IRS_USART1_BASE + USART_GTPR_S,__READ_WRITE ,__usart_gtpr_bits);

struct usart_regs_t
{
  IRS_IO_REG32_BIT(USART_CR1, __READ_WRITE ,__usart_cr1_bits);
  IRS_IO_REG32_BIT(USART_CR2, __READ_WRITE ,__usart_cr2_bits);
  IRS_IO_REG32_BIT(USART_CR3, __READ_WRITE ,__usart_cr3_bits);
  IRS_IO_REG32_BIT(USART_BRR, __READ_WRITE ,__usart_brr_bits);
  IRS_IO_REG32_BIT(USART_GTPR, __READ_WRITE ,__usart_gtpr_bits);
  uint32_t none1;
  uint32_t none2;
  IRS_IO_REG32_BIT(USART_ISR, __READ_WRITE, __usart_isr_bits);
  uint32_t none3;
  IRS_IO_REG32_BIT(USART_RDR, __READ_WRITE, __usart_rdr_bits);
  IRS_IO_REG32_BIT(USART_TDR, __READ_WRITE, __usart_tdr_bits);
};

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface (SPI)                   */
/*                                                                            */
/******************************************************************************/

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
  __REG32 CRCL            : 1;
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
  __REG32 NSSP            : 1;
  __REG32 FRF             : 1;
  __REG32 ERRIE           : 1;
  __REG32 RXNEIE          : 1;
  __REG32 TXEIE           : 1;
  __REG32 DS              : 4;
  __REG32 FRXTH           : 1;
  __REG32 LDMARX          : 1;
  __REG32 LDMATX          : 1;
  __REG32                 :17;
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
  __REG32 FRE             : 1;
  __REG32 FRLVL           : 2;
  __REG32 FTLVL           : 2;
  __REG32                 :19;
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
  __REG32 ASTRTEN         : 1;
  __REG32                 :19;
} __spi_i2scfgr_bits;

/* SPI_I2S Prescaler register (SPI_I2SPR) */
typedef struct {
  __REG32 I2SDIV          : 8;
  __REG32 ODD             : 1;
  __REG32 MCKOE           : 1;
  __REG32                 :22;
} __spi_i2spr_bits;

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

#endif // IRS_STM32F7xx

#endif // armregs_stm32f7xxH
