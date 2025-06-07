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
static uint32_t kernel_stack[1024];
static int tss_init(task_t *task,uint32_t entry,uint32_t esp, int kernel_or_user)
{
    kernel_memset(&task->tss,0,sizeof(tss_t));
    task->kernel_stack = esp;
    //0表示内核任务，1表示用户任务
    if(kernel_or_user == 0)
    {
        task->tss.cs = KERNEL_SELECTOR_CS;    // 内核代码段
        task->tss.ds = KERNEL_SELECTOR_DS;    // 内核数据段
        task->tss.ss = KERNEL_SELECTOR_DS;    // 内核栈段
        task->tss.eflags = EFLAGS_DEFAULT| EFLAGS_IF; // 开启中断
    }
    else
    {
        task->tss.cs = USER_SELECTOR_CS;    // 用户代码段
        task->tss.ds = USER_SELECTOR_DS;    // 用户数据段
        task->tss.ss = USER_SELECTOR_DS;    // 用户栈段
        task->tss.eflags = EFLAGS_DEFAULT| EFLAGS_IF; // 开启中断
    }
    task->tss.esp = esp;
    task->tss.eip = entry;
    uint32_t page_dir = memory_create_user_space();
    if(page_dir == 0)
    {
        return -1;
    }
    task->tss.cr3 = page_dir;
    return 0;
}
int kernel_task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp)
{
    ASSERT(task != (task *)0);
    tss_init(task,entry,esp,0);

    task->task_type = TASK_KERNEL;

    kernel_strncpy(task->name,name,TASK_NAME_SIZE);
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->sleep_ticks = 0;
    task->state = TASK_CREATED;
    
    list_node_init(&task->run_node);
    list_node_init(&task->all_node);
    list_node_init(&task->wait_node);
    irq_state_t state = irq_enter_protection();
    task_set_ready(task);
    list_insert_last(&task_manager.task_list,&task->all_node);
    irq_leave_protection(state);
    return 0;
}


int user_task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp)
{
    ASSERT(task != (task *)0);
    task->task_type = TASK_USER;
    tss_init(task,entry,esp,1);
    kernel_strncpy(task->name,name,TASK_NAME_SIZE);
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->sleep_ticks = 0;
    task->state = TASK_CREATED;
    
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
static void global_tss_init()
{
    task_manager.global_tss.ss0 = KERNEL_SELECTOR_DS;
    task_manager.global_tss.esp0 = (uint32_t)&kernel_stack[1024];
    task_manager.global_tss.cs = KERNEL_SELECTOR_CS;
    task_manager.global_tss.ds = KERNEL_SELECTOR_DS;
    task_manager.global_tss.es = KERNEL_SELECTOR_DS;
    task_manager.global_tss.fs = KERNEL_SELECTOR_DS;
    task_manager.global_tss.gs = KERNEL_SELECTOR_DS;
}
static void task_gdt_init()
{
    // 直接配置用户段到对应的GDT表项，确保段基址为0  
    int sel = gdt_alloc_desc();
    segment_desc_set(sel, 0, 0xFFFFFFFF, SEG_P_PRESENT | SEG_DPL_3 | SEG_S_NORMAL |
        SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);  // 索引3，对应选择子0x1B (0x18+RPL_3)
    sel = gdt_alloc_desc();
    segment_desc_set(sel, 0, 0xFFFFFFFF, SEG_P_PRESENT | SEG_DPL_3 | SEG_S_NORMAL |
        SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);  // 索引4，对应选择子0x23 (0x20+RPL_3)
    sel = gdt_alloc_desc();
    segment_desc_set(sel, (uint32_t)&task_manager.global_tss, sizeof(tss_t),
            SEG_P_PRESENT | SEG_DPL_0 | SEG_TYPE_TSS);
    task_manager.tss_sel = sel;
}
void task_manager_init()
{
    task_gdt_init();
    global_tss_init();
    
    
    // show_base(3);
    // show_base(4);
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);     
    list_init(&task_manager.sleep_list);
    task_manager.current_task = (task_t *)0;
    task_manager.from_task = (task_t *)0;
    task_manager.to_task = (task_t *)0;
    task_manager.need_reschedule = TASK_NOT_NEED_RESCHEDULE ;

    kernel_task_init(&task_manager.idle_task,"idle_task",idle_task_entry,(uint32_t)&idle_task_stack[1024]);
}

void task_first_init()
{
    void first_task_entry(void);
    extern uint8_t s_first_task[];
    extern uint8_t e_first_task[];

    uint32_t copy_size = (uint32_t)(e_first_task - s_first_task);
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;
    
    uint32_t first_start = (uint32_t)first_task_entry;
    kernel_task_init(&task_manager.first_task,"first_task",first_start,0);
    write_tr(task_manager.tss_sel);
    task_manager.current_task = &task_manager.first_task;
    mmu_set_page_dir_task(task_manager.current_task);

    memory_alloc_page_for(first_start,alloc_size,PTE_P | PTE_W | PTE_U);
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
    //do_schedule_switch();
    schedule_next_task();
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
    if(task_manager.need_reschedule || task_current() == (task_t *)0 )
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

void universal_task_switch();
void schedule_next_task() {
    
        task_t *old =  task_manager.from_task;
        task_t *next_task = task_manager.to_task;
        if (next_task && next_task != old) {
        // 一个函数搞定所有切换！
        universal_task_switch(
            old ? &old->tss : (void *)0,               // 保存当前任务的TSS指针
            next_task->task_type,                       // 任务类型 (0=内核, 1=用户)
            next_task->tss.cr3,                         // 新任务页目录(CR3)
            next_task->tss.cs,                          // 新任务CS段选择子
            next_task->tss.eip,                         // 新任务EIP入口地址
            next_task->tss.eflags,                      // 新任务EFLAGS标志
            next_task->tss.ss,                          // 新任务SS段选择子
            next_task->tss.esp,                         // 新任务ESP栈指针
            next_task->tss.eax,                         // 新任务EAX寄存器
            next_task->tss.ebx,                         // 新任务EBX寄存器
            next_task->tss.ecx,                         // 新任务ECX寄存器
            next_task->tss.edx,                         // 新任务EDX寄存器
            next_task->tss.esi,                         // 新任务ESI寄存器
            next_task->tss.edi,                         // 新任务EDI寄存器
            next_task->tss.ebp                          // 新任务EBP寄存器
        );
    }
}

