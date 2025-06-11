/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-07 21:49:00
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-09 14:00:39
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