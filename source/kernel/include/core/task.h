/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-30 16:07:56
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-23 16:57:30
 * @FilePath: \start\source\kernel\include\core\task.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef TASK_H
#define TASK_H

#include "cpu/cpu.h"
#include "comm/types.h"
#include "tools/list.h"
#include "fs/file.h"
#define TASK_NAME_SIZE 32
#define TASK_TIME_SLICE_DEFAULT 10

#define TASK_NEED_RESCHEDULE 1
#define TASK_NOT_NEED_RESCHEDULE 0
#define TASK_OFILE_NR 128
typedef struct _task_args_t
{
    uint32_t ret_addr;
    uint32_t argc;
    char **argv;
}task_args_t;
typedef struct _task_t
{
    uint32_t * stack;
    enum{
        TASK_CREATED,
        TASK_RUNNING,
        TASK_READY,
        TASK_SLEEP,
        TASK_SUSPEND,
        TASK_WAITTING,
        TASK_STOP,
        TASK_ZOMBIE,
    }state;

    enum{
        TASK_KERNEL,    // 内核任务
        TASK_USER       // 用户任务
    }task_type;

    uint32_t heap_start;
    uint32_t heap_end;
    int pid;

    struct _task_t * parent;
    int sleep_ticks;
    int slice_ticks;
    int time_ticks;
    char name[TASK_NAME_SIZE];                                  
    list_node_t run_node;
    list_node_t all_node;
    list_node_t wait_node;
    uint32_t esp0_start;
    tss_t tss;
    int is_first_run;

    int status;
    file_t * file_table[TASK_OFILE_NR];
}task_t;

int kernel_task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp);
int user_task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp);
void task_time_tick();


int task_alloc_fd(file_t *file);
void task_remove_fd(int fd);
file_t * task_file(int fd);

typedef struct _task_manager_t
{

    uint32_t need_reschedule;
    task_t * from_task;
    task_t * to_task;
    task_t * current_task;

    list_t ready_list;
    list_t sleep_list;
    list_t task_list;
    
    task_t  first_task;

    task_t  idle_task;
    
    tss_t global_tss;

    int tss_sel;
    
}task_manager_t;

void task_manager_init();
void task_first_init();
task_t * task_first_task();

void task_set_ready(task_t *task);
void task_set_block(task_t *task);
int sys_sched_yield();
task_t * task_next_run();
void task_dispatch();
task_t * task_current();

uint32_t task_is_need_reschedule();
task_t * get_from_task();
task_t * get_to_task();


void sys_sleep(uint32_t ms);
void task_set_sleep(task_t *task,uint32_t ticks);
void task_set_wakeup(task_t *task);

static void idle_task_entry(void);
void schedule_switch(void);
void do_schedule_switch(void);
void mmu_set_page_dir_task(task_t * to_task);

void schedule_next_task();

int sys_getpid();
int sys_fork();
int sys_execve(char *name,char **argv,char **env);
void sys_exit(int status);
int sys_wait(int * status);
#endif

