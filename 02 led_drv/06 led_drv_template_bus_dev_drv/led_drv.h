#ifndef _LEDDRV_H
#define _LEDDRV_H

#include "led_operations.h"

void led_device_create(int minor);
void led_device_destroy(int minor);
void register_led_operations(struct led_operations *operation);

#endif /* _LEDDRV_H */
