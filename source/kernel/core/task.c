#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "cpu/cpu.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "core/memory.h"
#include "cpu/mmu.h"
static task_manager_t task_manager;
int need_reschedule = 0;
static uint32_t idle_task_stack[1024];
static int tss_init(task_t *task,uint32_t entry,uint32_t esp)
{
    int tss_sel = gdt_alloc_desc();
    if(tss_sel == -1)
    {
        log_printf("tss_init: gdt_alloc_desc failed\n");
        return -1;
    }
    segment_desc_set(tss_sel,(uint32_t)&task->tss,sizeof(tss_t),
        SEG_P_PRESENT | SEG_DPL_0 | SEG_TYPE_TSS);
    kernel_memset(&task->tss,0,sizeof(tss_t));

    int code_sel = task_manager.app_code_sel | SEG_CPL_3;
    int data_sel = task_manager.app_data_sel | SEG_CPL_3;
    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss = task->tss.ss0 = data_sel;
    task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = data_sel;
    task->tss.cs = code_sel;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss_sel = tss_sel;
    uint32_t page_dir = memory_create_user_space();
    if(page_dir == 0)
    {
        gdt_free_desc(tss_sel);
        return -1;
    }
    task->tss.cr3 = page_dir;
    return 0;
}
int task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp)
{
    ASSERT(task != (task *)0);
    //tss_init(task,entry,esp);
    uint32_t * pesp = (uint32_t *)esp;
    if(pesp)
    {
       *(--pesp) = entry;              // 任务入口地址
        *(--pesp) = 0;                  // ebp
        *(--pesp) = 0;                  // ebx
        *(--pesp) = 0;                  // esi
        *(--pesp) = 0;                  // edi
        *(--pesp) = EFLAGS_DEFAULT | EFLAGS_IF; // EFLAGS，允许中断
        // *(--pesp) = task_manager.app_code_sel; // cs
        // *(--pesp) = task_manager.app_data_sel; // ds
        task->stack = pesp;
    }
    kernel_strncpy(task->name,name,TASK_NAME_SIZE);
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->sleep_ticks = 0;
    task->state = TASK_CREATED;
    uint32_t page_dir = memory_create_user_space();
    if(page_dir == 0)
    {
        return -1;
    }
    task->tss.cr3 = page_dir;
    list_node_init(&task->run_node);
    list_node_init(&task->all_node);
    list_node_init(&task->wait_node);
    irq_state_t state = irq_enter_protection();
    task_set_ready(task);
    list_insert_last(&task_manager.task_list,&task->all_node);
    irq_leave_protection(state);
    return 0;
}
void simple_switch(uint32_t ** from,uint32_t *to);
void interrupt_switch(uint32_t ** from,uint32_t *to);
static void idle_task_entry(void)
{
    while(1)
    {
        hlt();
    }
}
void task_manager_init()
{
    int sel = gdt_alloc_desc();
    segment_desc_set(sel,0,0xFFFFFFFF,SEG_P_PRESENT | SEG_DPL_3 | 
        SEG_TYPE_DATA | SEG_TYPE_RW | SEG_DPL_3);
    task_manager.app_data_sel = sel | SEG_CPL_3;
    sel = gdt_alloc_desc();
    segment_desc_set(sel,0,0xFFFFFFFF,SEG_P_PRESENT | SEG_DPL_3 | 
        SEG_TYPE_CODE | SEG_TYPE_RW | SEG_DPL_3);
    task_manager.app_code_sel = sel | SEG_CPL_3;
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    list_init(&task_manager.sleep_list);

    task_manager.current_task = (task_t *)0;
    task_manager.from_task = (task_t *)0;
    task_manager.to_task = (task_t *)0;
    task_manager.need_reschedule = TASK_NOT_NEED_RESCHEDULE ;

    task_init(&task_manager.idle_task,"idle_task",idle_task_entry,(uint32_t)&idle_task_stack[1024]);
}

void task_first_init()
{
    void first_task_entry(void);
    extern uint8_t s_first_task[];
    extern uint8_t e_first_task[];

    uint32_t copy_size = (uint32_t)(e_first_task - s_first_task);
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;
    
    uint32_t first_start = (uint32_t)first_task_entry;
    task_init(&task_manager.first_task,"first_task",first_start,0);
    write_tr(task_manager.first_task.tss_sel);
    task_manager.current_task = &task_manager.first_task;
    mmu_set_page_dir_task(task_manager.current_task);

    memory_alloc_page_for(first_start,alloc_size,PTE_P | PTE_W);
    kernel_memcpy((void *)first_start,s_first_task,copy_size);

}
task_t * get_from_task()
{
    return task_manager.from_task;
}
task_t * get_to_task()
{
    return task_manager.to_task;
}
uint32_t task_is_need_reschedule()
{
    return task_manager.need_reschedule;
}
task_t * task_first_task()
{
    return &task_manager.first_task;
}

void task_set_ready(task_t *task)
{
    if(task == &task_manager.idle_task)
    {
        return;
    }
    list_insert_last(&task_manager.ready_list,&task->run_node);
    task->state = TASK_READY;  
}

void task_set_block(task_t *task)
{
    if(task == &task_manager.idle_task)
    {
        return;
    }
    list_remove(&task_manager.ready_list,&task->run_node);
}

task_t * task_current()
{
    return task_manager.current_task;
}
int sys_sched_yield()
{
    irq_state_t state = irq_enter_protection();
    if(list_count(&task_manager.ready_list) > 1)
    {
        task_t * current_task = task_current();
        task_set_block(current_task);
        task_set_ready(current_task);

        task_dispatch();
    }
    irq_leave_protection(state);
    return 0;
}

task_t * task_next_run()
{
    if(list_count(&task_manager.ready_list) == 0)
    {
        return &task_manager.idle_task;
    }
    list_node_t * node = list_first(&task_manager.ready_list);
    if(node)
    {
        return list_node_parent(node,task_t,run_node);
    }
    return (task_t *)0;
}

void schedule_switch()
{
    task_dispatch();
    do_schedule_switch();
}
void task_dispatch()
{
    irq_state_t state = irq_enter_protection();
    task_t * next_task = task_next_run();
    if(next_task != task_current())
    {
        task_t * from = task_current();
        task_manager.current_task = next_task;
        next_task->state = TASK_RUNNING;
        task_manager.need_reschedule = TASK_NEED_RESCHEDULE;
        task_manager.from_task = from;
        task_manager.to_task = next_task;
    }
    irq_leave_protection(state);
}

void task_time_tick()
{
    if(task_manager.need_reschedule)
    {
        return; 
    }
    list_node_t * curr = list_first(&task_manager.sleep_list);
    while(curr)
    {
        list_node_t * next = list_node_next(curr);
        task_t * task = list_node_parent(curr,task_t,run_node);
        task->sleep_ticks--;
        if(task->sleep_ticks <= 0)
        {
            task_set_wakeup(task);
            task_set_ready(task);
        }
        curr = next;
    }
    task_t * current_task = task_current();
    current_task->slice_ticks--;
    if(current_task->slice_ticks <= 0)
    {
        current_task->slice_ticks = current_task->time_ticks;
        task_set_block(current_task);
        task_set_ready(current_task);
        task_dispatch();
    }
}

void mmu_set_page_dir_task(task_t * to_task)
{
    if(to_task == (task_t *)0)
    {
        return;
    }
    if(to_task->tss.cr3 != read_cr3() && to_task->tss.cr3 != 0)
    {
        write_cr3(to_task->tss.cr3);
    }
}
void do_schedule_switch(void)
{
    if (task_manager.need_reschedule) {
        task_manager.need_reschedule = TASK_NOT_NEED_RESCHEDULE;
        if (task_manager.from_task && task_manager.from_task != task_manager.to_task) {
            // 在这里进行任务切换
            // 使用简单的栈切换，避免复杂的中断上下文切换
            mmu_set_page_dir_task(task_manager.to_task);
            simple_switch(&task_manager.from_task->stack, task_manager.to_task->stack);
        }
    }
}

void task_set_sleep(task_t *task,uint32_t ticks)
{
    if(ticks == 0)
    {
        return;
    }
    task->sleep_ticks = ticks;
    task->state = TASK_SLEEP;
    list_insert_last(&task_manager.sleep_list,&task->run_node);
}

void task_set_wakeup(task_t *task)
{
    list_remove(&task_manager.sleep_list,&task->run_node);

    //task->state = TASK_READY;
}

void sys_sleep(uint32_t ms)
{
    irq_state_t state = irq_enter_protection();

    
    task_set_block(task_current());

    task_set_sleep(task_current(),(ms + (OS_TICK_MS - 1)) / OS_TICK_MS);

    schedule_switch();
    irq_leave_protection(state);
}


