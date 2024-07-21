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
#include <linux/of.h>
#include <asm/io.h>
#include <linux/platform_device.h>

#include "led_resource.h"
#include "led_operations.h"
#include "led_drv.h"


static int g_ledpins[100];					// 记录引脚
static int g_ledcount = 0;					// 计数器

// 不能使用物理地址，需要映射
// 1寄存器
// RCC_PLL4CR地址：0x50000000 + 0x894，提供时钟的
static volatile unsigned int* RCC_PLL4CR;

// 2使能GPIOA本身
// RCC_MP_AHB4ENSETR地址：0x50000000 + 0xA28
static volatile unsigned int* RCC_MP_AHB4ENSETR;

// 3设置引脚为输出模式
// GPIOA_MODER地址：0x50002000 + 0x00，设置bit[21:20]=0b01，用于输出模式
static volatile unsigned int* GPIOA_MODER;

// 4设置输出电平
// 方法2：直接写寄存器，一次操作即可，高效
// GPIOA_BSRR地址： 0x50002000 + 0x18
static volatile unsigned int* GPIOA_BSRR;



// init函数
static int board_demo_led_init(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	printk("init gpio: group: %d, pin: %d\n", GROUP(g_ledpins[which]), PIN(g_ledpins[which]));
	// 之前没有映射，就映射
	if (!RCC_PLL4CR)
	{
		// 驱动程序访问硬件，必须先ioremap，在这里映射，映射的是一页4k的地址，参考
		// ioremap(base_phy, size);
		// 1寄存器
		// RCC_PLL4CR地址：0x50000000 + 0x894，提供时钟的
		// static volatile unsigned int* RCC_PLL4CR;
		RCC_PLL4CR = ioremap(0x50000000 + 0x894, 4);

		// 2使能GPIOA本身
		// RCC_MP_AHB4ENSETR地址：0x50000000 + 0xA28
		// static volatile unsigned int* RCC_MP_AHB4ENSETR;
		RCC_MP_AHB4ENSETR = ioremap(0x50000000 + 0xA28, 4);

		// 3设置引脚为输出模式
		// GPIOA_MODER地址：0x50002000 + 0x00，设置bit[21:20]=0b01，用于输出模式
		// static volatile unsigned int* GPIOA_MODER;
		GPIOA_MODER = ioremap(0x50002000 + 0x00, 4);


		// 4设置输出电平
		// 方法2：直接写寄存器，一次操作即可，高效
		// GPIOA_BSRR地址： 0x50002000 + 0x18
		// static volatile unsigned int* GPIOA_BSRR;
		GPIOA_BSRR = ioremap(0x50002000 + 0x18, 4);
	}

	// 初始化引脚
	if (which == 0)
	{
		// 使能PLL4，是所有GPIO的时钟
		*RCC_PLL4CR |= (1 << 0);					// 设置bit0为1
		while ((*RCC_PLL4CR & (1 << 1)) == 0);		// 如果bit1一直为0的话，就等待
		
		// 使能GPIOA
		*RCC_MP_AHB4ENSETR |= (1 << 0); 			// 1左移0位
		
		// 将GPIOA的第十个引脚配置成GPIO
		// 配置GPIO是输出模式，只有用户程序open的时候，才表示要使用这个引脚，这个时候再配置引脚	
		*GPIOA_MODER &= ~(3 << 20); 				// 清零 11左移20位，取反，
		*GPIOA_MODER |= (1 << 20);					// 20位设置成1，配置成01，输出模式
	}

	return 0;
}

// ctl函数
static int board_demo_led_ctl(int which, char status)
{
	printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	printk("set led: %s: group: %d, pin: %d\n", status ? "on" : "off",
		GROUP(g_ledpins[which]), PIN(g_ledpins[which]));
	// 设置高/低电平
	if (which == 0)
	{
		// 设置GPIOA10寄存器1/0
		if (status)
		{
			// 设置led on，让引脚输出低电平
			*GPIOA_BSRR =  (1 << 26);				// 1左移26
			
		}
		else
		{
			// 设置led off，让引脚输出高电平
			*GPIOA_BSRR =  (1 << 10);				// 1左移10
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
	// 需要从platform_device中找到对应的设备结点，取出里面的pin属性
	struct device_node* np;
	int led_pin, err;
	np = device->dev.of_node;
	if (!np)
	{
		return -1;
	}
	// 从np结点中读出pin属性，存到led_pin中
	err = of_property_read_u32(np, "pin", &led_pin);
	// 记录引脚
	g_ledpins[g_ledcount] = led_pin;
	// 同时创建device
	led_device_create(g_ledcount);
	g_ledcount++;


	return 0;
}


// remove函数
static int chip_demo_gpio_remove(struct platform_device *device)
{
	struct device_node* np;
	int i = 0;
	int led_pin;
	int err;

	// 获取node结点
	np = device->dev.of_node;
	if (!np)
	{
		return -1;
	}
	// 取出pin属性
	err = of_property_read_u32(np, "pin", &led_pin);

	for (i = 0; i < g_ledcount; i++)
	{
		if (g_ledpins[i] == led_pin)
		{
			// 注销
			led_device_destroy(i);
			g_ledpins[i] = -1;
			break;
		}
	}

	for (i = 0; i < g_ledcount; i++)
	{
		if (g_ledpins[i] != -1)
		{
			break;
		}
	}
	if (i == g_ledcount)
	{
		g_ledcount = 0;
	}
	
	
	return 0;
}

static const struct of_device_id winter_leds[] = {
	{ .compatible = "winter_leddrv" },
};



// platform_driver结构体
// name一致
static struct platform_driver chip_demo_gpio_drv = {
	.driver = {
		.name  = "winter_led",
		.of_match_table = winter_leds,
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



