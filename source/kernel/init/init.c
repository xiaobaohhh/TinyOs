/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-27 13:09:35
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-06-02 17:31:40
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

void kernel_init(boot_info_t * boot_info)
{
    //ASSERT(boot_info->ram_region_count != 2);
    cpu_init();
    log_init(); 
    irq_init();
    time_init();

    task_manager_init();
}

static task_t init_task;
static uint32_t init_task_stack[1024];

static task_t idle_task;
static uint32_t idle_task_stack[1024];

void init_task_entry(void)
{
    //test();
    int count = 0;
    for(;;)
    {
        //log_printf("init task count = %d\n", count++);
        
    }
}

void idle_task_entry(void)
{
    int count = 0;
    for(;;)
    {
       //log_printf("init idle count = %d\n", count++);
    }
}

void init_main(void)
{
    
    
    //int a = 3 / 0;
    log_printf("init main\n");
    task_init(&init_task,"init_task",(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    task_init(&idle_task,"idle_task",(uint32_t)idle_task_entry,(uint32_t)&idle_task_stack[1024]);
    task_first_init();

    irq_enable_global();
    int count = 0;
    for(;;)
    {
        log_printf("init main count = %d\n", count++);
        sys_sleep(5000);
    }
}
