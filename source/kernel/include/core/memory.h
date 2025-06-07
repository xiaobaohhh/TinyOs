/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-03 13:39:44
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-06 14:45:55
 * @FilePath: \start\source\kernel\include\core\memory.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MEMORY_H
#define MEMORY_H

#include "comm/types.h"
#include "tools/bitmap.h"
#include "ipc/mutex.h"
#include "comm/boot_info.h"

#define MEM_EXT_START (1024 * 1024)
#define MEM_EXT_END (1024 * 1024 * 127)
#define MEM_PAGE_SIZE (4096)
#define MEM_EBDA_START (0x80000)

#define MEMORY_TASK_BASE (0x80000000)

typedef struct _addr_alloc_t 
{
    mutex_t mutex;
    bitmap_t bitmap;
    uint32_t start;
    uint32_t size;
    uint32_t page_size;
    
}addr_alloc_t;
typedef struct _memory_map_t
{
    void * vstart;
    void * vend;
    void * pstart;
    uint32_t perm;
}memory_map_t;
void memory_init(boot_info_t *boot_info);
uint32_t memory_create_user_space();

int memory_alloc_page_for(uint32_t addr,uint32_t size,int perm);
int memory_alloc_for_page_dir(uint32_t page_dir,uint32_t vaddr,uint32_t size,int perm);
uint32_t memory_copy(task_t * from,task_t * to);

#endif
