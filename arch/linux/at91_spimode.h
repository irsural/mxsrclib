#ifndef AT91_SPIMODEH
#define AT91_SPIMODEH

#define SPI_CS0 0
#define SPI_CS1 1
#define SPI_CS2 2
#define SPI_CS3 3
                              
struct spimode_t {

    unsigned char CPOL;
    unsigned char NCPHA;
    unsigned char Bits_per_trans;
    unsigned char SPI_BRate;
    unsigned char SPI_DBSPCK;
    unsigned char SPI_DBTransfers;
};

#endif
