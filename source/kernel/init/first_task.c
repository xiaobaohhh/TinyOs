/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-05 13:18:46
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-07 11:37:30
 * @FilePath: \start\source\kernel\init\first_task.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "core/task.h"
#include "tools/log.h"
#include "core/memory.h"
#include "cpu/mmu.h"


static uint32_t init_task_stack[1024];
static uint32_t init_task_stack_test[1024];

static task_t init_task;
void init_task_entry(void)
{
    // 测试1：最简单的死循环
    for(;;) {
        // 什么都不做，看看是否死掉
        log_printf("init task\n");
    }
} 
int first_task_main()
{
    kernel_task_init(&init_task,"init_task",(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    // 将init_task_entry函数映射到用户空间
    uint32_t func_addr = (uint32_t)init_task_entry;
    uint32_t func_page = func_addr & ~(MEM_PAGE_SIZE - 1);  // 页对齐
    memory_copy(task_current(),&init_task);
    //memory_alloc_for_page_dir(init_task.tss.cr3,func_page,2*MEM_PAGE_SIZE,PTE_P | PTE_W | PTE_U);
    for(;;)
    {
        log_printf("first task\n");
        //schedule_next_task(task_current(), &init_task);
        //sys_sleep(1000);
    }
    return 0;
} 