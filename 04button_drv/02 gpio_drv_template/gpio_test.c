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
 
/*
 * ./led_drv  /dev/winter_led@0 -w on
 * ./led_drv  /dev/winter_led@0 -w off
 * ./led_drv  /dev/winter_led@0 -r
 */
int main(int argc, char **argv)
{
	int fd;
	int len;
	char buf[1024];
 
	
	/* 1. 判断参数 */
	if (argc < 3) 
	{
		printf("Usage: %s <dev> -w <on | off>\n", argv[0]);
		printf("       %s <dev> -r\n", argv[0]);
		return -1;
	}
 
	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}
 
	/* 3. 写文件或读文件 */
	if ((0 == strcmp(argv[2], "-w")) && (0 == strcmp(argv[3], "on")))
	{
		write(fd, argv[3], strlen(argv[3]) + 1);
	}
	else if ((0 == strcmp(argv[2], "-w")) && (0 == strcmp(argv[3], "off")))
	{
		write(fd, argv[3], strlen(argv[3]) + 1);
	}
	else
	{
		len = read(fd, buf, 1024);
		buf[1023] = '\0';
		printf("APP read : %s\n", buf);
	}
	
	
	close(fd);
	
	return 0;
}
