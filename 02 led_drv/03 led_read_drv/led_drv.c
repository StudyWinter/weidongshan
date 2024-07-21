/*************************************************************************
 > File Name: led.drv.c
 > Author: Winter
 > Created Time: Sun 07 Jul 2024 12:35:19 AM EDT
 ************************************************************************/

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
#include "led_operations.h"

#define LED_NUM 2


// 1确定主设备号，也可以让内核分配
static int major = 0;				// 让内核分配
static struct class *led_class;
struct led_operations* p_led_operations;
char status[1024];




#define MIN(a, b) (a < b ? a : b)

// 3 实现对应的 drv_open/drv_read/drv_write 等函数，填入 file_operations 结构体
// res = read(fd, val, 1)
static ssize_t led_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	struct inode* node;
	int minor;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 将kernel_buf区的数据拷贝到用户区数据status中，即从内核kernel_buf中读数据
	err = copy_to_user(buf, status, MIN(1024, size));
	// 根据次设备号和status控制LED
	node = file_inode(file);
	minor = iminor(node);
	p_led_operations->ctl_read(minor);
	return MIN(1024, size);
}

// write(fd, &val, 1);
static ssize_t led_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	struct inode* node;
	int minor;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 把用户区的数据buf拷贝到内核区status，即向写到内核status中写数据
	err = copy_from_user(status, buf, MIN(1024, size));
	// 根据次设备号和status控制LED
	node = file_inode(file);
	minor = iminor(node);
	p_led_operations->ctl_write(minor, status);
	return MIN(1024, size);
}

static int led_drv_open (struct inode *node, struct file *file)
{
	int minor;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 得到次设备号
	minor = iminor(node);
	
	// 根据次设备号初始化LED
	p_led_operations->init(minor);
	return 0;
}

static int led_drv_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}


// 2定义自己的 file_operations 结构体
static struct file_operations led_drv = {
	.owner = THIS_MODULE,
	.open = led_drv_open,
	.read = led_drv_read,
	.write = led_drv_write,
	.release = led_drv_close,
};


// 4把 file_operations 结构体告诉内核： register_chrdev
// 5谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数
static int __init led_init(void)
{
	int err, i;	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 注册led_drv，返回主设备号
	major = register_chrdev(0, "winter_led", &led_drv);  /* /dev/led */
	// 创建class
	led_class = class_create(THIS_MODULE, "led_class");
	err = PTR_ERR(led_class);
	if (IS_ERR(led_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "led_class");
		return -1;
	}
	// 创建device
	// 根据次设备号访问多个LED
//	device_create(led_class, NULL, MKDEV(major, 0), NULL, "winter_led0"); /* /dev/winter_led0 */
//	device_create(led_class, NULL, MKDEV(major, 1), NULL, "winter_led1"); /* /dev/winter_led1 */
	for (i = 0; i < LED_NUM; i++)
	{
		device_create(led_class, NULL, MKDEV(major, i), NULL, "winter_led%d", i);
	}

	// 入口函数获得结构体指针
	p_led_operations = get_board_led_operations();
	
	return 0;
}



// 6有入口函数就应该有出口函数：卸载驱动程序时，出口函数调用unregister_chrdev
static void __exit led_exit(void)
{
	int i;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	for (i = 0; i < LED_NUM; i++)
	{
		device_destroy(led_class, MKDEV(major, i));
	}
	class_destroy(led_class);
	// 卸载
	unregister_chrdev(major, "winter_led");
}


// 7其他完善：提供设备信息，自动创建设备节点： class_create,device_create
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");

