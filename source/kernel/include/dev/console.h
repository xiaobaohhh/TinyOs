#ifndef CONSOLE_H
#define CONSOLE_H

#include "stdint.h"

#include "comm/types.h"
#include "dev/tty.h"
#include "ipc/mutex.h"
#define CONSOLE_DISP_ADDR 0xB8000
#define CONSOLE_DISP_END (0xB8000 + 32 * 1024)
#define CONSOLE_ROW_MAX 25
#define CONSOLE_COL_MAX 80
#define ASCII_ESC  0x1b
#define ESC_PARAM_MAX 10

typedef union _disp_char_t
{
    struct
    {
        char c;
        char foreground : 4;
        char background : 3;
    };
    uint16_t v;
}disp_char_t;

typedef enum 
{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_GREY = 7,
    COLOR_DARKGREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15,
}color_t;
typedef struct _console_t
{
    enum
    {
        CONSOLE_WRITE_NORMAL,
        CONSOLE_WRITE_ESC,
        CONSOLE_WRITE_SQUARE,
    }write_state;


    int esc_state;

    disp_char_t * disp_base;
    int display_row;
    int display_col;
    int cursor_row;
    int cursor_col;

    int old_cursor_row;
    int old_cursor_col;

    color_t foreground;
    color_t background;

    int esc_param[ESC_PARAM_MAX];
    int curr_param_index;
    mutex_t mutex;
}console_t;
typedef enum _cclor_t {
    COLOR_Black			= 0,
    COLOR_Blue			= 1,
    COLOR_Green			= 2,
    COLOR_Cyan			= 3,
    COLOR_Red			= 4,
    COLOR_Magenta		= 5,
    COLOR_Brown			= 6,
    COLOR_Gray			= 7,
    COLOR_Dark_Gray 	= 8,
    COLOR_Light_Blue	= 9,
    COLOR_Light_Green	= 10,
    COLOR_Light_Cyan	= 11,
    COLOR_Light_Red		= 12,
    COLOR_Light_Magenta	= 13,
    COLOR_Yellow		= 14,
    COLOR_White			= 15
}cclor_t;
int console_init(int idx);

int console_write(tty_t * tty);
void console_close(int console);
void console_select(int idx);
#endif