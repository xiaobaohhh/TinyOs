
	#include "boot.h"

  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern boot_entry
_start:
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov $0x7c00, %esp	# 设置栈指针到引导扇区开始处
#	mov $0xe, %ah 
#	mov $'H', %al
#	int $0x10

read_loader:
	mov $0x8000, %bx
	mov $0x2, %cx
	mov $0x02, %ah       # 读扇区功能
	mov $64, %al         # 读取64个扇区
	mov $0x0080, %dx
	int $0x13
	jc read_loader

	jmp boot_entry
	
	jmp .
	
	// 引导结束段
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
