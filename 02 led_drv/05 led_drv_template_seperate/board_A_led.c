#include "led_resource.h"

// gpio3_1
// 3(11)左移16位 -- 11 0000 0000 0000 0000									 1
static struct led_resource board_A_led = {
	.pin = GROUP_PIN(3, 1),
};

// 返回指针
struct led_resource* get_led_resource(void)
{
	return &board_A_led;
}

