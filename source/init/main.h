/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-22 20:51:55
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-23 14:53:48
 * @FilePath: \start\source\shell\main.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MAIN_H
#define MAIN_H

#define CLI_INPUT_SIZE  1024
#define CLI_MAX_ARG_COUNT 10

#define ESC_CMD2(Pn,cmd)    "\x1b["#Pn#cmd  
#define ESC_CLEAR_SCREEN ESC_CMD2(2,J)
#define ESC_MOVE_CURSOR(row,col) "\x1b["#row";"#col"H"


#define ESC_COLOR_ERROR     ESC_CMD2(31,m)
#define ESC_COLOR_DEFAULT   ESC_CMD2(039,m)

typedef struct _cli_cmd_t
{
    const char * name;
    const char * usage;
    int (*do_func)(int argc,char **argv);
}cli_cmd_t;

typedef struct _cli_t
{
    char curr_input[CLI_INPUT_SIZE];
    const cli_cmd_t * cmd_start;
    const cli_cmd_t * cmd_end;
    const char * prompt;
}cli_t;






#endif