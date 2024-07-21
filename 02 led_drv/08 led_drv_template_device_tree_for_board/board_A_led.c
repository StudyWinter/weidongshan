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


// 资源数组
// flag表示是哪一类资源，暂时认定IORESOURCE_IRQ表示引脚（IO不就是引脚吗）
// end先不管
static struct resource resource[] = {
	{
		.name = "winter_led_pin",
		.start = GROUP_PIN(3, 1),
		.flags = IORESOURCE_IRQ
	},
	{
		.name = "winter_led_pin",
		.start = GROUP_PIN(5, 8),
		.flags = IORESOURCE_IRQ
	},
};

// platform_device结构体
static struct platform_device board_A_led_drv = {
	.name = "winter_led",
	.num_resources = ARRAY_SIZE(resource),
	.resource = resource,
};

// 入口函数，注册用
static int led_dev_init(void)
{
	int err;
	err = platform_device_register(&board_A_led_drv);
	return 0;
}

// 出口函数，注销用
static void led_dev_exit(void)
{
	platform_device_unregister(&board_A_led_drv);
}

// 修饰为入口函数和出口函数
module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");


