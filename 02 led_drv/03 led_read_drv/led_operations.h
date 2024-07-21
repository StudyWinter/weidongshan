#ifndef LED_OPERATIONS_H
#define LED_OPERATIONS_H

struct led_operations {
    int (*init) (int which);                 	   	   // 初始化LED，which是哪一个LED
    int (*ctl_write) (int which, char status[1024]);   // 控制LED，which-哪一个LED，status-1亮，0-灭
    int (*ctl_read) (int which);				       // 控制LED，which-哪一个LED，返回值1-亮，0-灭
};

// 返回结构体指针
struct led_operations* get_board_led_operations(void);


#endif

