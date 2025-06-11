/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-03 13:45:11
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-11 14:33:13
 * @FilePath: \start\source\kernel\core\memory.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "core/memory.h"
#include  "tools/bitmap.h"
#include "tools/log.h"
#include "tools/klib.h"
#include "cpu/mmu.h"
#include "cpu/irq.h"

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
    //mutex_lock(&addr_alloc->mutex);
    irq_state_t state = irq_enter_protection();
    int page_index = bitmap_alloc_nbits(&addr_alloc->bitmap, 0, page_count);
    if (page_index >= 0)
    {
        addr = addr_alloc->start + page_index * addr_alloc->page_size;
    }
    irq_leave_protection(state);
    //mutex_unlock(&addr_alloc->mutex);
    return addr;
}

static void addr_free_page(addr_alloc_t *addr_alloc, uint32_t addr, int page_count)
{
    //mutex_lock(&addr_alloc->mutex);
    irq_state_t state = irq_enter_protection();
    uint32_t page_index = (addr - addr_alloc->start) / addr_alloc->page_size;
    bitmap_set_bit(&addr_alloc->bitmap, page_index, page_count,0);
    irq_leave_protection(state);
    //mutex_unlock(&addr_alloc->mutex);
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
            pde->v = pg_paddr | PDE_P | PDE_W | PDE_U;
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
        //log_printf("create map %d: 0x%x - 0x%x, 0x%x, perm :%d\n", i, vaddr, vaddr + MEM_PAGE_SIZE, paddr,perm);
        pte_t * pte = find_pte(page_dir, vaddr,1);
        if (pte == (pte_t *)0)
        {
            return -1;
        }
        //log_printf("pte addr: 0x%x\n", pte);
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
        {kernel_base,s_text,kernel_base,PTE_W},
        {s_text,e_text,s_text,0},
        {s_data,(void *)MEM_EBDA_START,s_data,PTE_W},
        {(void *)MEM_EXT_START,(void *)MEM_EXT_END,(void *)MEM_EXT_START,PTE_W}
    };
    for(int i = 0; i < sizeof(kernel_map) / sizeof(memory_map_t); i++)
    {
        memory_map_t *map = &kernel_map[i];
        uint32_t vstart = down2((uint32_t)map->vstart, MEM_PAGE_SIZE);
        uint32_t vend = up2((uint32_t)map->vend, MEM_PAGE_SIZE);
        uint32_t pstart = down2((uint32_t)map->pstart, MEM_PAGE_SIZE);
        int page_count = (vend - vstart) / MEM_PAGE_SIZE;
        memory_create_map(kernel_page_dir,vstart, (uint32_t)map->pstart,page_count,map->perm);
    }
}

uint32_t memory_create_user_space()
{
    pde_t * page_dir = (pde_t *)addr_alloc_page(&paddr_alloc, 1);
    if(page_dir == (pde_t *)0)
    {
        return 0;
    }
    kernel_memset((void *)page_dir, 0, MEM_PAGE_SIZE);
    uint32_t user_pde_start = pde_index(MEMORY_TASK_BASE);
    for(int i = 0; i < user_pde_start; i++)
    {
        page_dir[i].v = kernel_page_dir[i].v;
    }
    return (uint32_t)page_dir;
    
}
void memory_init(boot_info_t *boot_info)
{
    extern uint8_t *mem_free_start;
    //log_printf("mem init\n");
    show_mem_info(boot_info);

    uint8_t *mem_free = (uint8_t *)&mem_free_start;
    uint32_t mem_up1MB_free_size = total_mem_size(boot_info) - MEM_EXT_START;
    mem_up1MB_free_size = down2(mem_up1MB_free_size,MEM_PAGE_SIZE);
    //log_printf("free memory begin addr: 0x%x ,size 0x%x\n", MEM_EXT_START, mem_up1MB_free_size);
    
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_up1MB_free_size, MEM_PAGE_SIZE);
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);
    
    ASSERT(mem_free < (uint8_t *)MEM_EBDA_START);

    create_kernel_table();
    mmu_set_page_dir((uint32_t)kernel_page_dir);
}

uint32_t memory_alloc_page (void) {
    // 内核空间虚拟地址与物理地址相同
    return addr_alloc_page(&paddr_alloc, 1);
}
static pde_t * current_page_dir(void)
{
    return (pde_t *)(task_current()->tss.cr3);
}
void memory_free_page (uint32_t addr) {
    if (addr < MEMORY_TASK_BASE) {
        // 内核空间，直接释放
        addr_free_page(&paddr_alloc, addr, 1);
    } else {
        // 进程空间，还要释放页表
        pte_t * pte = find_pte(current_page_dir(), addr, 0);
        ASSERT((pte == (pte_t *)0) && pte->present);

        // 释放内存页
        addr_free_page(&paddr_alloc, pte_paddr(pte), 1);

        // 释放页表
        pte->v = 0;
    }
}



int memory_alloc_for_page_dir(uint32_t page_dir,uint32_t vaddr,uint32_t size,int perm)
{
    uint32_t current_vaddr = vaddr;

    int page_count = up2(size,MEM_PAGE_SIZE) / MEM_PAGE_SIZE;
    for(int i = 0; i < page_count; i++)
    {
        uint32_t paddr = addr_alloc_page(&paddr_alloc,1);
        if(paddr == 0)
        {
            return 0;
        }
        int err = memory_create_map((pde_t *)page_dir,current_vaddr,paddr,1,perm);
        if(err < 0)
        {
            log_printf("memory_alloc_for_page_dir error\n");
            return 0;
        }
        current_vaddr += MEM_PAGE_SIZE;
    }
    return 0;
}

int memory_alloc_page_for(uint32_t addr,uint32_t size,int perm)
{
    return memory_alloc_for_page_dir(task_current()->tss.cr3,addr,size,perm);
}


uint32_t memory_copy(task_t * from,task_t * to)
{
    pde_t * from_page_dir = (pde_t *)from->tss.cr3;
    pde_t * to_page_dir = (pde_t *)to->tss.cr3;
    
    uint32_t user_pde_start = pde_index(MEMORY_TASK_BASE);
    for(int i = user_pde_start; i < 1024; i++)
    {
        pde_t * from_pde = from_page_dir + i;
        pde_t * to_pde = to_page_dir + i;
        if(from_pde->present)
        {
            to_pde->v = from_pde->v;
        }
    }
    return 0;
    
}

uint32_t memory_copy_vum(uint32_t page_dir,uint32_t to_page_dir)
{
    // 复制用户空间的各项
    uint32_t user_pde_start = pde_index(MEMORY_TASK_BASE);
    pde_t * pde = (pde_t *)page_dir + user_pde_start;

    // 遍历用户空间页目录项
    for (int i = user_pde_start; i < PDE_CNT; i++, pde++) {
        if (!pde->present) {
            continue;
        }

        // 遍历页表
        pte_t * pte = (pte_t *)pde_paddr(pde);
        for (int j = 0; j < PTE_CNT; j++, pte++) {
            if (!pte->present) {
                continue;
            }

            // 分配物理内存
            uint32_t page = addr_alloc_page(&paddr_alloc, 1);
            if (page == 0) {
                goto copy_failed;
            }

            // 建立映射关系
            uint32_t vaddr = (i << 22) | (j << 12);
            int err = memory_create_map((pde_t *)to_page_dir, vaddr, page, 1, get_pte_perm(pte));
            if (err < 0) {
                goto copy_failed;
            }

            // 复制内容。
            kernel_memcpy((void *)page, (void *)vaddr, MEM_PAGE_SIZE);
        }
    }

    return 0;
copy_failed:
    return -1;
}

void memory_destroy_uvm(uint32_t page_dir)
{
     uint32_t user_pde_start = pde_index(MEMORY_TASK_BASE);
    pde_t * pde = (pde_t *)page_dir + user_pde_start;

    ASSERT(page_dir != 0);

    // 释放页表中对应的各项，不包含映射的内核页面
    for (int i = user_pde_start; i < PDE_CNT; i++, pde++) {
        if (!pde->present) {
            continue;
        }

        // 释放页表对应的物理页 + 页表
        pte_t * pte = (pte_t *)pde_paddr(pde);
        for (int j = 0; j < PTE_CNT; j++, pte++) {
            if (!pte->present) {
                continue;
            }

            addr_free_page(&paddr_alloc, pte_paddr(pte), 1);
        }

        addr_free_page(&paddr_alloc, (uint32_t)pde_paddr(pde), 1);
    }

    // 页目录表
    addr_free_page(&paddr_alloc, page_dir, 1);
}

uint32_t memory_get_paddr(uint32_t page_dir,uint32_t vaddr)
{
    pte_t * pte = find_pte((pde_t *)page_dir,vaddr,0);
    if(pte == (pte_t *)0)
    {
        return 0;
    }
    return pte_paddr(pte) + (vaddr & (MEM_PAGE_SIZE - 1));
}

int memory_copy_uvm_data(uint32_t to, uint32_t page_dir, uint32_t from, uint32_t size) {
    char *buf, *pa0;

    while(size > 0){
        // 获取目标的物理地址, 也即其另一个虚拟地址
        uint32_t to_paddr = memory_get_paddr(page_dir, to);
        if (to_paddr == 0) {
            return -1;
        }

        // 计算当前可拷贝的大小
        uint32_t offset_in_page = to_paddr & (MEM_PAGE_SIZE - 1);
        uint32_t curr_size = MEM_PAGE_SIZE - offset_in_page;
        if (curr_size > size) {
            curr_size = size;       // 如果比较大，超过页边界，则只拷贝此页内的
        }

        kernel_memcpy((void *)to_paddr, (void *)from, curr_size);

        size -= curr_size;
        to += curr_size;
        from += curr_size;
  }

  return 0;
}