#include <stdio.h>
#include <unistd.h>
#include <signal.h>
 
void my_signal_func(int signo)
{
	printf("get a signal: %d\n", signo);
}
 
int main(int argc, char** argv)
{
	int i = 0;
	signal(SIGIO, my_signal_func);
	
	while (1)
	{
		printf("hello world: %d\n", i++);
        sleep(2);
	}
	return 0;
}
 