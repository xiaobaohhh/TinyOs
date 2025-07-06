/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-15 13:45:40
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-22 22:38:58
 * @FilePath: \start\source\kernel\dev\tty.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "dev/tty.h"
#include "dev/dev.h"
#include "tools/log.h"
#include "dev/kbd.h"
#include "dev/console.h"
#include "cpu/irq.h"
static tty_t tty_devs[TTY_NR];
static int current_tty = 0;
static tty_t * get_tty(device_t *dev)
{
    int idx = dev->minor;
    if(idx < 0 || idx >= TTY_NR || !dev->open_count)
    {
        return (tty_t *)0;
    }
    return tty_devs + idx;
}
void tty_fifo_init(tty_fifo_t *fifo, char *buf, int size)
{
    fifo->buf = buf;
    fifo->count = 0;
    fifo->size = size;
    fifo->write = 0;
    fifo->read = 0;
}

int tty_fifo_put(tty_fifo_t *fifo, char c)
{
    irq_state_t state = irq_enter_protection();
    if(fifo->count >= fifo->size)
    {
        irq_leave_protection(state);
        return -1;
    }

    fifo->buf[fifo->write++] = c;
    if(fifo->write >= fifo->size)
    {
        fifo->write = 0;
    }
    fifo->count++;
    irq_leave_protection(state);
    return 0;
}

int tty_fifo_get(tty_fifo_t *fifo, char *c)
{
    irq_state_t state = irq_enter_protection();
    if(fifo->count <= 0)
    {
        irq_leave_protection(state);
        return -1;
    }
    *c = fifo->buf[fifo->read++];
    if(fifo->read >= fifo->size)
    {
        fifo->read = 0;
    }
    fifo->count--;
    irq_leave_protection(state);
    return 0;
}
int tty_open(device_t *dev)
{
    int idx = dev->minor;
    if(idx < 0 || idx >= TTY_NR)
    {
        log_printf("tty_open: invalid minor number %d\n", idx);
        return -1;
    }
    tty_t * tty = tty_devs + idx;
    tty_fifo_init(&tty->ofifo, tty->obuf, TTY_OBUF_SIZE);
    sem_init(&tty->osem, TTY_OBUF_SIZE);
    tty_fifo_init(&tty->ififo, tty->ibuf, TTY_IBUF_SIZE);
    sem_init(&tty->isem, 0);
    tty->oflags = TTY_OCRLF;
    tty->iflags = TTY_INCLR | TTY_IECHO;
    tty->console_index = idx;
    
    kbd_init();
    console_init(idx);
    return 0;
}
void tty_close(device_t *dev)
{
    return;
}
int tty_read(device_t *dev, int addr, char * buf, int size)
{
    if(size < 0)
    {
        return -1;
    }
    tty_t * tty = get_tty(dev);
    char * pbuf = buf;
    int len = 0;
    while(len < size)
    {
        sem_wait(&tty->isem);
        char ch;
        tty_fifo_get(&tty->ififo, &ch);
        switch (ch)
        {
        case 0x7f:
            if(len == 0)
            {
                continue;
            }
            len--;
            pbuf--;
            break;
        case '\n':
            if((tty->iflags & TTY_INCLR ) && (len < size - 1))
            {
                *pbuf++ = '\r';
                len++;
            }
            *pbuf++ = '\n';
            len++;
            break;
        default:
            *pbuf++ = ch;
            len++;
            break;
        }
        if(tty->iflags & TTY_IECHO)
        {
            tty_write(dev,0,&ch,1);
        }
        if(ch == '\n' || ch == '\r')
        {
            break;
        }
    }
    return len;
}
int tty_write(device_t *dev, int addr, char * buf, int size)
{
    if(size < 0)
    {
        return -1;
    }
    tty_t * tty = get_tty(dev);
    if(!tty)
    {
        return -1;
    }
    int len = 0;
    while (size)
    {
        char c = *buf++;
        sem_wait(&tty->osem);
        int err = tty_fifo_put(&tty->ofifo, c);
        if(err < 0)
        {
            break;
        }
        len++;
        size--;
        console_write(tty);
    }

    return len;
}
int tty_control(device_t *dev, int cmd, int arg0, int arg1)
{
    tty_t * tty = get_tty(dev);
    switch (cmd)
    {
    case TTY_CMD_ECHO:
        if(arg0)
        {
            tty->iflags |= TTY_IECHO;
        }
        else
        {
            tty->iflags &= ~TTY_IECHO;
        }
        break;
    
    default:
        break;
    }
    return 0;    
}

void tty_in(char ch)
{
    tty_t * tty = tty_devs + current_tty;
    if(sem_count(&tty->isem) >= TTY_IBUF_SIZE)
    {
        return;
    }
    tty_fifo_put(&tty->ififo, ch);
    sem_notify(&tty->isem);
}

void tty_select(int tty)
{
    if(tty != current_tty)
    {
        console_select(tty);
        current_tty = tty;
    }
}

dev_desc_t dev_tty_desc = {
    .name = "tty",
    .major = DEV_TTY,
    .open = tty_open,
    .close = tty_close,
    .read = tty_read,
    .write = tty_write,
    .control = tty_control,
};






