#include "cpu/cpu.h"
#include "os_cfg.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"
static segment_desc_t gdt_table[GDT_TABLE_SIZE];
static mutex_t gdt_mutex;

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
    mutex_lock(&gdt_mutex);
    for (int i = 1; i < GDT_TABLE_SIZE; i++)
    {
        if(gdt_table[i].attr == 0)
        {
            mutex_unlock(&gdt_mutex);
            return i * sizeof(segment_desc_t);
        }
        
    }
    mutex_unlock(&gdt_mutex);
    return -1;
}

void gdt_free_desc(int selector)
{
    mutex_lock(&gdt_mutex);
    gdt_table[selector / sizeof(segment_desc_t)].attr = 0;
    mutex_unlock(&gdt_mutex);
}
void init_gdt(void)
{
    for (int i = 0; i < GDT_TABLE_SIZE; i++)
    {
        segment_desc_set(i * sizeof(segment_desc_t), 0, 0, 0);
    }

    segment_desc_set(KERNEL_SELECTOR_CS, 0, 0xffffffff, SEG_P_PRESENT | SEG_DPL_0 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);
    segment_desc_set(KERNEL_SELECTOR_DS, 0, 0xffffffff, SEG_P_PRESENT | SEG_DPL_0 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);

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