#include "sd_card_driver.h"
// #define TEST_DATA_SIZE 1024*200 //100kB
// uint8_t test_data [TEST_DATA_SIZE];

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */
char t_filename[] = "F000.txt";

UINT bw;
FRESULT fr;
#define DATASET_TEST_SIZE 1024
uint8_t data[DATASET_TEST_SIZE] = {0};


uint8_t test_sd_card()
{
	memset(data, 'a', DATASET_TEST_SIZE);
    sd_create_file();
    sd_write(data, DATASET_TEST_SIZE);
    sd_close();
    return 0;
}

uint8_t sd_create_file()
{
    //printf("Creating File on SD card...\r\n");

	fr = f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	if (fr == FR_OK)
	{
		int filename_found = FR_OK;
		int f_counter = 0;
		FILINFO fno;

		while(filename_found == FR_OK)
		{
			sprintf(t_filename, "Out%d.ppm", f_counter++);
			filename_found = f_stat(t_filename ,&fno);
		}
		delay_ms(100);

		fr = f_open(&Fil, t_filename, FA_WRITE | FA_CREATE_ALWAYS);

		if( fr != FR_OK)
		{
			printf("Error in f_open() with value %d \r\n", fr);
			while(1);
		}
		else
        {
            //printf("File created, name = %s\r\n", t_filename);
        }
	}
	else
	{
		printf("Error in f_mount() with value %d \r\n", fr);
		while(1);
	}
}

int sd_write(uint8_t *data, int data_size)
{
 
    // if(Fil == NULL)
    // {
    //     printf("First open a file...\r");
    //     return;
    // }
    
    //int start_time  = delay_get_system_time();
    fr = f_write(&Fil, data, data_size, &bw);				/* Write data to the file */
    //int elapsed_time = (delay_get_system_time() - start_time)/1000;
    //float throughput = (float)data_size/((float)elapsed_time/1000);
    //printf("Test successfull, elapsed time %d, throughput = %d, fileName = %s\r\n", elapsed_time, (int)throughput, t_filename);
    
    // fr = f_close(&Fil);													/* Close the file */

	return bw;
}

void sd_close()
{
    fr = f_close(&Fil);
}

uint8_t save_sdcard(uint8_t *image, int size)
{

   	printf("Saving PIC on SD card...\r\n");
	
	UINT bw;
	FRESULT fr;

    

	// memset(test_data, 'a', TEST_DATA_SIZE);
	// test_data[TEST_DATA_SIZE-1] = '\0';

	fr = f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	if (fr == FR_OK)
	{
		int filename_found = FR_OK;
		int f_counter = 0;
		FILINFO fno;

		while(filename_found == FR_OK)
		{
			sprintf(t_filename, "Out%d.ppm", f_counter++);
			filename_found = f_stat(t_filename ,&fno);
		}
		delay_ms(100);

		fr = f_open(&Fil, t_filename, FA_WRITE | FA_CREATE_ALWAYS);

		if( fr != FR_OK)
		{
			printf("Error in f_open() with value %d \r\n", fr);
			while(1);
		}
			
		else
		{
			int start_time  = delay_get_system_time();
			fr = f_write(&Fil, image, size, &bw);				/* Write data to the file */
			int elapsed_time = (delay_get_system_time() - start_time)/1000;
			float throughput = (float)size/((float)elapsed_time/1000);
			printf("Test successfull, elapsed time %d, throughput = %d, fileName = %s\r\n", elapsed_time, (int)throughput, t_filename);
			fr = f_close(&Fil);													/* Close the file */
		}
	}
	else
	{
		printf("Error in f_mount() with value %d \r\n", fr);
		while(1);
	}

	//while(1);
}

