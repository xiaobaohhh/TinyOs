#include "lib_syscall.h"
#include <stdlib.h>
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
void msleep(int ms)
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
int getpid()
{
    syscall_args_t args;
    args.id = SYS_getpid;
    return sys_call(&args);
}
void print_msg(const char *msg,int arg)
{
    syscall_args_t args;
    args.id = SYS_print_msg;
    args.arg0 = (int)msg;
    args.arg1 = arg;
    sys_call(&args);
}

int fork()
{
    syscall_args_t args;
    args.id = SYS_fork;
    return sys_call(&args);
}

int execve(const char *name,char * const *argv,char * const *env)
{
    syscall_args_t args;
    args.id = SYS_execve;
    args.arg0 = (int)name;
    args.arg1 = (int)argv;
    args.arg2 = (int)env;
    return sys_call(&args);
}
int yield()
{
    syscall_args_t args;
    args.id = SYS_yield;
    return sys_call(&args);
}

int open(const char *name,int flags, ...)
{
    syscall_args_t args;
    args.id = SYS_open;
    args.arg0 = (int)name;
    args.arg1 = (int)flags;
    return sys_call(&args);
}
int read(int file, char *ptr, int len)
{
    syscall_args_t args;
    args.id = SYS_read;
    args.arg0 = (int)file;
    args.arg1 = (int)ptr;
    args.arg2 = (int)len;
    return sys_call(&args);
}
int write(int file, char *ptr, int len)
{
    syscall_args_t args;
    args.id = SYS_write;
    args.arg0 = (int)file;
    args.arg1 = (int)ptr;
    args.arg2 = (int)len;
    return sys_call(&args);
}
int close(int file)
{
    syscall_args_t args;
    args.id = SYS_close;
    args.arg0 = (int)file;
    return sys_call(&args);
}
int lseek(int file, int ptr, int dir)
{
    syscall_args_t args;
    args.id = SYS_lseek;
    args.arg0 = (int)file;
    args.arg1 = (int)ptr;
    args.arg2 = (int)dir;
    return sys_call(&args);
}

int isatty(int file)
{
    syscall_args_t args;
    args.id = SYS_isatty;
    args.arg0 = (int)file;
    return sys_call(&args);
}

int fstat(int file, struct stat * st)
{
    syscall_args_t args;    
    args.id = SYS_fstat;
    args.arg0 = (int)file;
    args.arg1 = (int)st;
    return sys_call(&args);
}

void * sbrk(ptrdiff_t incr)
{
    syscall_args_t args;
    args.id = SYS_sbrk;
    args.arg0 = (int)incr;
    return (void *)sys_call(&args);
}

int dup(int file)
{
    syscall_args_t args;
    args.id = SYS_dup;
    args.arg0 = (int)file;
    return sys_call(&args);
}

void _exit(int status)
{
    syscall_args_t args;
    args.id = SYS_exit;
    args.arg0 = (int)status;
    sys_call(&args);
}

int wait(int * status)
{
    syscall_args_t args;
    args.id = SYS_wait;
    args.arg0 = (int)status;
    return sys_call(&args);
}

DIR * opendir(const char * path)
{
    DIR * dir = (DIR *)malloc(sizeof(DIR));
    if(!dir)
    {
        return (DIR *)0;
    }
    syscall_args_t args;
    args.id = SYS_opendir;
    args.arg0 = (int)path;
    args.arg1 = (int)dir;
    int ret = sys_call(&args);
    if(ret < 0)
    {
        free(dir);
        return (DIR *)0;
    }
    return dir;
}

struct dirent * readdir(DIR * dir)
{
    syscall_args_t args;
    args.id = SYS_readdir;
    args.arg0 = (int)dir;
    args.arg1 = (int)&dir->dirent;
    int ret = sys_call(&args);
    if(ret < 0)
    {
        return (struct dirent *)0;
    }
    return &dir->dirent;
}

int closedir(DIR * dir)
{
    syscall_args_t args;
    args.id = SYS_closedir;
    args.arg0 = (uint32_t)dir;
    sys_call(&args);
    free(dir);
    return 0;
}


int ioctl(int file,int cmd,int arg0,int arg1)
{
    syscall_args_t args;
    args.id = SYS_ioctl;
    args.arg0 = (int)file;
    args.arg1 = (int)cmd;
    args.arg2 = (int)arg0;
    args.arg3 = (int)arg1;
    return sys_call(&args);
}

int unlink(const char * pathname)
{
    syscall_args_t args;
    args.id = SYS_unlink;
    args.arg0 = (int)pathname;
    return sys_call(&args);
}
