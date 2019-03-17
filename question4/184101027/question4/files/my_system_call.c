#include <stdio.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <unistd.h>
int main()
{
         long int status = syscall(548);
         printf("sys_hello system call returned status :%ld\n",status);
         return 0;
}
