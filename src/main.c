/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "pmsis.h"
//#include "ImgIO.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
// #include "gaplib/ImgIO.h"
#include "ImgIO_SDCard.h"
#ifdef OV5647
#include "bsp/camera/ov5647.h"
#else
#include "bsp/camera/ov9281.h"
#endif

#include "delay.h"

#define WIDTH    640
#define HEIGHT   400

#ifdef OV5647
#define BUFF_SIZE (WIDTH*HEIGHT*2)
#else
#define BUFF_SIZE (WIDTH*HEIGHT)
#endif

pi_device_t i2c = {0};

PI_L2 unsigned char *buff[2];

#ifdef LOGS
#define PRINTF printf
#else
#define PRINTF(...) ((void) 0)
#endif

PI_L2 unsigned char buff_img[WIDTH*HEIGHT*2];


static int open_camera(struct pi_device *device)
{
	PRINTF("Opening CSI2 camera\n");
#ifdef OV5647
	struct pi_ov5647_conf cam_conf;
	pi_ov5647_conf_init(&cam_conf);
#else
	struct pi_ov9281_conf cam_conf;
	pi_ov9281_conf_init(&cam_conf);
#endif

	#if !defined(__GAP8__)
	// Set pull-down to I2C0 pads to avoid 'X' propagation in netlist tests
	uint32_t *pad_cfg_10 = (uint32_t *) (ARCHI_APB_SOC_CTRL_ADDR + 0x58);
	// Activate the pull-down of the I2C SCK and SDA pads to avoid
	// 'x' propagation and test failure in netlist tests
	*pad_cfg_10 = (*pad_cfg_10) | (1 << 8) | (1 << 0);
	#endif  /* __GAP8__ */

	pi_open_from_conf(device, &cam_conf);
	if (pi_camera_open(device))
		return -1;

	return 0;
}

int write_reg8(pi_device_t *dev, uint8_t addr, uint8_t value)
{
    uint8_t buffer[2] = { addr, value };
    if (pi_i2c_write(dev, buffer, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP))
    {
        printf("Error when writing \n");
        return -1;
    }

    return 0;
}

static uint8_t read_reg8(pi_device_t *dev, uint8_t addr)
{
    uint8_t result;
    pi_i2c_write_read(dev, &addr, &result, 1, 1);
    return result;
}

// Setup FXL6408 GPIO Expander
// Datasheet https://www.onsemi.com/pdf/datasheet/fxl6408-d.pdf
static int fxl6408_setup()
{
    PRINTF("Setup fxl6408\n");

    struct pi_i2c_conf conf;
    pi_i2c_conf_init(&conf);
    conf.itf = 3;
    conf.max_baudrate = 100000;
    pi_i2c_conf_set_slave_addr(&conf, 0x88, 0);
    PRINTF("*** check 1 ***\n");

    pi_open_from_conf(&i2c, &conf);
    PRINTF("*** check 2 ***\n");

    if (pi_i2c_open(&i2c))
    {
        printf("*** TEST FAILED ***\n");
        return -1;
    }
    PRINTF("*** check 3 ***\n");

    // Turn-on fxl6408
    write_reg8(&i2c, 0x01, 0x1); // SW reset the GPIO expander
    write_reg8(&i2c, 0x03, 0x1); // Direction: GPIO0 -> Output
    write_reg8(&i2c, 0x05, 0x1); // Output state: GPIO0 -> High
    write_reg8(&i2c, 0x07, 0x0); // Output state follow the 0x05 (above)
    PRINTF("*** check 4 ***\n");

    PRINTF("*** SETUP fxl6408 OK ***\n");
    return 0;
}

static int test_entry()
{
    #ifdef OV5647
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 40*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_FC, 40*1000*1000);
    #else
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 25*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_FC, 25*1000*1000);
    #endif


    PRINTF("Entering main controller\n");

    // Going to I2C3 to access IO expander
    pi_pad_set_function(PI_PAD_046, PI_PAD_FUNC2); // I2C3 SDA
    pi_pad_set_function(PI_PAD_047, PI_PAD_FUNC2); // I2C3 SCL
    PRINTF("***** Step 1 OK *****\n");


    struct pi_device gpio_ic_3v3;
    struct pi_gpio_conf gpio_conf = {0};
    pi_gpio_e gpio_pin_o = PI_GPIO_A00;

    pi_gpio_pin_write(&gpio_ic_3v3, gpio_pin_o, 1);
    pi_time_wait_us(1000000);
    PRINTF("***** Step 2 OK *****\n");


    fxl6408_setup();
    PRINTF("***** Step 3 OK *****\n");


    // Coming back to I3C to open the camera
	pi_pad_set_function(PI_PAD_046, PI_PAD_FUNC0); // I3C SDA
    pi_pad_set_function(PI_PAD_047, PI_PAD_FUNC0); // I3C SCL
	PRINTF("***** Step 4 OK *****\n");

    struct pi_device camera;
    pi_task_t task;

    if (open_camera(&camera))
    {
    printf("Failed to open camera\n");
    goto error;
    }
    
    printf("Capture init...\n");
    printf("Starting in 10s...\n");
    for(int i=0; i<10; i++)
        pi_time_wait_us(1000000);

    

    int start_time;
    int elapsed_time; 

    start_time  = delay_get_system_time();
    while(1)
    {
        
        // printf("pmsis_l2_malloc\n");
        // buff[0] = pmsis_l2_malloc(BUFF_SIZE);
        // if (buff[0] == NULL) goto error;
        //start_time  = delay_get_system_time();
        // printf("pi_camera_capture_async\n");
        pi_camera_capture_async(&camera, buff_img, BUFF_SIZE, pi_task_block(&task));
        pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);

        pi_camera_control(&camera, PI_CAMERA_CMD_ON, 0);
        pi_task_wait_on(&task);

        pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
        //elapsed_time = (delay_get_system_time() - start_time)/1000;

        //printf("pi_camera_capture_async finished with elapsed time = %d\n", elapsed_time);

        //printf("\nTest success - Saving image...\n");
        //start_time  = delay_get_system_time();
        WriteImageToFile_sd("Out.ppm", WIDTH, HEIGHT, sizeof(uint16_t), buff_img, BYPASS_IO);
        //elapsed_time = (delay_get_system_time() - start_time)/1000;
        //printf("WriteImageToFile_sd finished with elapsed time = %d\n", elapsed_time);

    }
    elapsed_time = (delay_get_system_time() - start_time)/1000;
    printf("finished with elapsed time = %d\n", elapsed_time);
    // int elapsed_time = (delay_get_system_time() - start_time)/1000;
    // float frame_rate = (float)10/((float)elapsed_time/1000);

    // printf("Capture finished with elapsed time = %d and framerate = %2.2f...\r", elapsed_time, frame_rate);

    pi_camera_close(&camera);
    pi_i2c_close(&i2c);
    pi_gpio_pin_write(&gpio_ic_3v3, PI_GPIO_A00, 0);

// #ifdef SAVE_PIC
// #ifdef OV5647
//     // bayer RAW10,  2 bytes, only 10bit effective
//     WriteImageToFile("Out.ppm", WIDTH, HEIGHT, sizeof(uint16_t), buff_img, BYPASS_IO);
// #else
//     WriteImageToFile("Out.ppm", WIDTH, HEIGHT, sizeof(uint8_t), buff[0], GRAY_SCALE_IO);
// #endif
// #endif

    return 0;

error:
    printf("Test failure\n");
    return -1;
}

static void test_kickoff(void *arg)
{
    //int ret = test_sd_card();
	int ret = test_entry();
    pmsis_exit(ret);
}

int main (void)
{

    return pmsis_kickoff((void *)test_kickoff);
}
