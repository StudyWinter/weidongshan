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

#include "button_drv.h"



// 1主设备号
static int major = 0;
static struct class* button_class;
static struct button_operations* p_button_operation;

// 注册设备结点
void register_button_operations(struct button_operations* p_operaions)
{
	int i;
	// 赋值
	p_button_operation = p_operaions;
	for (i = 0; i < p_operaions->count; i++)
	{
		device_create(button_class, NULL, MKDEV(major, i), NULL, "winter_button@%d", i);
	}
}

// 注销设备结点
void unregister_button_operations(void)
{
	int i;
	for (i = 0; i < p_button_operation->count; i++)
	{
		device_destroy(button_class, MKDEV(major, i));
	}
}

// 导出
EXPORT_SYMBOL(register_button_operations);
EXPORT_SYMBOL(unregister_button_operations);

// 4实现open/read函数
// open函数主要完成初始化操作
int button_open (struct inode* node, struct file* file)
{
	int minor;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 用次设备号控制某个按键
	minor = iminor(node);
	p_button_operation->init(minor);
	return 0;
}

// read读取按键信息
ssize_t button_read (struct file* file, char __user* buff, size_t size, loff_t* offset)
{
	unsigned int minor;
	int level;
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 通过file获取次设备号
	minor = iminor(file_inode(file));
	// 电平高低
	// 调用read函数
	level = p_button_operation->read(minor);
	// 将kernel_buf区的数据拷贝到用户区数据buf中，即从内核kernel_buf中读数据
	err = copy_to_user(buff, &level, 1);
	return 1;
}




// 2定义自己的file_operations结构体
static struct file_operations button_operations = {
	.open = button_open,
	.read = button_read,
};




// 3在入口函数中注册
int button_init(void)
{
	// 注册file_operations结构体
	major = register_chrdev(0, "winter_button", &button_operations);
	// 注册结点
	button_class = class_create(THIS_MODULE, "winter_button");
	// 注册失败
	if (IS_ERR(button_class))
	{
		return -1;
	}
	return 0;
}

// 出口函数
void button_exit(void)
{
	// 注销结点
	class_destroy(button_class);
	unregister_chrdev(major, "winter_button");
}

module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");



