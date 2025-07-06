/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-15 13:45:30
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-21 16:57:03
 * @FilePath: \start\source\kernel\include\dev\tty.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef TTY_H
#define TTY_H

#include "dev/dev.h"
#include "ipc/sem.h"
#define TTY_DEV_ID 0
#define TTY_NR 8
#define TTY_OBUF_SIZE 512
#define TTY_IBUF_SIZE 512

#define TTY_CMD_ECHO  0x1
typedef struct _tty_fifo_t
{
    char * buf;
    int size;
    int read;
    int write;
    int count;
}tty_fifo_t;

#define TTY_OCRLF (1 << 0)
#define TTY_INCLR (1 << 1)
#define TTY_IECHO       (1 << 1)
typedef struct _tty_t
{
    char obuf[TTY_OBUF_SIZE];
    char ibuf[TTY_IBUF_SIZE];
    tty_fifo_t ofifo;
    tty_fifo_t ififo;
    sem_t osem;
    sem_t isem;
    int oflags;
    int iflags;
    int console_index;
}tty_t;

int tty_fifo_put(tty_fifo_t *fifo, char c);
int tty_fifo_get(tty_fifo_t *fifo, char *c);

void tty_select(int tty);
void tty_in(char ch);

#endif