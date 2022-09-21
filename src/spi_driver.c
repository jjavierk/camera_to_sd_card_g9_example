#include "spi_driver.h"

static int enable_3v3_periph(void)
{
    pi_device_t gpio_3v3_periph;
    struct pi_gpio_conf gpio_conf;

    pi_gpio_conf_init(&gpio_conf);

    gpio_conf.port = PAD_GPIO_3V3_PERIPH / 32;

    pi_open_from_conf(&gpio_3v3_periph, &gpio_conf);

    if (pi_gpio_open(&gpio_3v3_periph))
    {
        return -1;
    }

    /* set pad to gpio mode */
    pi_pad_set_function(PAD_GPIO_3V3_PERIPH, PI_PAD_FUNC1);

    /* configure gpio output */
    pi_gpio_flags_e flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_3v3_periph, PAD_GPIO_3V3_PERIPH, flags);

    pi_gpio_pin_write(&gpio_3v3_periph, PAD_GPIO_3V3_PERIPH, 1);

    /* wait some time to let voltage rise */
    pi_time_wait_us(20000);

    return 0;
}

pi_device_t gpio_cs_periph;

static int enable_cs_custom(void)
{

    struct pi_gpio_conf gpio_conf;

    pi_gpio_conf_init(&gpio_conf);

    gpio_conf.port = PAD_GPIO_CS_CUSTOM / 32;

    pi_open_from_conf(&gpio_cs_periph, &gpio_conf);

    if (pi_gpio_open(&gpio_cs_periph))
    {
        return -1;
    }

    /* set pad to gpio mode */
    pi_pad_set_function(PAD_GPIO_CS_CUSTOM, PI_PAD_FUNC1);

    /* configure gpio output */
    pi_gpio_flags_e flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_cs_periph, PAD_GPIO_CS_CUSTOM, flags);

    pi_gpio_pin_write(&gpio_cs_periph, PAD_GPIO_CS_CUSTOM, 1);

    /* wait some time to let voltage rise */
    pi_time_wait_us(20000);

    return 0;
}

static pi_device_t spi;

uint8_t spi_init(int baudrate, uint8_t polarity, uint8_t phase)
{
    /* set spi pads to function 0 */
    pi_pad_set_function(PI_PAD_034, PI_PAD_FUNC0); // CS
    pi_pad_set_function(PI_PAD_038, PI_PAD_FUNC0); // MOSI
    pi_pad_set_function(PI_PAD_039, PI_PAD_FUNC0); // MISO


    /* open SPI interface */
    struct pi_spi_conf conf;

    pi_spi_conf_init(&conf);
    conf.wordsize = PI_SPI_WORDSIZE_8;
    conf.big_endian = 1;
    conf.max_baudrate = baudrate;
    conf.polarity = polarity;
    conf.phase = phase;
    conf.itf = 1;
    conf.cs = 0; /* Mikroe connector SPI is enabled when CS = 0 */

    pi_open_from_conf(&spi, &conf);

    if (pi_spi_open(&spi))
    {
        printf("Failed to open SPI\n");
        return -1;
    }

    /* Enable support for Mikroe */
    enable_3v3_periph();
    enable_cs_custom();

    return 0;
}

void spi_transfer(uint8_t *tx_buffer_local, uint8_t *rx_buffer_local, int trx_size)
{
    pi_spi_transfer(&spi, tx_buffer_local, rx_buffer_local, trx_size * 8, 0);
}

void spi_send(uint8_t *tx_buffer_local, int tx_size)
{
    pi_spi_send(&spi, tx_buffer_local, tx_size * 8, 0);
}

void spi_receive( uint8_t *rx_buffer_local, int rx_size)
{
    pi_spi_receive(&spi, rx_buffer_local, rx_size * 8, 0);
}


void spi_cs_high()
{
    pi_gpio_pin_write(&gpio_cs_periph, PAD_GPIO_CS_CUSTOM, 1);
}

void spi_cs_low()
{
    pi_gpio_pin_write(&gpio_cs_periph, PAD_GPIO_CS_CUSTOM, 0);
}