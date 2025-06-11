/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-07 22:07:18
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-11 12:26:32
 * @FilePath: \start\source\applib\lib_syscall.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "core/syscall.h"
#include "os_cfg.h"
#include "core/task.h"


typedef struct _syscall_args_t
{
    int id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
}syscall_args_t;



static inline int sys_call(syscall_args_t *args)
{
    int ret;
    uint32_t addr[] = {0,SELECTOR_SYSCALL | 0};
    __asm__ __volatile__(
            "push %[arg3]\n\t"
            "push %[arg2]\n\t"
            "push %[arg1]\n\t"
            "push %[arg0]\n\t"
            "push %[id]\n\t"
            "lcalll *(%[a])"
            :"=a"(ret)
            :[arg3]"r"(args->arg3), [arg2]"r"(args->arg2), [arg1]"r"(args->arg1),
            [arg0]"r"(args->arg0), [id]"r"(args->id),
            [a]"r"(addr));
    return ret;

}
static inline void msleep(int ms)
{
    if(ms <= 0)
    {
        return;
    }
    syscall_args_t args;
    args.id = SYS_sleep;
    args.arg0 = ms;
    sys_call(&args);
}
static inline int getpid()
{
    syscall_args_t args;
    args.id = SYS_getpid;
    return sys_call(&args);
}
static inline void print_msg(const char *msg,int arg)
{
    syscall_args_t args;
    args.id = SYS_print_msg;
    args.arg0 = (int)msg;
    args.arg1 = arg;
    sys_call(&args);
}

static inline int fork()
{
    syscall_args_t args;
    args.id = SYS_fork;
    return sys_call(&args);
}

static inline int execve(const char *name,char * const *argv,char * const *env)
{
    syscall_args_t args;
    args.id = SYS_execve;
    args.arg0 = (int)name;
    args.arg1 = (int)argv;
    args.arg2 = (int)env;
    return sys_call(&args);
}
#endif
