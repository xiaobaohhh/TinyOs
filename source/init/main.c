/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-09 13:02:24
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-24 19:53:35
 * @FilePath: \start\source\shell\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "lib_syscall.h"
#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/file.h>
#include "fs/file.h"
#include "dev/tty.h"


int main(int argc,char **argv)
{
    int a = 3 / 0;
    *(char *)0 = 0x1234;
    return 0;
}

