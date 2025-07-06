#include "core/syscall.h"
#include "core/task.h"
#include "tools/log.h"
#include "fs/fs.h"
#include "core/memory.h"

typedef int (*sys_handler_t)(uint32_t arg0,uint32_t arg1,uint32_t arg2,uint32_t arg3);
extern void sys_sleep(uint32_t ms);
extern int sys_getpid(void);
extern int sys_fork(void);
extern int sys_execve(char *name, char **argv, char **env);
void sys_print_msg(const char *msg,int arg)
{
    log_printf(msg,arg);
}

static const sys_handler_t sys_handler_table[] = {
    [SYS_sleep] = (sys_handler_t)sys_sleep, 
    [SYS_getpid] = (sys_handler_t)sys_getpid,
    [SYS_print_msg] = (sys_handler_t)sys_print_msg,
    [SYS_fork] = (sys_handler_t)sys_fork,
    [SYS_execve] = (sys_handler_t)sys_execve,
    [SYS_yield] = (sys_handler_t)sys_sched_yield,
    [SYS_exit] = (sys_handler_t)sys_exit,
    [SYS_wait] = (sys_handler_t)sys_wait,

    [SYS_open] = (sys_handler_t)sys_open,
    [SYS_read] = (sys_handler_t)sys_read,
    [SYS_write] = (sys_handler_t)sys_write,
    [SYS_close] = (sys_handler_t)sys_close,
    [SYS_lseek] = (sys_handler_t)sys_lseek,
    [SYS_ioctl] = (sys_handler_t)sys_ioctl,


    [SYS_fstat] = (sys_handler_t)sys_fstat,
    [SYS_isatty] = (sys_handler_t)sys_isatty,
    [SYS_sbrk] = (sys_handler_t)sys_sbrk,

    [SYS_dup] = (sys_handler_t)sys_dup,

    [SYS_opendir] = (sys_handler_t)sys_opendir,
    [SYS_readdir] = (sys_handler_t)sys_readdir,
    [SYS_closedir] = (sys_handler_t)sys_closedir,

    [SYS_unlink] = (sys_handler_t)sys_unlink,


};
void do_handler_syscall(syscall_frame_t *frame)
{
    if(frame->func_id < sizeof(sys_handler_table) / sizeof(sys_handler_t))
    {
        sys_handler_t handler = sys_handler_table[frame->func_id];
        if(handler)
        {
            int ret = handler(frame->arg0,frame->arg1,frame->arg2,frame->arg3);
            frame->eax = ret;
            return;
        }
    }

    task_t *task = task_current();
    log_printf("task: %s, unknow syscall: %d\n",task->name,frame->func_id);
    frame->eax = -1;
}