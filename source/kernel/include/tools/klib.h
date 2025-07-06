/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-05-29 14:05:22
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-13 13:06:32
 * @FilePath: \start\source\kernel\include\tools\klib.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef KLIB_H
#define KLIB_H
#include <stdarg.h>
#include "comm/types.h"
static inline uint32_t down2(uint32_t size, uint32_t bound)
{
    return size & ~(bound - 1);
}
static inline uint32_t up2(uint32_t size, uint32_t bound)
{
    return (size + bound - 1) & ~(bound - 1);
}

void kernel_strcpy (char *dest, const char *src);
void kernel_strncpy (char *dest, const char *src, int size);
int kernel_strncmp (const char *str1, const char *str2, int size);
int kernel_strlen (const char *str);

void kernel_memcpy (void *dest, void *src, int size);
void kernel_memset (void *dest, uint8_t c, int size);
int kernel_memcmp (void *d1, void *d2, int size);

void kernel_vsprintf (char *buf, const char *fmt, va_list args);
void kernel_sprintf (char *buf, const char *fmt, ...);
int strings_count(const char **start);
char * get_file_name(const char *name);
#ifndef RELEASE
#define ASSERT(expr) \
    if(!(expr))panic(__FILE__,__LINE__,__func__,#expr)
void panic(const char *file,int line,const char *func,const char *cond);
#else
#define ASSERT(expr) ((void)0)
#endif

#endif


