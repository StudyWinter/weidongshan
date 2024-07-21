#include <linux/gfp.h>
#include "led_operations.h"

// init函数
static int board_demo_led_init(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	return 0;
}

// ctl_write函数
static int board_demo_led_ctl_write(int which, char status[1024])
{
	char* str = status;
	printk("%s %s line %d, led %d, %s\n", 
		__FILE__, __FUNCTION__, __LINE__, which, strcmp(str, "on") == 0 ? "on" : "off");
	return 0;
}

// ctl_write函数
static int board_demo_led_ctl_read(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	return 0;
}


static struct led_operations board_demo_led_operations = {
	.init = board_demo_led_init,
	.ctl_write = board_demo_led_ctl_write,
	.ctl_read = board_demo_led_ctl_read,
};

// 返回结构体
struct led_operations* get_board_led_operations(void)
{
	return &board_demo_led_operations;
}




