/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-29 11:01:06
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-02 22:05:36
 * @FilePath: \start\source\kernel\tools\log.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdarg.h>
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "tools/klib.h"
#include "cpu/irq.h"    
#include "ipc/mutex.h"
#define COM_PORT 0x3f8
static mutex_t log_mutex;
void log_init (void)
{
    mutex_init(&log_mutex);
    outb(COM_PORT + 1, 0x00);
    outb(COM_PORT + 3, 0x80);
    outb(COM_PORT + 0, 0x03);
    outb(COM_PORT + 1, 0x00);
    outb(COM_PORT + 3, 0x03);
    outb(COM_PORT + 2, 0xC7);
    outb(COM_PORT + 4, 0x0F);
}


void log_printf (const char *fmt, ...)
{
    char buf[128];
    kernel_memset(buf, 0, sizeof(buf));

    va_list args;
    va_start(args, fmt);

    kernel_vsprintf(buf, fmt, args);
    va_end(args);

    mutex_lock(&log_mutex);
    const char *p = buf;
    while (*p != '\0')
    {
        while ((inb(COM_PORT + 5) & (1 << 6)) == 0);
        outb(COM_PORT, *p);
        p++;
    }

    outb(COM_PORT, '\r');
    //outb(COM_PORT, '\n');
    mutex_unlock(&log_mutex);
}



