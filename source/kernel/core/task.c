#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "cpu/cpu.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "core/memory.h"
#include "cpu/mmu.h"
#include "core/syscall.h"
#include "comm/elf.h"
#include "fs/fs.h"
#include "comm/types.h"
static task_manager_t task_manager;
int need_reschedule = 0;
static uint32_t idle_task_stack[1024];
static uint32_t kernel_stack[1024];

static task_t task_table[TASK_COUNT];
static mutex_t task_table_mutex;     

int task_alloc_fd(file_t *file)
{
    task_t * task = task_current();
    for(int i = 0; i < TASK_OFILE_NR; i++)
    {
        file_t * f = task->file_table[i];
        if(f == (file_t *)0)
        {
            task->file_table[i] = file;
            return i;
        }
    }
    return -1;
}
file_t * task_file(int fd)
{
    if(fd >= 0 && fd < TASK_OFILE_NR)
    {
        file_t * file = task_current()->file_table[fd];
        return file;
    }
    return (file_t *)0;
}
void task_remove_fd(int fd)
{
    if(fd > 0 && fd < TASK_OFILE_NR)
    {
        task_current()->file_table[fd] = 0;
    }
}
static int tss_init(task_t *task,uint32_t entry,uint32_t esp, int kernel_or_user)
{
    kernel_memset(&task->tss,0,sizeof(tss_t));

    uint32_t kernel_stack= memory_alloc_page();
    if(kernel_stack == 0)
    {
        goto tss_init_failed;
    }
    task->tss.esp0 = kernel_stack + MEM_PAGE_SIZE;
    task->esp0_start = kernel_stack + MEM_PAGE_SIZE;
    
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
        //memory_destroy_uvm(page_dir);
        goto tss_init_failed;
    }
    task->tss.cr3 = page_dir;
    return 0;
tss_init_failed:
    if(kernel_stack)
    {
        memory_free_page(kernel_stack);
    }
    if(page_dir)
    {
        memory_destroy_uvm(page_dir);
    }
    return -1;
}

void task_uninit(task_t *task)
{
    if(task->tss.cr3)
    {
        memory_destroy_uvm(task->tss.cr3);
    }
    kernel_memset(task,0,sizeof(task_t));
}
int kernel_task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp)
{
    ASSERT(task != (task *)0);
    tss_init(task,entry,esp,0);

    task->is_first_run = 1;
    task->task_type = TASK_KERNEL;

    kernel_strncpy(task->name,name,TASK_NAME_SIZE);
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->sleep_ticks = 0;
    task->state = TASK_CREATED;
    task->parent = (task_t *)0;
    task->heap_start = 0;
    task->heap_end = 0;
    task->pid = (uint32_t)task;
    task->status = 0;
    list_node_init(&task->run_node);
    list_node_init(&task->all_node);
    list_node_init(&task->wait_node);

    kernel_memset(task->file_table,0,sizeof(task->file_table));
    irq_state_t state = irq_enter_protection();
    //task_set_ready(task);
    list_insert_last(&task_manager.task_list,&task->all_node);
    irq_leave_protection(state);
    return 0;
}


int user_task_init(task_t *task,const char *name,uint32_t entry,uint32_t esp)
{
    ASSERT(task != (task *)0);
    task->task_type = TASK_USER;
    task->is_first_run = 1;
    tss_init(task,entry,esp,1);
    kernel_strncpy(task->name,name,TASK_NAME_SIZE);
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->sleep_ticks = 0;
    task->state = TASK_CREATED;
    task->parent = (task_t *)0;
    task->heap_start = 0;
    task->heap_end = 0;
    task->pid = (uint32_t)task;
    task->status = 0;
    list_node_init(&task->run_node);
    list_node_init(&task->all_node);
    list_node_init(&task->wait_node);
    kernel_memset(task->file_table,0,sizeof(task->file_table));
    irq_state_t state = irq_enter_protection();
    //task_set_ready(task);
    list_insert_last(&task_manager.task_list,&task->all_node);
    irq_leave_protection(state);
    return 0;
}
void task_start(task_t *task)
{
    irq_state_t state = irq_enter_protection();
    task_set_ready(task);
    irq_leave_protection(state);
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
    task_manager.global_tss.esp0 = 0;
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

    kernel_memset(&task_table,0,sizeof(task_table));
    mutex_init(&task_table_mutex);
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

    kernel_task_init(&task_manager.idle_task,"idle_task",idle_task_entry,&idle_task_stack[1024]);
    
    task_start(&task_manager.idle_task);
}

void task_first_init()
{
    void first_task_entry(void);
    extern uint8_t s_first_task[];
    extern uint8_t e_first_task[];

    uint32_t copy_size = (uint32_t)(e_first_task - s_first_task);
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;
    
    uint32_t first_start = (uint32_t)first_task_entry;
    kernel_task_init(&task_manager.first_task,"first_task",first_start,first_start + alloc_size);
    task_manager.global_tss.esp0 = task_manager.first_task.tss.esp0;
    
    task_manager.first_task.heap_start = (uint32_t )e_first_task;
    task_manager.first_task.heap_end = (uint32_t )e_first_task;
    write_tr(task_manager.tss_sel);
    task_manager.current_task = &task_manager.first_task;
    mmu_set_page_dir_task(task_manager.current_task);
  
    memory_alloc_page_for(first_start,alloc_size,PTE_P | PTE_W | PTE_U);
    kernel_memcpy((void *)first_start,s_first_task,copy_size);
    task_start(&task_manager.first_task);
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

        schedule_switch();
    }
    irq_leave_protection(state);
    return 0;
}

void sys_exit(int status)
{
    task_t * current_task = task_current();
    for( int fd = 0; fd < TASK_OFILE_NR; fd++)
    {
        file_t * file = current_task->file_table[fd];
        if(file)
        {
            sys_close(fd);
            current_task->file_table[fd] = (file_t *)0;
        }
    }
    int move_child = 0;
    mutex_lock(&task_table_mutex);
    for(int i = 0; i < TASK_COUNT; i++)
    {
        task_t * task = task_table + i;
        if(task->parent == current_task )
        {
            task->parent = &task_manager.first_task;
            if(task->state = TASK_ZOMBIE)
            {
                move_child = 1;
            }
        }
    }
    mutex_unlock(&task_table_mutex);
    //irq_state_t state = irq_enter_protection();

    task_t * parent = current_task->parent;
    if(move_child && (parent != &task_manager.first_task))
    {
        if(task_manager.first_task.state == TASK_WAITTING)
        {
            task_set_ready(&task_manager.first_task);
        }
    }
    if(parent && parent->state == TASK_WAITTING)
    {
        task_set_ready(current_task->parent);
    }
    current_task->state = TASK_ZOMBIE;
    current_task->status = status;
    task_set_block(current_task);
    //irq_leave_protection(state);
    schedule_switch();
    
    while(1)
    {
        hlt();
    }
}

int sys_wait(int * status)
{
    task_t * current_task = task_current();
    for(;;)
    {
        //mutex_lock(&task_table_mutex);
        for(int i = 0; i < TASK_COUNT; i++)
        {
            task_t * task = task_table + i;
            if(task->parent == current_task && task->state == TASK_ZOMBIE)
            {
                int pid = task->pid;
                *status = task->status;
                memory_destroy_uvm(task->tss.cr3);
                memory_free_page(task->tss.esp0 - MEM_PAGE_SIZE);
                kernel_memset(task,0,sizeof(task_t));
                //mutex_unlock(&task_table_mutex);
                return pid;
            }
            
        }
        //irq_state_t state = irq_enter_protection();
        task_set_block(current_task);
        current_task->state = TASK_WAITTING;
        schedule_switch();
        //irq_leave_protection(state);

    }
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
    irq_state_t state = irq_enter_protection();
    task_dispatch();
    //do_schedule_switch();
    schedule_next_task();
    irq_leave_protection(state);
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
        //task_dispatch();
        schedule_switch();
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

int sys_getpid()
{
    return task_current()->pid;
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

    //task_dispatch();
    schedule_switch();
    irq_leave_protection(state);
}

void universal_task_switch();
void schedule_next_task() {
    
        task_t *old =  task_manager.from_task;
        task_t *next_task = task_manager.to_task;
        if(task_manager.need_reschedule == TASK_NEED_RESCHEDULE)
        {
            task_manager.need_reschedule = TASK_NOT_NEED_RESCHEDULE;
        }
        else
        {
            return;
        }
        if (next_task && next_task != old ) {
            uint32_t sesp = 0;
            // if(next_task == &task_manager.idle_task && next_task->is_first_run == 1)
            // {
            //     next_task->is_first_run = 0;
            //     next_task->tss.esp = next_task->tss.esp - (uint32_t)4;
            //     *(uint32_t *)next_task->tss.esp = next_task->tss.eip;
            //     next_task->tss.esp = next_task->tss.esp + (uint32_t)4;
            //     sesp = next_task->tss.esp;
            // }
            // else
            // {
            //     sesp = next_task->tss.esp0;
            // }
            
            if(next_task->task_type == TASK_USER)
            {
                sesp = next_task->tss.esp;
            }
            else if(next_task == &task_manager.idle_task && next_task->is_first_run == 1)
            {
                next_task->is_first_run = 0;
                next_task->tss.esp = next_task->tss.esp - (uint32_t)4;
                *(uint32_t *)next_task->tss.esp = next_task->tss.eip;
                //next_task->tss.esp = next_task->tss.esp + (uint32_t)4;
                sesp = next_task->tss.esp;
            }
            else
            {
                sesp = next_task->tss.esp0;
            }
            
            task_manager.global_tss.esp0 = next_task->esp0_start;//这一行很重要 这是每个任务内核栈的栈底
        // 一个函数搞定所有切换！
        universal_task_switch(
            old ? &old->tss : (void *)0,               // 保存当前任务的TSS指针
            &next_task->task_type,                       // 任务类型 (0=内核, 1=用户)
            next_task->tss.cr3,                         // 新任务页目录(CR3)
            next_task->tss.cs,                          // 新任务CS段选择子
            next_task->tss.eip,                         // 新任务EIP入口地址
            next_task->tss.eflags,                      // 新任务EFLAGS标志
            next_task->tss.ss,                          // 新任务SS段选择子
            //next_task->tss.esp,                         // 新任务ESP栈指针
            sesp,
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


static task_t * task_alloc()
{
    mutex_lock(&task_table_mutex);
    task_t *task = (task_t *)0;
    for(int i = 0; i < TASK_COUNT; i++)
    {
        task_t * curr = task_table + i;
        if(curr->name[0] == '\0')
        {
            task = curr;
            break;
        }
    }
mutex_unlock(&task_table_mutex);
    return task;
}

static void task_free(task_t *task)
{
    mutex_lock(&task_table_mutex);
    task->name[0] = '\0';
    mutex_unlock(&task_table_mutex);
}

static copy_opended_files(task_t * child_task)
{
    task_t * parent = task_current();
    for(int i = 0; i < TASK_OFILE_NR; i++)
    {
        file_t * file = parent->file_table[i];
        if(file)
        {
            file_inc_ref(file);
            child_task->file_table[i] = file;
        }
    }
}
int sys_fork()
{
    task_t * parent = task_current();
    task_t * child = task_alloc();
    if(child == (task_t *)0)
    {
        goto fork_failed;
    }
    syscall_frame_t * frame = (syscall_frame_t *)(task_manager.global_tss.esp0 - sizeof(syscall_frame_t));
    
    
    int err = user_task_init(child,parent->name,frame->eip,frame->esp + sizeof(uint32_t) * SYSCALL_PARAM_COUNT);
    
    copy_opended_files(child);
    if(err < 0)
    {
        goto fork_failed;
    }
    tss_t * tss = &child->tss;
    tss->eax = 0;
    tss->ebx = frame->ebx;
    tss->ecx = frame->ecx;
    tss->edx = frame->edx;
    tss->esi = frame->esi;
    tss->edi = frame->edi;
    tss->ebp = frame->ebp;
    tss->cs = frame->cs;
    tss->ds = frame->ds;
    tss->es = frame->es;
    tss->fs = frame->fs;
    tss->gs = frame->gs;
    tss->eflags = frame->eflags;

    if(memory_copy_vum(parent->tss.cr3,child->tss.cr3)<0)
    {
        goto fork_failed;
    }
    child->parent = parent;
    // tss->cr3 = parent->tss.cr3;
    task_start(child);
    return (uint32_t)child;

    
fork_failed: 
    if(child)
    {
        task_uninit(child);
        task_free(child);
    }
    return -1;
}
static int load_phdr(int file,Elf32_Phdr *elf_phdr,uint32_t new_page_dir)
{
    int err = memory_alloc_for_page_dir(new_page_dir,elf_phdr->p_vaddr,elf_phdr->p_memsz,PTE_P | PTE_W | PTE_U);
    if(err < 0)
    {
        return -1;
    }
    if(sys_lseek(file,elf_phdr->p_offset,0) < 0)
    {
        return -1;
    }
    uint32_t vaddr = elf_phdr->p_vaddr;
    uint32_t size = elf_phdr->p_filesz;
    while(size > 0)
    {
        int cnt_size = size > MEM_PAGE_SIZE ? MEM_PAGE_SIZE : size;
        uint32_t paddr = memory_get_paddr(new_page_dir,vaddr);
        if(sys_read(file,(char *)paddr,cnt_size) < cnt_size)
        {
            return -1;
        }
        vaddr += cnt_size;
        size -= cnt_size;
    }
    return 0;
}
static uint32_t load_elf_file(task_t *task,char *name,uint32_t new_page_dir)
{
    Elf32_Ehdr elf_hdr;
    Elf32_Phdr elf_phdr;
    int file = sys_open(name,0);
    if(file < 0)
    {
        return 0;
        goto load_failed;
    }
    int cnt = sys_read(file,(char *)&elf_hdr,sizeof(Elf32_Ehdr));
    if(cnt < sizeof(Elf32_Ehdr))
    {
        log_printf("load elf file failed,read cnt = %d",cnt);
        goto load_failed;
    }
    if(elf_hdr.e_ident[0] != 0x7f || elf_hdr.e_ident[1] != 'E' 
        || elf_hdr.e_ident[2] != 'L' 
        || elf_hdr.e_ident[3] != 'F')
    {
        log_printf("load elf file failed,elf_hdr.e_ident[0] = %d,elf_hdr.e_ident[1] = %d,elf_hdr.e_ident[2] = %d,elf_hdr.e_ident[3] = %d",elf_hdr.e_ident[0],elf_hdr.e_ident[1],elf_hdr.e_ident[2],elf_hdr.e_ident[3]);
        goto load_failed;
    }
    uint32_t e_phoff = elf_hdr.e_phoff;
    for(int i = 0; i < elf_hdr.e_phnum; i++,e_phoff += elf_hdr.e_phentsize)
    {
        if(sys_lseek(file,e_phoff,0) < 0)
        {
            log_printf("load elf file failed,sys_lseek failed");
            goto load_failed;
        }
        cnt = sys_read(file,(char *)&elf_phdr,sizeof(elf_phdr));
        if(cnt < sizeof(elf_phdr))
        {
            log_printf("load elf file failed,read cnt = %d",cnt);
            goto load_failed;
        }
        if(elf_phdr.p_type != PT_LOAD || elf_phdr.p_vaddr < MEM_PAGE_SIZE)
        {
            continue;
        }
        int err = load_phdr(file,&elf_phdr,new_page_dir);
        if(err < 0)
        {
            log_printf("load elf file failed,load_phdr failed");
            goto load_failed;
        }

        task->heap_start = elf_phdr.p_vaddr + elf_phdr.p_memsz;
        task->heap_end = task->heap_start;

    }
    
    sys_close(file);
    return elf_hdr.e_entry;
load_failed:
    if(file)
    {
        sys_close(file);
    }
    return 0;
}

static int copy_args (char * to, uint32_t page_dir, int argc, char **argv) {
    // 在stack_top中依次写入argc, argv指针，参数字符串
    task_args_t task_args;
    task_args.argc = argc;
    task_args.argv = (char **)(to + sizeof(task_args_t));

    // 复制各项参数, 跳过task_args和参数表
    // 各argv参数写入的内存空间
    char * dest_arg = to + sizeof(task_args_t) + sizeof(char *) * (argc + 1);   // 留出结束符
    
    // argv表
    char ** dest_argv_tb = (char **)memory_get_paddr(page_dir, (uint32_t)(to + sizeof(task_args_t)));
    ASSERT(dest_argv_tb != 0);

    for (int i = 0; i < argc; i++) {
        char * from = argv[i];

        // 不能用kernel_strcpy，因为to和argv不在一个页表里
        int len = kernel_strlen(from) + 1;   // 包含结束符
        int err = memory_copy_uvm_data((uint32_t)dest_arg, page_dir, (uint32_t)from, len);
        ASSERT(err >= 0);

        // 关联ar
        dest_argv_tb[i] = dest_arg;

        // 记录下位置后，复制的位置前移
        dest_arg += len;
    }

    // 可能存在无参的情况，此时不需要写入
    if (argc) {
        dest_argv_tb[argc] = '\0';
    }

     // 写入task_args
    return memory_copy_uvm_data((uint32_t)to, page_dir, (uint32_t)&task_args, sizeof(task_args_t));
}
int sys_execve(char *name,char **argv,char **env)
{
    task_t * task = task_current();

    kernel_strncpy(task->name,get_file_name(name),TASK_NAME_SIZE);
    uint32_t old_page_dir = task->tss.cr3;
    uint32_t new_page_dir = memory_create_user_space();
    if(new_page_dir == 0)
    {
        goto execve_failed;
    }
    uint32_t entry = load_elf_file(task,name,new_page_dir);
    if(entry == 0)
    {
        goto execve_failed;
    }

    uint32_t stack_up = MEM_TASK_STACK_TOP - MEM_TASK_ARG_SIZE;
    int err = memory_alloc_for_page_dir(new_page_dir,MEM_TASK_STACK_TOP - MEM_TASK_STACK_SIZE,MEM_TASK_STACK_SIZE,PTE_P | PTE_W | PTE_U);
    if(err < 0)
    {
        goto execve_failed;
    }
    
    int argc = strings_count(argv);
    err = copy_args((char *)stack_up,new_page_dir,argc,argv);
    if(err < 0)
    {
        goto execve_failed;        
    }
    syscall_frame_t * frame = (syscall_frame_t *)(task_manager.global_tss.esp0 - sizeof(syscall_frame_t));
    frame->eip = entry;
    frame->eax = frame->ebx = frame->ecx = frame->edx = frame->esi = frame->edi = frame->ebp = 0;
    frame->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    frame->esp = stack_up - sizeof(uint32_t) * SYSCALL_PARAM_COUNT;
    frame->cs = USER_SELECTOR_CS;
    frame->ds = USER_SELECTOR_DS;
    frame->es = USER_SELECTOR_DS;
    frame->fs = USER_SELECTOR_DS;
    frame->gs = USER_SELECTOR_DS;


    task->tss.cr3 = new_page_dir;
    mmu_set_page_dir(new_page_dir);
    memory_destroy_uvm(old_page_dir);
    return 0;
execve_failed:

    if(new_page_dir != 0)
    {
        task->tss.cr3 = old_page_dir;
        mmu_set_page_dir(old_page_dir);
        memory_destroy_uvm(new_page_dir);
    }
    return -1;
}