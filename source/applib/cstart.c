/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-09 13:17:07
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-23 16:26:32
 * @FilePath: \start\source\applib\cstart.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdint.h>
#include "lib_syscall.h"
#include <stdlib.h>
extern char __bss_start__[];
extern char __bss_end__[];

int main(int argc,char **argv);

void cstart(int argc,char **argv)
{
    uint8_t *start = (uint8_t *)&__bss_start__;
    while(start < (uint8_t *)&__bss_end__)
    {
        *start = 0;
        start++;
    }
    exit(main(argc,argv));
}