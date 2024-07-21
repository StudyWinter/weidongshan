#include <linux/gfp.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include "led_resource.h"
#include "led_operations.h"

static struct led_resource* p_led_reource;


// init函数
static int board_demo_led_init(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	if (!p_led_reource)
	{
		p_led_reource = get_led_resource();
	}
	printk("init gpio: group: %d, pin: %d\n", GROUP(p_led_reource->pin), PIN(p_led_reource->pin));
	switch(GROUP(p_led_reource->pin))
	{
		case 0:
		{
			printk("init pin of group 0...\n");
			break;
		}
		case 1:
		{
			printk("init pin of group 1...\n");
			break;
		}
		case 2:
		{
			printk("init pin of group 2...\n");
			break;
		}
		case 3:
		{
			printk("init pin of group 3...\n");
			break;
		}
	}
	return 0;
}

// ctl函数
static int board_demo_led_ctl(int which, char status)
{
	printk("set led: %s: group: %d, pin: %d\n", status ? "on" : "off",
		GROUP(p_led_reource->pin), PIN(p_led_reource->pin));
	switch(GROUP(p_led_reource->pin))
	{
		case 0:
		{
			printk("init pin of group 0...\n");
			break;
		}
		case 1:
		{
			printk("init pin of group 1...\n");
			break;
		}
		case 2:
		{
			printk("init pin of group 2...\n");
			break;
		}
		case 3:
		{
			printk("init pin of group 3...\n");
			break;
		}
	}
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








