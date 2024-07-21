#ifndef LED_RESOURCE_H
#define LED_RESOURCE_H

// gpio3_0
// bit[31:16] = group
// bit[15:0] = which pin
#define GROUP(x) ((x) >> 16)
#define PIN(x) ((x) & 0xFFFF)
#define GROUP_PIN(g, p) (((g) << 16) | (p))

struct led_resource {
	int pin;
};

// 声明函数
struct led_resource* get_led_resource(void);

#endif

