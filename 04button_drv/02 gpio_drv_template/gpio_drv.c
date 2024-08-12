
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


// 1主设备号
static int major = 0;
static struct class* gpio_class;
static struct gpio_operations* p_gpio_operations;
char status[1024];

#define MIN(a, b) (a < b ? a : b)


// 注册设备结点
void register_gpio_operations(struct gpio_operations* p_operaions)
{
	int i;
	// 赋值
	p_gpio_operations = p_operaions;
	for (i = 0; i < p_operaions->count; i++)
	{
		device_create(gpio_class, NULL, MKDEV(major, i), NULL, "winter_gpio@%d", i);
	}
}
 
// 注销设备结点
void unregister_gpio_operations(void)
{
	int i;
	for (i = 0; i < p_gpio_operations->count; i++)
	{
		device_destroy(gpio_class, MKDEV(major, i));
	}
}
 

EXPORT_SYMBOL(register_gpio_operations);
EXPORT_SYMBOL(unregister_gpio_operations);



// 4实现open/read/write函数
int gpio_drv_open (struct inode* node, struct file* file)
{
	int minor;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 用次设备号控制某个按键
	minor = iminor(node);
	p_gpio_operations->init(minor);
	return 0;
}

// read读取按键信息，即从内核中读数据
ssize_t gpio_drv_read (struct file* file, char __user* buff, size_t size, loff_t* offset)
{
	unsigned int minor;
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 通过file获取次设备号
	minor = iminor(file_inode(file));
	// 电平高低
	// 调用read函数
	p_gpio_operations->read(minor);
	// 将kernel_buf区的数据拷贝到用户区数据buf中，即从内核kernel_buf中读数据
	err = copy_to_user(buff, status, MIN(1024, size));
	return 1;
}
// write写数据，即向内核中写数据
ssize_t gpio_drv_write (struct file* file, const char __user* buff, size_t size, loff_t* offset)
{
	int err;
	struct inode* node;
	int minor;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 把用户区的数据buf拷贝到内核区status，即向写到内核status中写数据
	err = copy_from_user(status, buff, MIN(1024, size));
	// 根据次设备号和status控制LED
	node = file_inode(file);
	minor = iminor(node);
	p_gpio_operations->write(minor, status);
	return MIN(1024, size);
}

static int gpio_drv_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}


// 2定义自己的file_operations结构体
static struct file_operations gpio_operations = {
	.open = gpio_drv_open,
	.read = gpio_drv_read,
	.write = gpio_drv_write,
	.release = gpio_drv_close,
};

// 3在入口函数中注册
int gpio_init(void)
{
	// 注册file_operations结构体
	major = register_chrdev(0, "winter_gpio", &gpio_operations);
	// 注册结点
	gpio_class = class_create(THIS_MODULE, "winter_gpio");
	// 注册失败
	if (IS_ERR(gpio_class))
	{
		return -1;
	}
	return 0;
}

// 出口函数
void gpio_exit(void)
{
	class_destroy(gpio_class);
	unregister_chrdev(major, "winter_gpio");
}

module_init(gpio_init);
module_exit(gpio_exit);
MODULE_LICENSE("GPL");



