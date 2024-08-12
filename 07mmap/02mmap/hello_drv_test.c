/*************************************************************************
 > File Name: hello_test.c
 > Author: Winter
 > Created Time: Sun 07 Jul 2024 01:39:39 AM EDT
 ************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

/*
 * ./hello_drv_test
 */
int main(int argc, char **argv)
{
	int fd;
	char* buf;
	int len;
	char str[1024];
	
	/* 1. 打开文件 */
	fd = open("/dev/hello", O_RDWR);
	if (fd == -1)
	{
		printf("can not open file /dev/hello\n");
		return -1;
	}

	// 2mmap可读可写共享偏移地址是0
	buf = mmap(NULL, 1024 * 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED)
	{
		printf("can not mmap file /dev/hello\n");
		return -1;
	}
	printf("mmap address = 0x%x\n", buf);

	// 3写数据
	strcpy(buf, "winter");


	// 4读数据并且比较
	read(fd, str, 1024);
	if(strcmp(buf, str) == 0)
	{
		printf("compare ok\n");
	}
	else
	{
		printf("compare fail\n");
	}

	while (1)
	{
		// cat /proc/id/maps
		sleep(10);
	}
	
	// 5关闭
	munmap(buf, 1024 * 8);
	close(fd);
	
	return 0;
}

