/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-27 13:09:35
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-02 22:13:03
 * @FilePath: \start\source\kernel\init\init.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "init.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "core/task.h"
#include "comm/cpu_instr.h"
#include "tools/list.h"
#include "ipc/sem.h"
#include "ipc/mutex.h"
#include "core/memory.h"
#include "dev/time.h"
void kernel_init(boot_info_t * boot_info)
{
    //ASSERT(boot_info->ram_region_count != 2);
    cpu_init();
    memory_init(boot_info);
    log_init(); 
    irq_init();
    time_init();

    task_manager_init();
}

static task_t init_task;
static uint32_t init_task_stack[1024];

static task_t test_task;
static uint32_t test_task_stack[1024];

static sem_t sem;
static mutex_t mutex;
void init_task_entry(void)
{
    int count = 0;
    for(;;)
    {
        sys_sleep(3000);
        mutex_lock(&mutex);
        log_printf("init task count = %d\n", count++);
        sys_sleep(1000);
        mutex_unlock(&mutex);
    }
}

void test_task_entry(void)
{
    int count = 0;
    for(;;)
    {
        sys_sleep(4000);
        mutex_lock(&mutex);
        log_printf("init idle count = %d\n", count++);
        sys_sleep(1000);
        mutex_unlock(&mutex);
    }
}

void init_main(void)
{
    sem_init(&sem,0);
    log_printf("init main\n");
    task_init(&init_task,"init_task",(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    task_init(&test_task,"test_task",(uint32_t)test_task_entry,(uint32_t)&test_task_stack[1024]);
    task_first_init();

    irq_enable_global();
    int count = 0;
    for(;;)
    {
        mutex_lock(&mutex);
        log_printf("init main count = %d\n", count++);
        sys_sleep(3000);
        mutex_unlock(&mutex);
    }
}
