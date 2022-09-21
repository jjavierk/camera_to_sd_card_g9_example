#include "delay.h"
//#include "stm32f4xx.h"

int delay_get_system_time()
{
	return pi_time_get_us();
}

void delay_ms(int time_ms)
{
	pi_time_wait_us(time_ms*1000);
	//check_module_state();
	// counter_systick = 0;
	// while(counter_systick<time_ms*1000);
}

void delay_us(int time_us)
{
	pi_time_wait_us(time_us);
	// check_module_state();
	// counter_systick = 0;
	// while(counter_systick<time_us);
}


