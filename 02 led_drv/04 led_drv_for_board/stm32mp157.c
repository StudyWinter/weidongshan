#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/device.h>

#include "led_operations.h"

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


// init函数-配置引脚，把引脚配置成GPIO输出功能
static int board_demo_led_init(int which)
{
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
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

// ctl函数-通过参数把引脚设置成高/低电平
static int board_demo_led_ctl(int which, char status)
{
	printk("%s %s line %d, led %d, %s\n", 
		__FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
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


// 加一个num属性
static struct led_operations board_demo_led_operations = {
	.num = 1,
	.init = board_demo_led_init,
	.ctl = board_demo_led_ctl,
};

// 返回结构体
struct led_operations* get_board_led_operations(void)
{
	return &board_demo_led_operations;
}





