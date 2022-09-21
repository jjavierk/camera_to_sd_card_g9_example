#include "spi_driver.h"
#include "delay.h"
#include "ff.h"		/* Declarations of FatFs API */
#include <stdio.h>
#include <string.h>

uint8_t sd_create_file();
uint8_t save_sdcard(uint8_t *image, int size);
uint8_t test_sd_card();
int sd_write(uint8_t *data, int data_size);
void sd_close();