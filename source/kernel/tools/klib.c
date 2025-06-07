#include "tools/klib.h"
#include "comm/types.h"
#include "comm/cpu_instr.h"
void kernel_strcpy (char *dest, const char *src)
{
    if(!dest || !src)
    {
        return;
    }
    while((*dest++ = *src++) != '\0');
    //*dest = '\0';
}

void kernel_strncpy (char *dest, const char *src, int size)
{
    if(!dest || !src)
    {
        return;
    }
    char *d = dest;
    const char *s = src;
    while(size-- > 0 && (*s))
    {
        *d++ = *s++;
    }
    if(size == 0)
    {
        *(d - 1) = '\0';
    }
    else
    {
        *d = '\0';
    }
}

int kernel_strncmp (const char *str1, const char *str2, int size)
{
    if(!str1 || !str2)
    {
        return 0;
    }
    while(*str1 && *str2 && (*str1 == *str2) && size)
    {
        str1++;
        str2++;
    }
    return !((*str1 == '\0') || (*str2 == '\0') || (*str1 == *str2));
}

int kernel_strlen (const char *str)
{
    if(!str)
    {
        return 0;
    }
    const char *s = str;
    int len = 0;
    while(*s++)
    {
        len++;
    }
    return len;
}


void kernel_memcpy (void *dest, void *src, int size)
{
    if(!dest || !src || size <= 0)
    {
        return;
    }
    uint8_t *d = (uint8_t *)dest;
    uint8_t *s = (uint8_t *)src;
    while(size--)
    {
        *d++ = *s++;
    }
}

void kernel_memset (void *dest, int c, int size)
{
    if(!dest || size <= 0)
    {
        return;
    }
    uint8_t *d = (uint8_t *)dest;
    while(size--)
    {
        *d++ = (uint8_t)c;
    }
}

int kernel_memcmp (void *d1, void *d2, int size)
{
    if(!d1 || !d2 || !size)
    {
        return 1;
    }
    uint8_t *p_d1 = (uint8_t *)d1;
    uint8_t *p_d2 = (uint8_t *)d2;
    while(size--)
    {
        if(*p_d1++ != *p_d2++)
        {
            return 1;
        }
    }
    return 0;
}

void kernel_itoa (char *buf, int num, int base)
{
    int is_neg = 0;
    static const char * num2ch = "0123456789abcdef";
    char *p = buf;
    if(base != 2 && base != 8 && base != 10 && base != 16)
    {
        *p = '\0';
        return;
    }
    
    // 对于十六进制，强制按无符号数处理
    if(base == 16)
    {
        uint32_t unum = (uint32_t)num;  // 强制转换为无符号数
        do{
            char ch = num2ch[unum % base];
            *p++ = ch;
            unum /= base;
        }while (unum);
    }
    else
    {
        // 其他进制的原有逻辑
        is_neg = num < 0 ? 1 : 0;
        if(num < 0 && base == 10)
        {
            num = -num;
        }
        do{
            char ch = num2ch[num % base];
            *p++ = ch;
            num /= base;
        }while (num);
        if(base == 10 && is_neg)
        {
            *p++ = '-';
        }
    }
    
    *p-- = '\0';

    char *start = buf;
    while(start < p)
    {
        char tmp = *start;
        *start = *p;
        *p = tmp;
        p--;
        start++;
    }
}
void kernel_sprintf (char *buf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    kernel_vsprintf(buf, fmt, args);
    va_end(args);
}
void kernel_vsprintf (char *buf, const char *fmt, va_list args)
{
    enum{
        NORMAL,READ_FMT
    }state = NORMAL;
    char * curr = buf;
    char ch;
    while((ch = *fmt++))
    {
        switch(state)
        {
        case NORMAL:
            if(ch == '%')
            {
                state = READ_FMT;
            }
            else
            {
                *curr++ = ch;
            }
            break;
        case READ_FMT:
            if(ch == 'd')
            {
                int num = va_arg(args,int);
                kernel_itoa(curr,num,10);
                curr += kernel_strlen(curr);
            }
            else if(ch == 'x')
            {
                int num = va_arg(args,int);
                kernel_itoa(curr,num,16);
                curr += kernel_strlen(curr);
            }
            else if(ch == 'c')
            {
                char ch = va_arg(args,int);
                *curr++ = ch;
            }
            else if(ch == 's')
            {
                const char *str = va_arg(args,char *);
                int len = kernel_strlen(str);
                while(len--)
                {
                    *curr++ = *str++;
                }
            }
            state = NORMAL;
            break;
        }
    }
    *curr = '\0';
}
void panic(const char *file,int line,const char *func,const char *cond)
{
    log_printf("assert failed: %s\n",cond);
    log_printf("file: %s\n",file);
    log_printf("line: %d\n",line);
    log_printf("function: %s\n",func);
    for(;;){
        hlt();
    }
}