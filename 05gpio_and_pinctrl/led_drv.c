#include <linux/module.h>
#include <linux/platform_device.h>

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
#include <linux/gpio/consumer.h>
#include <linux/of.h>



// 1确定主设备号
static int major = 0;
static struct class* led_class;
static struct gpio_desc *led_gpio;


// 实现对应的open/read等函数
int led_drv_open (struct inode* node, struct file* file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 根据次设备号初始化LED
	gpiod_direction_output(led_gpio, 0);
	return 0;
}
// 从内核中读数据到用户
ssize_t led_drv_read (struct file* file, char __user* buf, size_t size, loff_t* offset)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}
// 用户向内核中写数据
ssize_t led_drv_write (struct file* file, const char __user* buf, size_t size, loff_t* offset)
{
	char status;
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_from_user(&status, buf, 1);
	// 根据次设备号和status控制LED
	gpiod_set_value(led_gpio, status);

	return 1;
}

int led_drv_close (struct inode* noed, struct file* file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}


// 定义自己的file_operations结构体
static struct file_operations led_drv = {
	.owner = THIS_MODULE,
	.open = led_drv_open,
	.read = led_drv_read,
	.write = led_drv_write,
	.release = led_drv_close,
};


/* 	  从platform_device获得GPIO
 *    把file_operations结构体告诉内核：注册驱动程序
 */
int chip_demo_gpio_probe (struct platform_device* pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 设备树中定义 led-gpios=<...>
	led_gpio = gpiod_get(&pdev->dev, "led", 0);
	if (IS_ERR(led_gpio))
	{
		dev_err(&pdev->dev, "Failed to get GPIO for led\n");
		return PTR_ERR(led_gpio);
	}

	// 注册file_operations结构体
	major = register_chrdev(0, "winter_led", &led_drv);	 // /proc/devices/winter_led
	// 注册结点
	led_class = class_create(THIS_MODULE, "winter_led_class");
	if (IS_ERR(led_class))
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "led");
		gpiod_put(led_gpio);
		return PTR_ERR(led_class);
	}

	device_create(led_class, NULL, MKDEV(major, 0), NULL, "winter_led%d", 0); /* /dev/100ask_led0 */
	return 0;
}



int chip_demo_gpio_remove (struct platform_device* pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	unregister_chrdev(major, "winter_led");
	gpiod_put(led_gpio);
	return 0;
}


// 资源
static const struct of_device_id winter_leds[] = {
	{ .compatible = "winter,led_drv"},
	{},
};

// 定义platform_driver
static struct platform_driver chip_demo_gpio_driver = {
	.probe = chip_demo_gpio_probe,
	.remove = chip_demo_gpio_remove,
	.driver = {
		.name = "winter_led",
		.of_match_table = winter_leds,
	},
};

// 入口函数中注册platform_driver
static int __init led_init(void)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 注册platform_driver结构体
	err = platform_driver_register(&chip_demo_gpio_driver);
	return err;
}

// 出口函数
static void __exit led_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	platform_driver_unregister(&chip_demo_gpio_driver);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");



