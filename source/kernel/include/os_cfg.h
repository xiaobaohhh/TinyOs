/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-27 19:58:30
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-07 21:40:34
 * @FilePath: \start\source\kernel\include\os_cfg.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef OS_CFG_H
#define OS_CFG_H

#define GDT_TABLE_SIZE 256
#define KERNEL_SELECTOR_CS 0x08
#define KERNEL_SELECTOR_DS 0x10
#define SELECTOR_SYSCALL (5 * 8)
#define USER_SELECTOR_CS 0x1B
#define USER_SELECTOR_DS 0x23


#define KERNEL_STACK_SIZE 1024 * 8

#define OS_TICK_MS 20
#define OS_VERSION "0.0.1"


#define TASK_COUNT 128

#define ROOT_DEV  DEV_DISK,0xb1
#endif

