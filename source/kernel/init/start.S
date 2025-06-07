#include "os_cfg.h"

    .text
    .extern kernel_init
    .global _start
    .extern init_main
    .extern need_reschedule
_start:
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax

    # mov 4(%esp), %eax

    push %eax 
    call kernel_init

    jmp $KERNEL_SELECTOR_CS, $gdt_reload

gdt_reload:
    mov $KERNEL_SELECTOR_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss    
    
    mov $(stack + KERNEL_STACK_SIZE), %esp
    jmp init_main

    .bss
.comm stack, KERNEL_STACK_SIZE

.macro exception_handler name num with_err_code
    .text
    .extern do_handler_\name
    .global exception_handler_\name
exception_handler_\name:
    .if \with_err_code == 0
        push $0
    .endif

    push $\num
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    # 设置内核段寄存器
    mov $KERNEL_SELECTOR_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    push %esp
    call do_handler_\name
    add $(1*4), %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp

    iret 
.endm


.text
    .extern do_handler_time
    .global exception_handler_time
exception_handler_time:
    push $0

    push $0x20
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    # 设置内核段寄存器
    mov $KERNEL_SELECTOR_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    push %esp
    call do_handler_time
    add $(1*4), %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp
#   call do_schedule_switch
    call schedule_next_task
    iret 



    .text
    .extern do_handler_page_fault
    .global exception_handler_page_fault
exception_handler_page_fault:

    push $14
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    # 设置内核段寄存器
    mov $KERNEL_SELECTOR_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    push %esp
    call do_handler_page_fault
    add $(1*4), %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp

    iret 


    
exception_handler unknown,-1,0
exception_handler divider,0,0
exception_handler debug,1,0
exception_handler nmi,2,0
exception_handler breakpoint,3,0
exception_handler overflow,4,0
exception_handler bounds,5,0
exception_handler invalid_op,6,0
exception_handler device_not_available,7,0
exception_handler double_fault,8,1

exception_handler invalid_tss,10,1
exception_handler segment_not_present,11,1
exception_handler stack_segment_fault,12,1
exception_handler general_protection,13,1
# exception_handler page_fault,14,1
exception_handler x87_floating_point,16,0
exception_handler alignment_check,17,1
exception_handler machine_check,18,0
exception_handler simd_floating_point,19,0
exception_handler virtualization,20,0
exception_handler security,21,1

/*
exception_handler time,0x20,0
*/

    .text
    .global simple_switch
simple_switch:
    mov 4(%esp), %eax
    mov 8(%esp), %edx

    
    push %ebp
    push %ebx
    push %esi
    push %edi
    pushf                # 保存EFLAGS寄存器

    mov %esp,(%eax)

    mov %edx,%esp

    popf                 # 恢复EFLAGS寄存器
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    

    ret


# =========================== 通用任务切换系统 ===========================

# 通用任务切换函数
# 参数: universal_task_switch(save_tss_ptr, new_task_type, new_cr3, new_cs, new_eip, new_eflags, new_ss, new_esp, new_eax, new_ebx, new_ecx, new_edx, new_esi, new_edi, new_ebp)
# save_tss_ptr: 保存当前TSS的地址 (如果为0则不保存)
# new_task_type: 0=内核任务, 1=用户任务
    .global universal_task_switch
universal_task_switch:
    # 保存当前TSS (如果需要)
    mov 4(%esp), %eax    # save_tss_ptr
    cmp $0, %eax
    je skip_save_tss
    
    # 保存当前TSS到指定地址
    # TSS结构: pre_link(0), esp0(4), ss0(8), esp1(12), ss1(16), esp2(20), ss2(24), cr3(28), eip(32), eflags(36), eax(40), ecx(44), edx(48), ebx(52), esp(56), ebp(60), esi(64), edi(68), es(72), cs(76), ss(80), ds(84), fs(88), gs(92), ldt(96), iomap(100)
    
    # 保存当前寄存器到TSS
    mov %eax, 40(%eax)   # 保存eax到tss.eax
    mov %ebx, 52(%eax)   # 保存ebx到tss.ebx
    mov %ecx, 44(%eax)   # 保存ecx到tss.ecx
    mov %edx, 48(%eax)   # 保存edx到tss.edx
    mov %esi, 64(%eax)   # 保存esi到tss.esi
    mov %edi, 68(%eax)   # 保存edi到tss.edi
    mov %ebp, 60(%eax)   # 保存ebp到tss.ebp
    mov %esp, 56(%eax)   # 保存esp到tss.esp
    
    # 保存返回地址作为EIP
    mov (%esp), %edx
    mov %edx, 32(%eax)   # 保存eip到tss.eip
    
    # 保存EFLAGS
    pushf
    pop %edx
    mov %edx, 36(%eax)   # 保存eflags到tss.eflags
    
    # 保存段寄存器
    mov %cs, %dx
    mov %dx, 76(%eax)    # 保存cs到tss.cs
    mov %ds, %dx
    mov %dx, 84(%eax)    # 保存ds到tss.ds
    mov %ss, %dx
    mov %dx, 80(%eax)    # 保存ss到tss.ss
    mov %es, %dx
    mov %dx, 72(%eax)    # 保存es到tss.es
    mov %fs, %dx
    mov %dx, 88(%eax)    # 保存fs到tss.fs
    mov %gs, %dx
    mov %dx, 92(%eax)    # 保存gs到tss.gs
    
    # 保存CR3
    mov %cr3, %edx
    mov %edx, 28(%eax)   # 保存cr3到tss.cr3

skip_save_tss:
    # 获取新任务参数
    mov 8(%esp), %eax    # new_task_type
    cmp $0, %eax
    je switch_to_kernel_task
    jmp switch_to_user_task

switch_to_kernel_task:
    # 切换到内核任务
    
    # 切换页目录
    mov 12(%esp), %eax   # new_cr3
    cmp $0, %eax
    je skip_page_dir_kernel
    mov %eax, %cr3
skip_page_dir_kernel:
    
    # 设置内核段寄存器
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    # 恢复寄存器
    mov 36(%esp), %eax   # new_eax
    mov 40(%esp), %ebx   # new_ebx
    mov 44(%esp), %ecx   # new_ecx
    mov 48(%esp), %edx   # new_edx
    mov 52(%esp), %esi   # new_esi
    mov 56(%esp), %edi   # new_edi
    mov 60(%esp), %ebp   # new_ebp

    # 恢复EFLAGS并跳转
    mov 24(%esp), %eax   # new_eflags
    push %eax
    popf
    mov 20(%esp),%eax
    mov 32(%esp), %esp   # new_esp
    
    
    
    # 跳转到新任务
    jmp *%eax        # new_eip

switch_to_user_task:
    # 切换到用户任务
    
    # 切换页目录
    mov 12(%esp), %eax   # new_cr3
    cmp $0, %eax
    je skip_page_dir_user
    mov %eax, %cr3
skip_page_dir_user:
    
    # 构造iret栈帧（特权级切换需要5个值）
    pushl 28(%esp)       # new_ss (原来28+0，因为SS是第7个参数)
    pushl 36(%esp)       # new_esp (原来32+4)
    pushl 32(%esp)       # new_eflags (原来24+8)
    pushl 28(%esp)       # new_cs (原来16+12)
    pushl 36(%esp)       # new_eip (原来20+16)
    
    # 设置用户段寄存器
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    # 恢复通用寄存器
    mov 56(%esp), %eax   # new_eax (原来36+20)
    mov 60(%esp), %ebx   # new_ebx (原来40+20)
    mov 64(%esp), %ecx   # new_ecx (原来44+20)
    mov 68(%esp), %edx   # new_edx (原来48+20)
    mov 72(%esp), %esi   # new_esi (原来52+20)
    mov 76(%esp), %edi   # new_edi (原来56+20)
    mov 80(%esp), %ebp   # new_ebp (原来60+20)
    
    # 切换到用户态
    iret

# 快速内核任务切换 (只保存/恢复栈指针)
# 参数: quick_switch(current_esp_ptr, new_esp)
    .global quick_switch
quick_switch:
    mov 4(%esp), %eax    # current_esp_ptr
    mov 8(%esp), %edx    # new_esp
    
    # 保存调用者保存的寄存器
    push %ebp
    push %ebx
    push %esi
    push %edi
    pushf
    
    # 保存当前ESP
    mov %esp, (%eax)
    
    # 切换到新任务
    mov %edx, %esp
    
    # 恢复新任务寄存器
    popf
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    
    ret

# 获取当前完整上下文
# 参数: get_full_context(context_ptr)
    .global get_full_context
get_full_context:
    mov 4(%esp), %eax    # context_ptr
    
    # 保存所有寄存器
    mov %ebx, 0(%eax)
    mov %ecx, 4(%eax)
    mov %edx, 8(%eax)
    mov %esi, 12(%eax)
    mov %edi, 16(%eax)
    mov %ebp, 20(%eax)
    mov %esp, 24(%eax)
    
    # 保存返回地址作为EIP
    mov (%esp), %edx
    mov %edx, 28(%eax)
    
    # 保存EFLAGS
    pushf
    pop %edx
    mov %edx, 32(%eax)
    
    # 保存段寄存器
    mov %cs, %dx
    mov %dx, 36(%eax)
    mov %ds, %dx
    mov %dx, 38(%eax)
    mov %ss, %dx
    mov %dx, 40(%eax)
    
    ret

# 从中断中切换任务 (修改中断返回帧)
# 参数: switch_from_interrupt(interrupt_frame_ptr, new_task_type, new_page_dir, new_cs, new_eip, new_eflags, new_ss, new_esp, new_ebx, new_ecx, new_edx, new_esi, new_edi, new_ebp)
    .global switch_from_interrupt
switch_from_interrupt:
    mov 4(%esp), %eax    # interrupt_frame_ptr
    mov 8(%esp), %edx    # new_task_type
    
    # 切换页目录
    mov 12(%esp), %ebx   # new_page_dir
    cmp $0, %ebx
    je skip_page_dir_int
    mov %ebx, %cr3
skip_page_dir_int:
    
    cmp $0, %edx         # 检查任务类型
    je modify_for_kernel
    jmp modify_for_user

modify_for_kernel:
    # 修改为内核任务返回
    # 假设中断帧格式: eip, cs, eflags, esp, ss (从低到高)
    mov 20(%esp), %ebx   # new_eip
    mov %ebx, 0(%eax)    # 修改帧中的EIP
    mov $0x08, %bx       # 内核CS
    mov %bx, 4(%eax)     # 修改帧中的CS
    mov 24(%esp), %ebx   # new_eflags
    mov %ebx, 8(%eax)    # 修改帧中的EFLAGS
    
    # 设置内核段
    mov $0x10, %bx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    jmp restore_registers

modify_for_user:
    # 修改为用户任务返回
    mov 20(%esp), %ebx   # new_eip
    mov %ebx, 0(%eax)    # 修改帧中的EIP
    mov 16(%esp), %bx    # new_cs
    mov %bx, 4(%eax)     # 修改帧中的CS
    mov 24(%esp), %ebx   # new_eflags
    mov %ebx, 8(%eax)    # 修改帧中的EFLAGS
    mov 32(%esp), %ebx   # new_esp
    mov %ebx, 12(%eax)   # 修改帧中的ESP
    mov 28(%esp), %bx    # new_ss
    mov %bx, 16(%eax)    # 修改帧中的SS
    
    # 设置用户段
    mov $0x23, %bx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs

restore_registers:
    # 恢复通用寄存器
    mov 36(%esp), %ebx   # new_ebx
    mov 40(%esp), %ecx   # new_ecx
    mov 44(%esp), %edx   # new_edx
    mov 48(%esp), %esi   # new_esi
    mov 52(%esp), %edi   # new_edi
    mov 56(%esp), %ebp   # new_ebp
    
    ret                  # 返回到中断处理程序，让iret处理剩余工作





    