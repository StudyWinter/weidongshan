#ifndef BUTTON_DRV_H_
#define BUTTON_DRV_H_

struct button_operations {
	int count;
	void (*init) (int which);
	int (*read) (int which);
};

void register_button_operations(struct button_operations* operaions);
void unregister_button_operations(void);

#endif

