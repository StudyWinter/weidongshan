#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/bitrev.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/io.h>


// 主设备号
static int major = 0;
static struct class *led_class;



// write函数
static ssize_t led_write(struct file *filp, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	// 从用户拷贝数据
	char value;
	copy_from_user(&value, buf, 1);				// 将用户空间buf的数据拷贝1字节到value中

	// 设置GPIO寄存器1/0
	if (value)
	{
		// 设置led on
	}
	else
	{
		// 设置led off
	}

	return 1;
}



// open函数
static int led_open(struct inode *inode, struct file *filp)
{
	// 使能GPIO
	// 将某个引脚配置成GPIO
	// 配置GPIO是输出模式，只有用户程序open的时候，才表示要使用这个引脚，这个时候再配置引脚
	
	return 0;
}


// file_operations结构体
static struct file_operations led_fops {
	.owner	= THIS_MODULE,
	.write	= led_write,
	.open	= led_open,
};




// 入口函数
static int __init led_drv(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);				// 打印
	// 注册file_operations结构体，返回值是主设备号
	major = register_chrdev(0, "winter_led", &led_fops);

	// 驱动程序访问硬件，必须先ioremap

	// 创建class
	class_led = class_create(THIS_MODULE, "myled");
	// 创建设备
	device_create(class_led, NULL, MKDEV(major, 0), NULL, "myled");		// 系统会创建名为/dev/myled的设备节点


	return 0;
}

// 出口函数
static void __exit led_exit(void)
{
	// 卸载设备
	device_destroy(class_led, MKDEV(major, 0));
	// 销毁类
	class_destroy(class_led);
	// 卸载
	unregister_chrdev(major, "winter_led");
	
};



module_init(led_drv);
module_exit(led_exit);
MODULE_LICENSE("GPL");




