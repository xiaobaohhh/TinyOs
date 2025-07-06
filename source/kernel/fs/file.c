/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-21 14:47:18
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-21 15:08:49
 * @FilePath: \start\source\kernel\fs\file.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "fs/file.h"
#include "ipc/mutex.h"
static file_t file_table[FILE_TABLE_SIZE];
static mutex_t file_alloc_mutex;

file_t * file_alloc()
{
    file_t * file = (file_t *)0;
    mutex_lock(&file_alloc_mutex);
    for(int i = 0; i < FILE_TABLE_SIZE; i++)
    {
        if(file_table[i].ref == 0)
        {
            kernel_memset(&file_table[i],0,sizeof(file_t));
            file_table[i].ref = 1;
            file = &file_table[i];
            break;
        }   
    } 
    mutex_unlock(&file_alloc_mutex);
    return file;
}


void file_table_init()
{
    mutex_init(&file_alloc_mutex);
    kernel_memset(file_table,0,sizeof(file_table));
}

void file_free(file_t *file)
{
    mutex_lock(&file_alloc_mutex);
    if(file->ref)
    {
        file->ref--;
    }
    mutex_unlock(&file_alloc_mutex);
}


void file_inc_ref(file_t *file)
{
    mutex_lock(&file_alloc_mutex);
    file->ref++;
    mutex_unlock(&file_alloc_mutex);
}


