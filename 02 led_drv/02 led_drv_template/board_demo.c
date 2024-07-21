#include <linux/gfp.h>
#include "led_operations.h"

// init函数
static int board_demo_led_init(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	return 0;
}

// ctl函数
static int board_demo_led_ctl(int which, char status)
{
	printk("%s %s line %d, led %d, %s\n", 
		__FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	return 0;
}

static struct led_operations board_demo_led_operations = {
	.init = board_demo_led_init,
	.ctl = board_demo_led_ctl,
};

// 返回结构体
struct led_operations* get_board_led_operations(void)
{
	return &board_demo_led_operations;
}




