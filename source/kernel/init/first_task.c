#include "core/task.h"
#include "tools/log.h"
#include "core/memory.h"
#include "cpu/mmu.h"


static uint32_t init_task_stack[1024];
static task_t init_task;
void init_task_entry(void)
{
    // 测试1：最简单的死循环
    for(;;) {
        // 什么都不做，看看是否死掉
    }
}
int first_task_main()
{
    user_task_init(&init_task,"init_task",(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    
    // 将init_task_entry函数映射到用户空间
    uint32_t func_addr = (uint32_t)init_task_entry;
    uint32_t func_page = func_addr & ~(MEM_PAGE_SIZE - 1);  // 页对齐
    int ret = memory_alloc_page_for(func_page, MEM_PAGE_SIZE, PTE_P | PTE_W | PTE_U);
    if(ret < 0)
    {
        log_printf("memory_alloc_page_for error\n");
        return -1;
    }
    for(;;)
    {
        log_printf("first task\n");
        schedule_next_task(task_current(), &init_task);
        //sys_sleep(1000);
    }
    return 0;
} 