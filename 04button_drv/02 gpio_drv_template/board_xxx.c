#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <linux/capi.h>
#include <linux/kernelcapi.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
 
#include "gpio_drv.h"
 
 
// 实现具体的函数
static void board_xxx_gpio_init_gpio(int which)
{
	printk("%s %s %d, init gpio for gpio %d\n", __FILE__, __FUNCTION__, __LINE__, which);
}
 
static int board_xxx_gpio_read_gpio(int which)
{
	printk("%s %s %d, init gpio for gpio %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	// 返回高电平
	return 0;
}

static int board_xxx_gpio_write_gpio(int which, char status[1024])
{
	char* str = status;
	printk("%s %s line %d, led %d, %s\n", 
		__FILE__, __FUNCTION__, __LINE__, which, strcmp(str, "on") == 0 ? "on" : "off");
	return 0;
}

 
// 定义button_operations结构体
static struct gpio_operations my_gpio_operations = {
	.count = 2,
	.init = board_xxx_gpio_init_gpio,
	.read = board_xxx_gpio_read_gpio,
	.write = board_xxx_gpio_write_gpio,
};
 
// 入口函数
int board_xxx_init(void)
{
	// 注册设备结点
	register_gpio_operations(&my_gpio_operations);
	return 0;
}
 
// 出口函数
void board_xxx_exit(void)
{
	// 注销设备结点
	unregister_gpio_operations();
}
 
module_init(board_xxx_init);
module_exit(board_xxx_exit);
MODULE_LICENSE("GPL");

