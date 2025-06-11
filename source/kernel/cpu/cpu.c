/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-05-27 19:55:41
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-07 21:49:53
 * @FilePath: \start\source\kernel\cpu\cpu.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"
#include "core/syscall.h"
static segment_desc_t gdt_table[GDT_TABLE_SIZE];
static mutex_t gdt_mutex;

void show_base (int index)
{
    segment_desc_t *desc = &gdt_table[index];  
    uint32_t base = desc->base15_0 | (desc->base23_16 << 16) | (desc->base31_24 << 24);
    log_printf("GDT[%d] base=%x\n", index, base);
}
void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr)
{
    segment_desc_t *desc = gdt_table + selector / sizeof(segment_desc_t);
    if(limit > 0xfffff)
    {
        limit /= 0x1000;
        attr |= SEG_G;
    }
    desc->limit15_0 = limit & 0xffff;
    desc->base15_0 = base & 0xffff;
    desc->base23_16 = (base >> 16) & 0xff;
    desc->attr = attr | (((limit >> 16) & 0x0f) << 8);
    desc->base31_24 = (base >> 24) & 0xff;
}
void gate_desc_set(gate_desc_t * gate_desc, uint16_t selector, uint32_t offset, uint16_t attr)
{
    gate_desc->selector = selector;
    gate_desc->offset15_0 = offset & 0xffff;
    gate_desc->offset31_16 = (offset >> 16) & 0xffff;
    gate_desc->attr = attr;
}

int gdt_alloc_desc(void)
{
    irq_state_t state = irq_enter_protection();
    //mutex_lock(&gdt_mutex);
    for (int i = 1; i < GDT_TABLE_SIZE; i++)
    {
        if(gdt_table[i].attr == 0)
        {
            //mutex_unlock(&gdt_mutex);
            irq_leave_protection(state);
            return i * sizeof(segment_desc_t);
        }
        
    }
    //mutex_unlock(&gdt_mutex);
    irq_leave_protection(state);
    return -1;
}

void gdt_free_desc(int selector)
{
    //mutex_lock(&gdt_mutex);
    irq_state_t state = irq_enter_protection();
    gdt_table[selector / sizeof(segment_desc_t)].attr = 0;
    //mutex_unlock(&gdt_mutex);
    irq_leave_protection(state);
}
void init_gdt(void)
{
    for (int i = 0; i < GDT_TABLE_SIZE; i++)
    {
        segment_desc_set(i * sizeof(segment_desc_t), 0, 0, 0);
    }

    segment_desc_set(KERNEL_SELECTOR_CS, 0, 0xffffffff, SEG_P_PRESENT | SEG_DPL_0 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);
    segment_desc_set(KERNEL_SELECTOR_DS, 0, 0xffffffff, SEG_P_PRESENT | SEG_DPL_0 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);

    gate_desc_set((gate_desc_t *)(gdt_table + (SELECTOR_SYSCALL >> 3)),KERNEL_SELECTOR_CS,
    (uint32_t)exception_handler_syscall,
    GATE_P_PRESENT | GATE_DPL_3 | GATE_TYPE_SYSCALL | SYSCALL_PARAM_COUNT
    );
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

void cpu_init(void)
{
    mutex_init(&gdt_mutex);
    init_gdt();
    
}

void switch_to_tss (int tss_sel)
{
    far_jump(tss_sel,0);
}