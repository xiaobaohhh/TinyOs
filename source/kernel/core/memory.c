/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-03 13:45:11
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-03 22:50:48
 * @FilePath: \start\source\kernel\core\memory.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "core/memory.h"
#include  "tools/bitmap.h"
#include "tools/log.h"
#include "tools/klib.h"
#include "cpu/mmu.h"
static addr_alloc_t paddr_alloc;

static pde_t kernel_page_dir[PDE_CNT] __attribute__((aligned(MEM_PAGE_SIZE)));
static void addr_alloc_init(addr_alloc_t *addr_alloc,uint8_t* bits, 
    uint32_t start, uint32_t size, uint32_t page_size)
{
    mutex_init(&addr_alloc->mutex);
    
    addr_alloc->start = start;
    addr_alloc->size = size;
    addr_alloc->page_size = page_size;
    bitmap_init(&addr_alloc->bitmap, bits, size / page_size, 0);
    
}

static uint32_t addr_alloc_page(addr_alloc_t *addr_alloc, int page_count)
{
    uint32_t  addr = 0;
    mutex_lock(&addr_alloc->mutex);
    int page_index = bitmap_alloc_nbits(&addr_alloc->bitmap, 0, page_count);
    if (page_index >= 0)
    {
        addr = addr_alloc->start + page_index * addr_alloc->page_size;
    }
    mutex_unlock(&addr_alloc->mutex);
    return addr;
}

static void addr_free_page(addr_alloc_t *addr_alloc, uint32_t addr, int page_count)
{
    mutex_lock(&addr_alloc->mutex);
    uint32_t page_index = (addr - addr_alloc->start) / addr_alloc->page_size;
    bitmap_set_bit(&addr_alloc->bitmap, page_index, page_count,0);
    mutex_unlock(&addr_alloc->mutex);
}

void show_mem_info(boot_info_t *boot_info)
{
    for(int i = 0; i < boot_info->ram_region_count; i++)
    {
        log_printf("ram region %d: 0x%x - 0x%x, %d\n", i, boot_info->ram_region_cfg[i].start, 
        boot_info->ram_region_cfg[i].start + boot_info->ram_region_cfg[i].size,
        boot_info->ram_region_cfg[i].size);
    }
}
static uint32_t total_mem_size(boot_info_t *boot_info)
{
    uint32_t total_size = 0;
    for(int i = 0; i < boot_info->ram_region_count; i++)
    {
        total_size += boot_info->ram_region_cfg[i].size;
    }
    return total_size;
}

pte_t * find_pte(pde_t *page_dir, uint32_t vaddr, int alloc)
{
    pte_t *page_table = (pte_t *)0;
    pde_t *pde = page_dir + pde_index(vaddr);
    if (pde->present)
    {
        page_table = (pte_t *)pde_paddr(pde);
    }
    else
    {
        if (alloc == 0)
        {
            return (pte_t *)0;
        }
        else
        {
            uint32_t pg_paddr = addr_alloc_page(&paddr_alloc, 1);
            if (pg_paddr == 0)
            {
                return (pte_t *)0;
            }
            page_table = (pte_t *)pg_paddr;
            kernel_memset(page_table, 0, MEM_PAGE_SIZE);        
        }
    }
    return page_table + pte_index(vaddr);
    
    
}
int memory_create_map(pde_t *page_dir, uint32_t vaddr, uint32_t paddr, int count, uint32_t perm)
{
    for(int i = 0; i < count; i++)
    {
        log_printf("create map %d: 0x%x - 0x%x, 0x%x, perm :%d\n", i, vaddr, vaddr + MEM_PAGE_SIZE, paddr,perm);
        pte_t * pte = find_pte(page_dir, vaddr,1);
        if (pte == (pte_t *)0)
        {
            return -1;
        }
        log_printf("pte addr: 0x%x\n", pte);
        ASSERT(pte->present == 0);
        pte->v = paddr | perm | PTE_P;

        vaddr += MEM_PAGE_SIZE;
        paddr += MEM_PAGE_SIZE;
    }
    return 0;
}
void create_kernel_table(void)
{
    extern uint8_t s_text[], e_text[], s_data[];
    extern uint8_t kernel_base[];
    static memory_map_t kernel_map[] = {
        {kernel_base,s_text,0,0},
        {s_text,e_text,s_text,0},
        {s_data,(void *)MEM_EBDA_START,s_data,0},
    };
    for(int i = 0; i < sizeof(kernel_map) / sizeof(memory_map_t); i++)
    {
        memory_map_t *map = &kernel_map[i];
        uint32_t vstart = down2((uint32_t)map->vstart, MEM_PAGE_SIZE);
        uint32_t vend = up2((uint32_t)map->vend, MEM_PAGE_SIZE);
        int page_count = (vend - vstart) / MEM_PAGE_SIZE;

        memory_create_map(kernel_page_dir,vstart, (uint32_t)map->pstart,page_count,map->perm);
    }


}
void memory_init(boot_info_t *boot_info)
{
    extern uint8_t *mem_free_start;
    log_printf("mem init\n");
    show_mem_info(boot_info);

    uint8_t *mem_free = (uint8_t *)&mem_free_start;
    uint32_t mem_up1MB_free_size = total_mem_size(boot_info) - MEM_EXT_START;
    mem_up1MB_free_size = down2(mem_up1MB_free_size,MEM_PAGE_SIZE);
    log_printf("free memory begin addr: 0x%x ,size 0x%x\n", MEM_EXT_START, mem_up1MB_free_size);
    
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_up1MB_free_size, MEM_PAGE_SIZE);
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);
    
    ASSERT(mem_free < (uint8_t *)MEM_EBDA_START);

    create_kernel_table();
    mmu_set_page_dir((uint32_t)kernel_page_dir);
}
