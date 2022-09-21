#ifndef SPI_DRV_
#define SPI_DRV_

#include "pmsis.h"

#define PAD_GPIO_3V3_PERIPH (PI_PAD_040)
#define PAD_GPIO_CS_CUSTOM (PI_PAD_044)

typedef enum
{
    CPOL_LOW = 0,
    CPOL_HIGH
} spi_cpol;

typedef enum
{
    CPHA_LOW = 0,
    CPHA_HIGH
} spi_cpha;

uint8_t spi_init(int baudrate, uint8_t polarity, uint8_t phase);
void spi_transfer(uint8_t *tx_buffer_local, uint8_t *rx_buffer_local, int trx_size);
void spi_send(uint8_t *tx_buffer_local, int tx_size);
void spi_receive( uint8_t *rx_buffer_local, int rx_size);

void spi_cs_high();
void spi_cs_low(); 

#endif