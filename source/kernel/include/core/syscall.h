/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-07 21:49:00
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-23 16:09:59
 * @FilePath: \start\source\kernel\include\core\syscall.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef SYSCALL_H
#define SYSCALL_H
#include "comm/types.h"
#define SYSCALL_PARAM_COUNT 5
#define SYS_sleep 0
#define SYS_getpid 1
#define SYS_print_msg 100
#define SYS_fork 2
#define SYS_execve 3
#define SYS_yield 4
#define SYS_exit 5
#define SYS_wait 6

#define SYS_opendir 60
#define SYS_readdir 61
#define SYS_closedir 62

#define SYS_open 50
#define SYS_read 51
#define SYS_write 52
#define SYS_close 53
#define SYS_lseek 54

#define SYS_isatty 55
#define SYS_fstat 56
#define SYS_sbrk 57

#define SYS_dup 58

#define SYS_ioctl 59

#define SYS_unlink 63

void exception_handler_syscall(void);

typedef struct _syscall_frame_t
{
    int eflags;
    int gs;
    int fs;
    int es;
    int ds;
    int edi;
    int esi;
    int ebp;
    int dummy;
    int ebx;
    int edx;
    int ecx;
    int eax;
    int eip;
    int cs;
    int func_id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
    int esp,ss;
}syscall_frame_t;
#endif