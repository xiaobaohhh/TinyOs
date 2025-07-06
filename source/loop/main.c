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
    if(argc == 1)
    {
        char msg_buf[128];
        fgets(msg_buf,sizeof(msg_buf),stdin);
        msg_buf[strlen(msg_buf) - 1] = '\0';
        puts(msg_buf);
        return 0;
    }
    int count = 1;
    char ch;
    while((ch = getopt(argc,argv,"n:h")) != -1)
    {
        switch(ch)
        {
            case 'n':
                count = atoi(optarg);
                break;
            case 'h':
                printf("echo [-n count] message\n");
                optind = 1;
                return 0;
            case '?':
                if(optarg)
                {
                    fprintf(stderr,ESC_COLOR_ERROR"%s: invalid option -- '%c'\n"ESC_COLOR_DEFAULT,argv[0],optarg);
                }
                optind = 1;
                return -1;
            default:
                optind = 1;
                return -1;
        }
    }
    if(optind > argc - 1)
    {
        fprintf(stderr,ESC_COLOR_ERROR"Message is empty\n"ESC_COLOR_DEFAULT);
        optind = 1;
        return -1;
    }
    char * msg = argv[optind];
    for(int i = 0;i < count;i++)
    {
        puts(msg);
    }
    optind = 1;
    return 0;
}

