/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-27 13:09:35
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-21 15:12:22
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
#include "core/task.h"
#include "dev/console.h"
#include "dev/kbd.h"
#include "fs/fs.h"
void kernel_init(boot_info_t * boot_info)
{
    //ASSERT(boot_info->ram_region_count != 2);
    cpu_init();
    irq_init();
    log_init(); 
    memory_init(boot_info);
    fs_init();
    
    time_init();

    task_manager_init();

}



static sem_t sem;
static mutex_t mutex;


void move_to_first_task(void)
{
    // void first_task_entry(void);
    // first_task_entry();
    task_t *curr = task_current();
    tss_t *tss = &curr->tss;
    curr->task_type = TASK_KERNEL;
    tss->cs = USER_SELECTOR_CS;
    tss->ss = USER_SELECTOR_DS;
    tss->ds = USER_SELECTOR_DS;
    tss->es = USER_SELECTOR_DS;
    tss->fs = USER_SELECTOR_DS;
    tss->gs = USER_SELECTOR_DS;
    //__asm__ __volatile__("jmp *%[ip]"::[ip]"r"(tss->eip));
    __asm__ __volatile__(
        // 模拟中断返回，切换入第1个可运行应用进程
        // 不过这里并不直接进入到进程的入口，而是先设置好段寄存器，再跳过去
        "push %[ss]\n\t"			// SS
        "push %[esp]\n\t"			// ESP
        "push %[eflags]\n\t"           // EFLAGS
        "push %[cs]\n\t"			// CS
        "push %[eip]\n\t"		    // ip
        "iret\n\t"::[ss]"r"(USER_SELECTOR_DS),  [esp]"r"(tss->esp), [eflags]"r"(tss->eflags),
        [cs]"r"(USER_SELECTOR_CS), [eip]"r"(tss->eip));
}
void init_main(void)
{
    irq_enable_global();
    sem_init(&sem,0);

    task_first_init();
    move_to_first_task();
    
    
}
