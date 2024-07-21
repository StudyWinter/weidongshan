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



// 主设备号
static int major = 0;
static struct class *led_class;

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


// write函数
static ssize_t led_write(struct file *filp, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	// 从用户拷贝数据
	char value;
	copy_from_user(&value, buf, 1);				// 将用户空间buf的数据拷贝1字节到value中

	// 设置GPIOA10寄存器1/0
	if (value)
	{
		// 设置led on，让引脚输出低电平
		*GPIOA_BSRR =  (1 << 26); 				// 1左移26
		
	}
	else
	{
		// 设置led off，让引脚输出高电平
		*GPIOA_BSRR =  (1 << 10); 				// 1左移10
	}

	return 1;
}



// open函数
static int led_open(struct inode *inode, struct file *filp)
{
	// 使能PLL4，是所有GPIO的时钟
	*RCC_PLL4CR |= (1 << 0);					// 设置bit0为1
	while ((*RCC_PLL4CR & (1 << 1)) == 0);		// 如果bit1一直为0的话，就等待
	
	// 使能GPIOA
	*RCC_MP_AHB4ENSETR |= (1 << 0);				// 1左移0位

	// 将GPIOA的第十个引脚配置成GPIO
	// 配置GPIO是输出模式，只有用户程序open的时候，才表示要使用这个引脚，这个时候再配置引脚	
	*GPIOA_MODER &= ~(3 << 20);					// 清零 11左移20位，取反，
	*GPIOA_MODER |= (1 << 20);					// 20位设置成1，配置成01，输出模式
	
	return 0;
}


// file_operations结构体
static struct file_operations led_fops = {
	.owner	= THIS_MODULE,
	.write	= led_write,
	.open	= led_open,
};




// 入口函数
static int __init led_init(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);				// 打印
	// 注册file_operations结构体，返回值是主设备号
	major = register_chrdev(0, "winter_led", &led_fops);

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
	

	// 创建class
	led_class = class_create(THIS_MODULE, "myled");
	// 创建设备
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled");		// 系统会创建名为/dev/myled的设备节点


	return 0;
}

// 出口函数
static void __exit led_exit(void)
{
	iounmap(RCC_PLL4CR);
	iounmap(RCC_MP_AHB4ENSETR);
	iounmap(GPIOA_MODER);
	iounmap(GPIOA_BSRR);
	
	// 卸载设备
	device_destroy(led_class, MKDEV(major, 0));
	// 销毁类
	class_destroy(led_class);
	// 卸载
	unregister_chrdev(major, "winter_led");
	
};



module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");




