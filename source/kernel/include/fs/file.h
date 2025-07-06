/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-21 14:42:58
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-24 16:11:11
 * @FilePath: \start\source\kernel\include\fs\file.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef FILE_H
#define FILE_H

#include "comm/types.h"

#define FILE_NAME_SIZE 32
#define FILE_TABLE_SIZE 2048


struct _fs_t;
typedef enum _file_type_t
{
    FILE_UNKNOWN = 0,
    FILE_TTY,
    FILE_DIR,
    FILE_NORMAL,
    
}file_type_t;
typedef struct _file_t
{
    char file_name[FILE_NAME_SIZE];
    file_type_t type;
    uint32_t size;
    int ref;
    int dev_id;
    int pos;
    int mode;

    int sblk;
    int cblk;
    int p_index;
    struct _fs_t * fs;
}file_t;

file_t * file_alloc();
void file_free(file_t *file);

void file_inc_ref(file_t *file);
void file_table_init();
#endif