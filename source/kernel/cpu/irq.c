#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "core/task.h"
#include "tools/log.h"
#define IRQ_TABLE_NU 128
static gate_desc_t idt_table[IRQ_TABLE_NU];
static dump_core_regs(exception_frame_t *frame)
{
    uint32_t ss,esp;
    if(frame->cs & 0x3)
    {
        ss = frame->ss3;
        esp = frame->esp3;
    }
    else
    {
        ss = frame->ds;
        esp = frame->esp;
    }
    log_printf("IRQ: %d, error code: %d\n",frame->irq_num,frame->error_code);
    log_printf("CS: %x DS: %x ES: %x SS: %x FS: %x GS: %x\n",frame->cs,frame->ds,frame->es,ss,frame->fs,frame->gs);
    log_printf("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n",frame->eax,frame->ebx,frame->ecx,frame->edx);
    log_printf("EIP: 0x%x EFLAGS: 0x%x ESP: 0x%x EBP: 0x%x\n",frame->eip,frame->eflags,esp,frame->ebp);
    log_printf("EDI: 0x%x ESI: 0x%x EDI: 0x%x\n",frame->edi,frame->esi,frame->edi);
}
static void do_default_handler(exception_frame_t *frame,const char *message)
{
    log_printf("---------------------\n");
    log_printf("IRQ/Exception happend: %s\n",message);

    dump_core_regs(frame);
    if(frame->cs & 0x3)
    {
        sys_exit(frame->error_code);
    }
    else
    {
        while(1)
        {
            hlt();
        }
    }
}
void do_handler_unknown(exception_frame_t *frame)
{
    do_default_handler(frame,"unknown exception");
}
void do_handler_divider(exception_frame_t *frame)
{
    do_default_handler(frame,"divider exception");
}
void do_handler_debug(exception_frame_t *frame)
{
    do_default_handler(frame,"debug exception");
}
void do_handler_nmi(exception_frame_t *frame)   
{
    do_default_handler(frame,"nmi exception");
}
void do_handler_breakpoint(exception_frame_t *frame)
{
    do_default_handler(frame,"breakpoint exception");
}
void do_handler_overflow(exception_frame_t *frame)
{
    do_default_handler(frame,"overflow exception");
}
void do_handler_bounds(exception_frame_t *frame)
{
    do_default_handler(frame,"bounds exception");
}
void do_handler_invalid_op(exception_frame_t *frame)
{
    do_default_handler(frame,"invalid operation exception");
}
void do_handler_device_not_available(exception_frame_t *frame)
{
    do_default_handler(frame,"device not available exception");
}
void do_handler_double_fault(exception_frame_t *frame)
{
    do_default_handler(frame,"double fault exception");
}
void do_handler_invalid_tss(exception_frame_t *frame)
{
    do_default_handler(frame,"invalid tss exception");
}
void do_handler_segment_not_present(exception_frame_t *frame)
{
    do_default_handler(frame,"segment not present exception");
}
void do_handler_stack_segment_fault(exception_frame_t *frame)
{
    do_default_handler(frame,"stack segment fault exception");
}
void do_handler_general_protection(exception_frame_t *frame)
{
    log_printf("--------------------------------");
    log_printf("IRQ/Exception happened: General Protection.");
    if (frame->error_code & ERR_GP_EXT) {
        log_printf("the exception occurred during delivery of an "
                "event external to the program, such as an interrupt"
                "or an earlier exception.");
    } else {
        log_printf("the exception occurred during delivery of a"
                    "software interrupt (INT n, INT3, or INTO).");
    }
    
    if (frame->error_code & ERR_GP_IDT) {
        log_printf("the index portion of the error code refers "
                    "to a gate descriptor in the IDT");
    } else {
        log_printf("the index refers to a descriptor in the GDT");
    }
    
    log_printf("segment index: %d", frame->error_code & 0xFFF8);

    dump_core_regs(frame);
    if(frame->cs & 0x3)
    {
        sys_exit(frame->error_code);
    }
    else
    {
        while(1)
        {
            hlt();
        }
    }
}
void do_handler_page_fault(exception_frame_t *frame)
{
    log_printf("---------------------\n");
    log_printf("page fault exception\n");
    if(frame->error_code & ERR_PAGE_P)
    {
        log_printf("page fault exception: page level protection: 0x%x\n",read_cr2());
    }
    else
    {
        log_printf("page fault exception: page not present: 0x%x\n",read_cr2());
    }
    if(frame->error_code & ERR_PAGE_WR)
    {
        log_printf("page fault exception: write access :0x%x\n",read_cr2());
    }
    else
    {
        log_printf("page fault exception: read access :0x%x\n",read_cr2());
    }
    if(frame->error_code & ERR_PAGE_US)
    {
        log_printf("page fault exception: user access :0x%x\n",read_cr2());
    }
    else
    {
        log_printf("page fault exception: supervisor access :0x%x\n",read_cr2());
    }
    dump_core_regs(frame);
    if(frame->cs & 0x3)
    {
        sys_exit(frame->error_code);
    }
    else
    {
        while(1)
        {
            hlt();
        }
    }
    
}
void do_handler_x87_floating_point(exception_frame_t *frame)
{   
    do_default_handler(frame,"x87 floating point exception");
}
void do_handler_alignment_check(exception_frame_t *frame)
{
    do_default_handler(frame,"alignment check exception");
}
void do_handler_machine_check(exception_frame_t *frame)
{
    do_default_handler(frame,"machine check exception");
}
void do_handler_simd_floating_point(exception_frame_t *frame)
{
    do_default_handler(frame,"simd floating point exception");
}
void do_handler_virtualization(exception_frame_t *frame)
{
    do_default_handler(frame,"virtualization exception");
}
void do_handler_security(exception_frame_t *frame)
{
    do_default_handler(frame,"security exception");
}

static void init_pic(void)
{
    outb(PIC0_ICW1,PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
    outb(PIC0_ICW2,IRQ_PIC_START);
    outb(PIC0_ICW3,1 << IRQ2_NMI);
    outb(PIC0_ICW4,PIC_ICW4_8086);


    outb(PIC1_ICW1,PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
    outb(PIC1_ICW2,IRQ_PIC_START + 8);
    outb(PIC1_ICW3,2);
    outb(PIC1_ICW4,PIC_ICW4_8086);

    outb(PIC0_IMR,0xff & ~ (1 << 2));
    outb(PIC1_IMR,0xff);
}

void irq_init(void)
{
    for(int i = 0; i < IRQ_TABLE_NU; i++)
    {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handler_unknown, 
        GATE_P_PRESENT | GATE_DPL_0 | GATE_TYPE_INT);
    }
    //irq_install(IRQ0_DE,do_handler_divider); 用这个的话能进入do_handler_divider 但是跳不到.S的地方出栈
    irq_install(IRQ0_DE,exception_handler_divider);
    irq_install(IRQ1_DB,exception_handler_debug);
    irq_install(IRQ2_NMI,exception_handler_nmi);
    irq_install(IRQ3_BP,exception_handler_breakpoint);
    irq_install(IRQ4_OF,exception_handler_overflow);
    irq_install(IRQ5_BR,exception_handler_bounds);
    irq_install(IRQ6_UD,exception_handler_invalid_op);
    irq_install(IRQ7_NM,exception_handler_device_not_available);
    irq_install(IRQ8_DF,exception_handler_double_fault);
    irq_install(IRQ10_TS,exception_handler_invalid_tss);
    irq_install(IRQ11_NP,exception_handler_segment_not_present);
    irq_install(IRQ12_SS,exception_handler_stack_segment_fault);
    irq_install(IRQ13_GP,exception_handler_general_protection);
    irq_install(IRQ14_PF,exception_handler_page_fault);
    irq_install(IRQ16_MF,exception_handler_x87_floating_point);
    irq_install(IRQ17_AC,exception_handler_alignment_check);
    irq_install(IRQ18_MC,exception_handler_machine_check);
    irq_install(IRQ19_XM,exception_handler_simd_floating_point);
    irq_install(IRQ20_VE,exception_handler_virtualization);
    irq_install(IRQ21_CP,exception_handler_security);

    lidt((uint32_t)idt_table, sizeof(idt_table));

    init_pic();
}

int irq_install(int irq_num,irq_handler_t handler)
{
    if(irq_num < 0 || irq_num >= IRQ_TABLE_NU)
    {
        return -1;
    }
    gate_desc_set(idt_table + irq_num, KERNEL_SELECTOR_CS, (uint32_t)handler, 
    GATE_P_PRESENT | GATE_DPL_0 | GATE_TYPE_INT);
    return 0;
}

void irq_enable(int irq_num)
{
    if(irq_num < IRQ_PIC_START)
    {
        return;
    }
    irq_num -= IRQ_PIC_START;
    if(irq_num < 8)
    {
        uint8_t mask = inb(PIC0_IMR) & ~(1 << irq_num);
        outb(PIC0_IMR,mask);
    }
    else
    {
        irq_num -= 8;
        uint8_t mask = inb(PIC1_IMR) & ~(1 << (irq_num));
        outb(PIC1_IMR,mask);
    }
}

void irq_disable(int irq_num)
{
    if(irq_num < IRQ_PIC_START)
    {
        return;
    }
    irq_num -= IRQ_PIC_START;
    if(irq_num < 8)
    {
        uint8_t mask = inb(PIC0_IMR) | ~(1 << irq_num);
        outb(PIC0_IMR,mask);
    }
    else
    {
        irq_num -= 8;
        uint8_t mask = inb(PIC1_IMR) | ~(1 << (irq_num));
        outb(PIC1_IMR,mask);
    }
}
void irq_disable_global(void)
{
    cli();
}

void irq_enable_global(void)
{
    sti();
}
void pic_send_eoi(int irq_num)
{
    irq_num -= IRQ_PIC_START;
    if(irq_num >= 8)
    {
        outb(PIC1_OCW2,PIC_OCW2_EOI);
    }
    outb(PIC0_OCW2,PIC_OCW2_EOI);
}

irq_state_t irq_enter_protection(void)
{
    irq_state_t state = read_eflags();
    irq_disable_global();
    return state;
}

void irq_leave_protection(irq_state_t state)
{
    
    write_eflags(state);
}

