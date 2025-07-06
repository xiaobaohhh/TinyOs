/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-12 21:59:54
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-23 22:19:46
 * @FilePath: \start\source\kernel\dev\console.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "dev/console.h"
#include "tools/klib.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "dev/tty.h"

#define CONSOLE_NR  8
static console_t console_buf[CONSOLE_NR];
static int curr_console_idx = 0;
static void scroll_up(console_t * console,int lines);

static int read_cursor_pos()
{
    int pos;
    irq_state_t state = irq_enter_protection();
    
    outb(0x3d4,0x0f);
    pos = inb(0x3d5);
    outb(0x3d4,0x0e);
    pos |= inb(0x3d5) << 8;
    irq_leave_protection(state);
    return pos;
}

static void update_cursor_pos(console_t * console)
{
    
    uint16_t pos = (console - console_buf) * console->display_row * console->display_col;
    pos += console->cursor_row * console->display_col + console->cursor_col;
    irq_state_t state = irq_enter_protection();
    outb(0x3d4,0x0f);
    outb(0x3d5,(uint8_t)(pos & 0xff));
    outb(0x3d4,0x0e);
    outb(0x3d5,(uint8_t)((pos >> 8) & 0xff));
    irq_leave_protection(state);
}
static void move_dorward(console_t * console,int n)
{
    for(int i = 0; i < n; i++)
    {
        if (++console->cursor_col >= console->display_col) 
        {
			console->cursor_col = 0;
            console->cursor_row++;
            if(console->cursor_row >= console->display_row)
            {
                scroll_up(console,1);
            }
        }
    }
}
static void show_char(console_t * console,char c)
{
    int offset = console->cursor_col + console->cursor_row * console->display_col;
    disp_char_t * p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_dorward(console,1);
}
static void erase_rows(console_t * console,int start,int end)
{
    disp_char_t * disp_start = console->disp_base + start * console->display_col;
    disp_char_t * disp_end = console->disp_base + (end + 1) * console->display_col;
    while(disp_start < disp_end)
    {
        disp_start->c = ' ';
        disp_start->foreground = console->foreground;
        disp_start->background = console->background;
        disp_start++;
    }
}
static void clear_display(console_t * console)
{
    int size = console->display_row * console->display_col;
    disp_char_t * start = console->disp_base;
    for(int i = 0; i < size; i++)
    {
        start[i].c = ' ';
        start[i].foreground = console->foreground;
        start[i].background = console->background;
    }
}

int console_init(int idx)
{
    

    console_buf[idx].disp_base = (disp_char_t *)CONSOLE_DISP_ADDR + idx * CONSOLE_ROW_MAX * CONSOLE_COL_MAX;
    console_buf[idx].display_row = CONSOLE_ROW_MAX;
    console_buf[idx].display_col = CONSOLE_COL_MAX;
    console_buf[idx].foreground = COLOR_WHITE;
    console_buf[idx].background = COLOR_BLACK;
    if(idx == 0)
    {
        int cursor_pos = read_cursor_pos();
        console_buf[idx].cursor_row = cursor_pos / console_buf[idx].display_col;
        console_buf[idx].cursor_col = cursor_pos % console_buf[idx].display_col;
    }
    else
    {
        console_buf[idx].cursor_row = 0;
        console_buf[idx].cursor_col = 0;
        clear_display(&console_buf[idx]);
        //update_cursor_pos(&console_buf[idx]);
    }
    console_buf[idx].write_state = CONSOLE_WRITE_NORMAL;
    console_buf[idx].old_cursor_row = console_buf[idx].cursor_row;
    console_buf[idx].old_cursor_col = console_buf[idx].cursor_col;
    //clear_display(&console_buf[i]);     
    mutex_init(&console_buf[idx].mutex);
    return 0;
}

static void mov_to_col0(console_t * console)
{
    console->cursor_col = 0;
}

static void scroll_up(console_t * console,int lines)
{
    disp_char_t * dest = console->disp_base;
    disp_char_t * src = console->disp_base + lines * console->display_col;
    uint32_t size = (console->display_row - lines) * console->display_col * sizeof(disp_char_t);
    kernel_memcpy(dest,src,size);
    erase_rows(console,console->display_row - lines,console->display_row - 1);
    console->cursor_row -= lines;
}
static void mov_next_line(console_t * console)
{
    console->cursor_row++;
    if(console->cursor_row >= console->display_row)
    {
        scroll_up(console,1);
    }
}

static int move_backword(console_t * console,int n)
{
    int status = -1;
    for(int i = 0; i < n; i++)
    {
        if(console->cursor_col > 0)
        {
            console->cursor_col--;
            status = 0;
        }
        else if(console->cursor_row > 0)
        {
            console->cursor_row--;
            console->cursor_col = console->display_col - 1;
            status = 0;
        }
    }
    return status;
}
static void erase_backword(console_t * console)
{
    if(move_backword(console,1) == 0)
    {
        show_char(console,' ');
        move_backword(console,1);
    }
}

static void write_normal(console_t * con,char ch)
{
    switch (ch)
        {
            case ASCII_ESC:
                con->write_state = CONSOLE_WRITE_ESC;
                break;
            case 0x7f:
                erase_backword(con);
                break;
            case '\b':
                move_backword(con,1);
                break;
            case '\r':
                mov_to_col0(con);
                break;
            case '\n':
                mov_to_col0(con);
                mov_next_line(con);
                break;
            default:
                if(ch >= ' ' && ch <= '~')
                {
                    show_char(con,ch);
                }
                break;
        }
}
void save_cursor(console_t * console) {
    console->old_cursor_col = console->cursor_col;
    console->old_cursor_row = console->cursor_row;
}

void restore_cursor(console_t * console) {
    console->cursor_col = console->old_cursor_col;
    console->cursor_row = console->old_cursor_row;
}
static void clear_esc_param (console_t * console) {
	kernel_memset(console->esc_param, 0, sizeof(console->esc_param));
	console->curr_param_index = 0;
}
static void write_esc (console_t * console, char c) {
    // https://blog.csdn.net/ScilogyHunter/article/details/106874395
    // ESC状态处理, 转义序列模式 ESC 0x20-0x27(0或多个) 0x30-0x7e
    switch (c) {
        case '7':		// ESC 7 保存光标
            save_cursor(console);
            console->write_state = CONSOLE_WRITE_NORMAL;
            break;
        case '8':		// ESC 8 恢复光标
            restore_cursor(console);
            console->write_state = CONSOLE_WRITE_NORMAL;
            break;
        case '[':
            clear_esc_param(console);
            console->write_state = CONSOLE_WRITE_SQUARE;
            break;
        default:
            console->write_state = CONSOLE_WRITE_NORMAL;
            break;
    }
}
static void set_font_style (console_t * console) {
	static const cclor_t color_table[] = {
			COLOR_Black, COLOR_Red, COLOR_Green, COLOR_Yellow, // 0-3
			COLOR_Blue, COLOR_Magenta, COLOR_Cyan, COLOR_White, // 4-7
	};

	for (int i = 0; i < console->curr_param_index; i++) {
		int param = console->esc_param[i];
		if ((param >= 30) && (param <= 37)) {  // 前景色：30-37
			console->foreground = color_table[param - 30];
		} else if ((param >= 40) && (param <= 47)) {
			console->background = color_table[param - 40];
		} else if (param == 39) { // 39=默认前景色
			console->foreground = COLOR_White;
		} else if (param == 49) { // 49=默认背景色
			console->background = COLOR_Black;
		}
	}
}
static void move_left (console_t * console, int n) {
    // 至少移致动1个
    if (n == 0) {
        n = 1;
    }

    int col = console->cursor_col - n;
    console->cursor_col = (col >= 0) ? col : 0;
}
static void move_right (console_t * console, int n) {
    // 至少移致动1个
    if (n == 0) {
        n = 1;
    }

    int col = console->cursor_col + n;
    if (col >= console->display_col) {
        console->cursor_col = console->display_col - 1;
    } else {
        console->cursor_col = col;
    }
}

/**
 * 移动光标
 */
static void move_cursor(console_t * console) {
	if (console->curr_param_index >= 1) {
		console->cursor_row = console->esc_param[0];
	}

	if (console->curr_param_index >= 2) {
		console->cursor_col = console->esc_param[1];
	}
}

static void erase_in_display(console_t * console) {
	if (console->curr_param_index <= 0) {
		return;
	}

	int param = console->esc_param[0];
	if (param == 2) {
		// 擦除整个屏幕
		erase_rows(console, 0, console->display_row - 1);
        console->cursor_col = console->cursor_row = 0;
	}
}
static void write_esc_square (console_t * console, char c) {
    // 接收参数
    if ((c >= '0') && (c <= '9')) {
        // 解析当前参数
        int * param = &console->esc_param[console->curr_param_index];
        *param = *param * 10 + c - '0';
    } else if ((c == ';') && console->curr_param_index < ESC_PARAM_MAX) {
        // 参数结束，继续处理下一个参数
        console->curr_param_index++;
    } else {
        // 结束上一字符的处理
        console->curr_param_index++;

        // 已经接收到所有的字符，继续处理
        switch (c) {
        case 'm': // 设置字符属性
            set_font_style(console);
            break;
        case 'D':	// 光标左移n个位置 ESC [Pn D
            move_left(console, console->esc_param[0]);
            break;
        case 'C':
            move_right(console, console->esc_param[0]);
            break;
        case 'H':
        case 'f':
            move_cursor(console);
            break;
        case 'J':
            erase_in_display(console);
            break;
        }
        console->write_state = CONSOLE_WRITE_NORMAL;
    }
}
int console_write(tty_t * tty)
{
    
    console_t * con = &console_buf[tty->console_index];
    int len;
    mutex_lock(&con->mutex);
    do{
        char ch;
        int err = tty_fifo_get(&tty->ofifo,&ch);
        
        if(err < 0)
        {
            break;
        }
        sem_notify(&tty->osem);
        switch(con->write_state)
        {
            case CONSOLE_WRITE_NORMAL:
                write_normal(con,ch);
                break;
            case CONSOLE_WRITE_ESC:
                write_esc(con,ch);
                break;
            case CONSOLE_WRITE_SQUARE:
                write_esc_square(con,ch);
                break;
        }
        len++;
    }while(1);
    mutex_unlock(&con->mutex);
    if(tty->console_index == curr_console_idx)
    {
        update_cursor_pos(con);
    }
    
    return len;
}

void console_close(int console)
{

}

void console_select(int idx)
{
    console_t * console = &console_buf[idx];
    if(console->disp_base == 0)
    {
        console_init(idx);
    }
    uint16_t pos = idx * console->display_row * console->display_col;
    outb(0x3d4,0xc);
    outb(0x3d5,(pos >> 8) & 0xff);
    outb(0x3d4,0xd);
    outb(0x3d5,pos & 0xff);
    curr_console_idx = idx;
    update_cursor_pos(console);
}