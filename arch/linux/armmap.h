#ifndef GPIO_H
#define GPIO_H

//local PIO definition
#define PIOA_OFFSET   0x400
#define PIOB_OFFSET   0x600
#define PIOC_OFFSET   0x800
#define PIOD_OFFSET   0xa00

//peripheral identifiers
#define AIC_ID        0
#define SYSIRQ_ID     1
#define PIOA_ID       2
#define PIOB_ID       3
#define PIOC_ID       4
#define PIOD_ID       5
#define US0_ID        6
#define US1_ID        7
#define US2_ID        8
#define US3_ID        9
#define MCI_ID       10
#define UDP_ID       11
#define TWI_ID       12
#define SPI_ID       13
#define TC0_ID       17

#define DBGU_OFFSET 0x200
#define PMC_OFFSET  0xC00
#define ST_OFFSET   0xD00



/* Debug constants */
#define DEBUG_FRAME   0x0001
#define DEBUG_STATUS  0x0002
#define DEBUG_FIELD   0x0004
#define DEBUG_ERR     0x0008
#define DEBUG_TIME    0x0010
#define DEBUG_INIT    0x0020

/* Debug settings */
#define DEBUG
#define DEBUG_ALL     (DEBUG_STATUS | DEBUG_ERR)
#define DEBUG_LEVEL   DEBUG_ALL

/* Memory mapping definitions */
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)
#define MAP_16_SIZE 16384UL
#define MAP_16_MASK (MAP_16_SIZE - 1)


/* AT91RM9200 PIO definitions */
#define AT91_SYS  0xFFFFF000
#define AT91_PIOA 0xFFFFF400
#define AT91_PIOB 0xFFFFF600
#define AT91_PIOC 0xFFFFF800
#define AT91_PIOD 0xFFFFFA00
#define AT91_SPI  0xFFFE0000
#define AT91_TC0  0xFFFA0000
#define AT91_DBGU 0xFFFFF200

#define PMC_PCER  0x0010
#define PMC_PCSR  0x0018
#define PMC_MCKR  0x0030
#define PMC_SR 	  0x0068

#define PIO_PER   0x0000
#define PIO_PDR   0x0004
#define PIO_PSR   0x0008
#define PIO_OER   0x0010
#define PIO_ODR   0x0014
#define PIO_OSR   0x0018

#define PIO_IFER  0x0020
#define PIO_IFDR  0x0024
#define PIO_IFSR  0x0028

#define PIO_SODR  0x0030
#define PIO_CODR  0x0034
#define PIO_ODSR  0x0038
#define PIO_PDSR  0x003C

#define PIO_IER   0x0040
#define PIO_IDR   0x0044
#define PIO_IMR   0x0048
#define PIO_ISR   0x004C
#define PIO_MDER  0x0050
#define PIO_MDDR  0x0054
#define PIO_MDSR  0x0058
#define PIO_PUDR  0x0060
#define PIO_PUER  0x0064
#define PIO_PUSR  0x0068
#define PIO_ASR   0x0070
#define PIO_BSR   0x0074
#define PIO_ABSR  0x0078
#define PIO_OWER  0x00A0
#define PIO_OWDR  0x00A4
#define PIO_OWSR  0x00A8

/* AT91RM9200 SPI definitions */
#define SPI_CR    0x00
#define SPI_MR    0x04
#define SPI_RDR   0x08
#define SPI_TDR   0x0C
#define SPI_SR    0x10
#define SPI_IER   0x14
#define SPI_IDR   0x18
#define SPI_IMR   0x1C
#define SPI_CSR0  0x30
#define SPI_CSR1  0x34
#define SPI_CSR2  0x38
#define SPI_CSR3  0x3C

/* Offsets from AT91C_BASE_SYS */
#define AIC_SMR   (0) // Source Mode Register
#define AIC_SVR   (128) // Source Vector Register
#define AIC_IVR   (256) // IRQ Vector Register
#define AIC_FVR   (260) // FIQ Vector Register
#define AIC_ISR   (264) // Interrupt Status Register
#define AIC_IPR   (268) // Interrupt Pending Register
#define AIC_IMR   (272) // Interrupt Mask Register
#define AIC_CISR  (276) // Core Interrupt Status Register
#define AIC_IECR  (288) // Interrupt Enable Command Register
#define AIC_IDCR  (292) // Interrupt Disable Command Register
#define AIC_ICCR  (296) // Interrupt Clear Command Register
#define AIC_ISCR  (300) // Interrupt Set Command Register
#define AIC_EOICR (304) // End of Interrupt Command Register
#define AIC_SPU   (308) // Spurious Vector Register
#define AIC_DCR   (312) // Debug Control Register (Protect)
#define AIC_FFER  (320) // Fast Forcing Enable Register
#define AIC_FFDR  (324) // Fast Forcing Disable Register
#define AIC_FFSR  (328) // Fast Forcing Status Register

/* Offset for TC0*/
#define TC_CCR    0x00  //Chanel control registr
#define TC_CMR    0x04  //Chanel mode registr
#define TC_CV     0x10  //Timer counter value
#define TC_RA     0x14  //Registr A
#define TC_RB     0x18  //Registr B
#define TC_RC     0x1C  //Registr C
#define TC_SR     0x20  //Status registr
#define TC_IER    0x24  //interrupt enable registr
#define TC_IDR    0x28  //interrupt disable registr
#define TC_IMR    0x2C  //interrupt mask registr

/*Offset for System timer*/
#define ST_CRTR    0x0024  //

/*Offset for DBGU*/
#define DBGU_SR    0x0014
#define DBGU_BRGR  0x0020
#define DBGU_CIDR  0x0040

#endif
