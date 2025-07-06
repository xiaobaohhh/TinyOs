/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-05 13:18:46
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-24 15:40:55
 * @FilePath: \start\source\kernel\init\first_task.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "core/task.h"
#include "tools/log.h"
#include "core/memory.h"
#include "cpu/mmu.h"
#include "applib/lib_syscall.h"
#include <stdio.h>
#include "dev/tty.h"
int first_task_main()
{
#if 0
    int count = 3;
    int pid = getpid();
    //printf("parent task pid: %d\n",pid);
    pid = fork();
    if(pid < 0)
    {
        //printf("fork failed :%d\n",0);
    }
    else if(pid == 0)
    {
        char * argv[] = {"arg0","arg1","arg2","arg3"};
        execve("/shell.elf",argv,(char **)0);
        for(;;)
        {
            //printf("child task count: %d\n",count++);
            msleep(1000);
        }
        
    }
    else
    {
        for(;;)
        {
            //print_msg("child task pid: %d\n",pid);
            // print_msg("parent task count ______________________: %d\n",count++);
            // msleep(1000);
        }
        
    }
#endif
    for(int i = 0; i < TTY_NR; i++)
    {
        int pid = fork();
        if(pid < 0)
        {
            break;
        }
        else if(pid == 0)
        {
            char tty_num[10] = "/dev/tty?";
            tty_num[sizeof(tty_num) - 2] = i + '0';
            char * argv[] = {tty_num,(char *)0};
            execve("shell.elf",argv,(char **)0);
            while(1)
            {
                msleep(1000);
            }
        }
        else
        {
            break;
        }
        
    }
    for(;;)
    {
        int status;
        wait(&status);
    }
    return 0;
} 