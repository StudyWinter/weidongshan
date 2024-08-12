#ifndef GPIO_DRV_H_
#define GPIO_DRV_H_
 
struct gpio_operations {
	int count;
	void (*init) (int which);				   		 // 初始化LED，which是哪一个LED
	int (*read) (int which);				   		 // 读LED,which是哪一个LED
	int (*write) (int which, char status[1024]);     // 控制LED，which-哪一个LED，status-1亮，0灭
};

void register_gpio_operations(struct gpio_operations* p_operaions);
void unregister_gpio_operations(void);

 
#endif

