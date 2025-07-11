/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-05-27 19:51:47
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-07 21:47:43
 * @FilePath: \start\source\kernel\include\cpu\cpu.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CPU_H
#define CPU_H

#include "comm/types.h"

#define EFLAGS_DEFAULT  (1 << 1)
#define EFLAGS_IF       (1 << 9)
typedef struct _tss_t
{
    uint32_t pre_link;
    uint32_t esp0,ss0,esp1,ss1,esp2,ss2;
    uint32_t cr3;
    uint32_t eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
    uint32_t es,cs,ss,ds,fs,gs;
    uint32_t ldt;
    uint16_t iomap;
}tss_t;
#pragma pack(1)

typedef struct _segment_desc_t
{
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
}segment_desc_t;
typedef struct _gate_desc_t
{
    uint16_t offset15_0;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset31_16;
}gate_desc_t;

#pragma pack()

#define GATE_P_PRESENT (1 << 15)
#define GATE_DPL_0 (0 << 13)
#define GATE_DPL_3 (3 << 13)
#define GATE_TYPE_INT (0xe << 8)
#define GATE_TYPE_SYSCALL (0xc << 8)


#define SEG_G (1 << 15)
#define SEG_D (1 << 14)
#define SEG_P_PRESENT (1 << 7)
#define SEG_DPL_0 (0 << 5)
#define SEG_DPL_3 (3 << 5)

#define SEG_CPL_0 (0 << 0)
#define SEG_CPL_3 (3 << 0)

#define SEG_S_SYSTEM (0 << 4)
#define SEG_S_NORMAL (1 << 4)
#define SEG_S_CODE_DATA (1 << 4)
#define SEG_TYPE_TSS (9 << 0)
#define SEG_TYPE_CODE (1 << 3)
#define SEG_TYPE_DATA (0 << 3)

#define SEG_TYPE_RW (1 << 1)



void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr);
void init_gdt(void);
void cpu_init(void);
void gate_desc_set(gate_desc_t * gate_desc, uint16_t selector, uint32_t offset, uint16_t attr);
int gdt_alloc_desc(void);
void gdt_free_desc(int selector);
void switch_to_tss (int tss_sel);
void show_base (int index);
#endif

