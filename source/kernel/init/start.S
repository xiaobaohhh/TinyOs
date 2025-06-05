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
    push %esp
    call do_handler_time
    add $(1*4), %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp
    call do_schedule_switch

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
exception_handler page_fault,14,1
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


# 手动构造iret栈帧切换到用户态
    .global manual_switch_to_user
manual_switch_to_user:
    # 参数：user_entry_point
    mov 4(%esp), %eax    # 用户程序入口地址
    
    # 设置用户态段寄存器
    mov $0x23, %bx       # 用户数据段选择子
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    # 构造iret栈帧（从高地址到低地址压栈）
    push $0x23           # 用户栈段(SS)
    push $0x7FFFFFFF     # 用户栈指针(ESP) 
    push $0x202          # EFLAGS（开启中断标志IF=1）
    push $0x1B           # 用户代码段(CS)
    push %eax            # 用户程序入口(EIP)
    
    # 执行特权级切换
    iret

# 创建用户进程并切换（更完整的版本）
    .global create_and_switch_user
create_and_switch_user:
    # 参数：用户程序地址、用户栈地址
    mov 4(%esp), %eax    # 用户程序地址
    mov 8(%esp), %ebx    # 用户栈地址
    
    # 清理寄存器（安全考虑）
    xor %ecx, %ecx
    xor %edx, %edx
    xor %esi, %esi
    xor %edi, %edi
    xor %ebp, %ebp
    
    # 设置用户态段寄存器
    mov $0x23, %cx       # 用户数据段选择子
    mov %cx, %ds
    mov %cx, %es
    mov %cx, %fs
    mov %cx, %gs
    
    # 构造完整的iret栈帧
    push $0x23           # SS - 用户栈段
    push %ebx            # ESP - 用户栈指针
    pushf                # 获取当前EFLAGS
    pop %ecx
    or $0x200, %ecx      # 设置IF标志（开启中断）
    push %ecx            # EFLAGS
    push $0x1B           # CS - 用户代码段
    push %eax            # EIP - 用户程序入口
    
    # 跳转到用户态
    iret





# =========================== 统一任务切换系统 ===========================

# 统一任务切换函数 - 支持内核态/用户态任务间随意切换
    .global universal_task_switch
universal_task_switch:
    mov 4(%esp), %eax    # 当前任务指针
    mov 8(%esp), %edx    # 新任务指针
    
    # 保存当前任务上下文到任务结构
    # 假设context结构从偏移0开始
    mov %ebx, 0(%eax)    # ebx
    mov %ecx, 4(%eax)    # ecx
    mov %edx, 8(%eax)    # edx (先保存)
    mov %esi, 12(%eax)   # esi
    mov %edi, 16(%eax)   # edi
    mov %ebp, 20(%eax)   # ebp
    mov %esp, 24(%eax)   # esp
    
    # 保存返回地址作为EIP
    mov (%esp), %ebx
    mov %ebx, 28(%eax)   # eip
    
    # 保存EFLAGS
    pushf
    pop %ebx
    mov %ebx, 32(%eax)   # eflags
    
    # 保存当前段寄存器
    mov %cs, %bx
    mov %bx, 36(%eax)    # cs
    mov %ds, %bx
    mov %bx, 38(%eax)    # ds
    mov %ss, %bx
    mov %bx, 46(%eax)    # ss
    
    # 重新获取新任务指针
    mov 8(%esp), %edx
    
    # 检查新任务类型 (type字段假设在偏移64)
    cmpl $0, 64(%edx)    # 0=内核任务, 1=用户任务
    je switch_to_kernel
    jmp switch_to_user

switch_to_kernel:
    # 切换到内核任务
    
    # 切换页目录 (page_directory字段假设在偏移68)
    mov 68(%edx), %ebx
    cmp $0, %ebx
    je skip_page_switch1
    mov %ebx, %cr3
skip_page_switch1:
    
    # 设置内核段寄存器
    mov $0x10, %bx       # 内核数据段
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    mov %bx, %ss
    
    # 恢复通用寄存器
    mov 0(%edx), %ebx
    mov 4(%edx), %ecx
    mov 12(%edx), %esi
    mov 16(%edx), %edi
    mov 20(%edx), %ebp
    mov 24(%edx), %esp
    
    # 恢复EFLAGS
    mov 32(%edx), %eax
    push %eax
    popf
    
    # 恢复edx并跳转到任务
    mov 8(%edx), %eax
    mov %eax, %edx
    ret                  # 返回到新任务

switch_to_user:
    # 切换到用户任务
    
    # 切换页目录
    mov 68(%edx), %ebx
    cmp $0, %ebx
    je skip_page_switch2
    mov %ebx, %cr3
skip_page_switch2:
    
    # 构造iret栈帧进行特权级切换
    # 用户栈指针 (user_esp字段假设在偏移76)
    pushl $0x23          # SS - 用户栈段
    pushl 76(%edx)       # ESP - 用户栈指针
    pushl 32(%edx)       # EFLAGS
    pushl $0x1B          # CS - 用户代码段
    pushl 28(%edx)       # EIP - 用户程序地址
    
    # 设置用户态段寄存器
    mov $0x23, %bx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    # 恢复通用寄存器
    mov 0(%edx), %ebx
    mov 4(%edx), %ecx
    mov 12(%edx), %esi
    mov 16(%edx), %edi
    mov 20(%edx), %ebp
    mov 8(%edx), %eax    # edx暂存到eax
    mov %eax, %edx       # 恢复edx
    
    # 执行特权级切换
    iret

# 快速内核任务切换 (只保存必要寄存器)
    .global fast_kernel_switch
fast_kernel_switch:
    mov 4(%esp), %eax    # 当前ESP保存地址
    mov 8(%esp), %edx    # 新ESP值
    
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

# 从中断返回时切换任务
    .global switch_on_interrupt_return
switch_on_interrupt_return:
    mov 4(%esp), %eax    # 新任务指针
    
    # 检查任务类型
    cmpl $0, 64(%eax)
    je return_to_kernel
    jmp return_to_user

return_to_kernel:
    # 返回内核任务
    mov 68(%eax), %ebx   # 切换页目录
    cmp $0, %ebx
    je skip_page_switch3
    mov %ebx, %cr3
skip_page_switch3:
    
    # 设置内核段
    mov $0x10, %bx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    # 恢复并跳转
    mov 0(%eax), %ebx
    mov 4(%eax), %ecx
    mov 8(%eax), %edx
    mov 12(%eax), %esi
    mov 16(%eax), %edi
    mov 20(%eax), %ebp
    mov 24(%eax), %esp
    jmp *28(%eax)        # 跳转到任务

return_to_user:
    # 返回用户任务 - 需要修改当前栈上的iret帧
    
    # 切换页目录
    mov 68(%eax), %ebx
    cmp $0, %ebx
    je skip_page_switch4
    mov %ebx, %cr3
skip_page_switch4:
    
    # 修改栈上的iret帧
    mov 28(%eax), %ebx   # 用户EIP
    mov %ebx, 20(%esp)   # 修改栈上的EIP
    mov $0x1B, %bx       # 用户CS
    mov %bx, 16(%esp)    # 修改栈上的CS
    mov 32(%eax), %ebx   # 用户EFLAGS
    mov %ebx, 12(%esp)   # 修改栈上的EFLAGS
    mov 76(%eax), %ebx   # 用户ESP
    mov %ebx, 8(%esp)    # 修改栈上的ESP
    mov $0x23, %bx       # 用户SS
    mov %bx, 4(%esp)     # 修改栈上的SS
    
    # 设置用户段
    mov $0x23, %bx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    # 恢复通用寄存器
    mov 0(%eax), %ebx
    mov 4(%eax), %ecx
    mov 8(%eax), %edx
    mov 12(%eax), %esi
    mov 16(%eax), %edi
    mov 20(%eax), %ebp
    
    # 通过修改过的iret帧返回用户态
    iret





    