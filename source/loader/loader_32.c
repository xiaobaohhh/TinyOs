#include "loader.h"
#include "comm/elf.h"
static void read_disk(uint32_t sector, uint32_t sector_count,uint8_t *buf)
{
    outb(0x1f6, 0xe0);
    outb(0x1f2, (uint8_t)(sector_count >> 8));
    outb(0x1f3, (uint8_t)(sector >> 24));
    outb(0x1f4, 0);
    outb(0x1f5, 0);
    
    outb(0x1f2, (uint8_t)(sector_count));
    outb(0x1f3, (uint8_t)(sector));
    outb(0x1f4, (uint8_t)(sector >> 8));
    outb(0x1f5, (uint8_t)(sector >> 16));
   
   outb(0x1f7, 0x24);

   uint16_t * data_buf = (uint16_t *)buf;
   while(sector_count--)
   {
        while((inb(0x1f7) & 0x08) != 0x08);
        for(int i = 0; i < SECTOR_SIZE / 2; i++)
        {
            *data_buf++ = inw(0x1f0);
        }
   }
}

static uint32_t reload_elf_file(uint8_t *file_buffer)
{
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr *)file_buffer;
    if(elf_hdr->e_ident[0] != 0x7f || elf_hdr->e_ident[1] != 'E' 
        || elf_hdr->e_ident[2] != 'L' 
        || elf_hdr->e_ident[3] != 'F')
    {
        return 0;
    }

    for(int i = 0; i < elf_hdr->e_phnum; i++)
    {
        Elf32_Phdr *phdr = (Elf32_Phdr *)(file_buffer + elf_hdr->e_phoff) + i;
        if(phdr->p_type != PT_LOAD)
        {
            continue;
        }
        uint8_t *src = file_buffer + phdr->p_offset;
        uint8_t *dest = (uint8_t *)phdr->p_paddr;
        for(int j = 0; j < phdr->p_filesz; j++)
        {
            *dest++ = *src++;
        }
        dest = (uint8_t *)phdr->p_paddr + phdr->p_filesz;
        for(int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++)
        {
            *dest++ = 0;
        }
    }
    return elf_hdr->e_entry;
}
static void die(int err_code)
{
    for(;;);
}
#define CR4_PSE (1 << 4)
#define CR0_PG (1 << 31)
#define PDE_P (1 << 0)
#define PDE_RW (1 << 1)
#define PDE_US (1 << 2)
#define PDE_PWT (1 << 3)
#define PDE_PCD (1 << 4)
#define PDE_A (1 << 5)
#define PDE_D (1 << 6)
#define PDE_PS (1 << 7)
void enable_page_mode(void)
{
    static uint32_t page_dir[1024] __attribute__((aligned(4096))) = {
        [0] = PDE_P | PDE_RW | PDE_PS
    };
    uint32_t cr4 = read_cr4();
    uint32_t cr0 = read_cr0();
    write_cr4(cr4 | CR4_PSE);
    write_cr3((uint32_t)page_dir);
    write_cr0(cr0 | CR0_PG);
}
void load_kernel(void)
{
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);

    uint32_t kernel_entry = reload_elf_file((uint8_t *)SYS_KERNEL_LOAD_ADDR);
    if(kernel_entry == 0)
    {
        die(-1);
    }
    enable_page_mode();
    ((void(*)(boot_info_t *))kernel_entry)(&boot_info);
    for(;;);
}  