#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/platform_device.h>

#include "led_resource.h"
#include "led_operations.h"
#include "led_drv.h"


static int g_ledpins[100];					// 记录引脚
static int g_ledcount = 0;					// 计数器


// init函数
static int board_demo_led_init(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	printk("init gpio: group: %d, pin: %d\n", GROUP(g_ledpins[which]), PIN(g_ledpins[which]));
	switch(GROUP(g_ledpins[which]))
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
	printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	printk("set led: %s: group: %d, pin: %d\n", status ? "on" : "off",
		GROUP(g_ledpins[which]), PIN(g_ledpins[which]));
	switch(GROUP(g_ledpins[which]))
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

// get函数
struct led_operations *get_board_led_operations(void)
{
    return &board_demo_led_operations;
}


// probe函数
// (1)记录引脚；（2）对于每一个引脚要调用device_create（辅助作用）
static int chip_demo_gpio_probe(struct platform_device *device)
{
	int i = 0;
	struct resource* res;
	while (1)
	{
		// 获取资源，IORESOURCE_IRQ类型的资源，第i个资源
		res = platform_get_resource(device, IORESOURCE_IRQ, i++);
		if (!res)
		{
			break;
		}
		// 记录引脚
		g_ledpins[g_ledcount] = res->start;
		// 同时创建device
		led_device_create(g_ledcount);
		g_ledcount++;
	}

	return 0;
}


// remove函数
static int chip_demo_gpio_remove(struct platform_device *device)
{
	struct resource *res;
	int i = 0;
	
	while (1)
	{
		res = platform_get_resource(device, IORESOURCE_IRQ, i);
		if (!res)
		{
			break;
		}
		led_device_destroy(i);
		i++;
		g_ledcount--;
	}
	return 0;
}


// platform_driver结构体
// name一致
static struct platform_driver chip_demo_gpio_drv = {
	.driver = {
		.name  = "winter_led",
	},
	.probe = chip_demo_gpio_probe,
	.remove = chip_demo_gpio_remove,
};


// 注册这个结构体，入口出口
static int chip_demo_gpio_drv_init(void)
{
	int err;
	err = platform_driver_register(&chip_demo_gpio_drv);
	// 向上层提供了led的操作函数
	register_led_operations(&board_demo_led_operations);
	return 0;
}

// 出口函数，注销用
static void chip_demo_gpio_drv_exit(void)
{
	platform_driver_unregister(&chip_demo_gpio_drv);
}

// 修饰为入口函数和出口函数
module_init(chip_demo_gpio_drv_init);
module_exit(chip_demo_gpio_drv_exit);
MODULE_LICENSE("GPL");







