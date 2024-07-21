#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

// ledtest /dev/myled on 	点灯
// lettest /dev/myled off 	熄灯

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <dev> <on/off>\n", argv[0]);
		printf("eg: %s /dev/myled on\n", argv[0]);
		printf("eg: %s /dev/myled off\n", argv[0]);
		return -1;
	}
	// open
	int fd = open(argv[1], O_RDWR);					// 可读可写
	if (fd < 0)
	{
		printf("Failed to open %s\n", argv[1]);
		return -1;
	}

	char status = 0;
	// write
	if (strcmp(argv[2], "on") == 0)
	{
		status = 1;
	}
	write(fd, &status, 1);				// 写回去


	return 0;
}

