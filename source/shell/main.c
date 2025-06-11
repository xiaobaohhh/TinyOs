#include "lib_syscall.h"

int main(int argc,char **argv)
{
    int count = 0;
    for(int i = 0; i < argc; i++)
    {
        print_msg("argv = %s\n",argv[i]);
    }
    for(;;)
    {
        print_msg("count = %d\n",count++);
        //msleep(1000);
    }
}