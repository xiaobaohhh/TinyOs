/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-07 22:07:18
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-12 15:25:29
 * @FilePath: \start\source\applib\lib_syscall.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "core/syscall.h"
#include "os_cfg.h"
#include "core/task.h"
#include <sys/stat.h>

typedef struct _syscall_args_t
{
    int id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
}syscall_args_t;




void msleep(int ms);
int getpid();
void print_msg(const char *msg,int arg);

int fork();

int execve(const char *name,char * const *argv,char * const *env);
int yield();
int open(const char *name,int flags, ...);
int read(int file, char *ptr, int len);
int write(int file, char *ptr, int len);
int close(int file);
int lseek(int file, int ptr, int dir);


int ioctl(int file,int cmd,int arg0,int arg1);
int isatty(int file);
int fstat(int file, struct stat * st);
void * sbrk(ptrdiff_t incr);

int dup(int file);

void _exit(int status);
int wait(int * status);


int unlink(const char * path);
struct dirent
{
    int index;
    int type;
    char name[255];
    int size;
};
typedef struct _DIR
{
    int index;
    struct dirent dirent;
}DIR;
DIR * opendir(const char * path);

struct dirent * readdir(DIR * dir);
int closedir(DIR * dir);
#endif
