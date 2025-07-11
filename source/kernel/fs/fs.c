/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-09 14:31:30
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-24 19:51:55
 * @FilePath: \start\source\kernel\fs\fs.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "fs/fs.h"
#include "tools/klib.h"
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include <sys/stat.h>
#include "tools/log.h"
#include "dev/console.h"
#include "fs/file.h"
#include "dev/dev.h"
#include "core/task.h"
#include "tools/list.h"
#include <sys/file.h>
#include "dev/disk.h"
#include "os_cfg.h"
#define FS_TABLE_SIZE 10
static list_t mounted_list;
static fs_t fs_table[FS_TABLE_SIZE];
static list_t free_list;

extern fs_op_t devfs_op;
extern fs_op_t fatfs_op;
static fs_t * root_fs;




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

static int is_path_valid(const char *path)
{
    if(path == (const char *)0 || path[0] == '\0')
    {
        return 0;
    }
    return 1;
}
int path_to_num(const char * path,int * num)
{
    int n = 0;
    const char * c = path;
    while(*c)
    {
        n = n * 10 + *c - '0';
        c++;
    }
    *num = n;
    return 0;
}
const char * path_next_child(const char * path)
{
    const char * c = path;
    while(*c && (*c++ == '/'));
    while(*c && (*c++ != '/'));

    return *c ? c : (const char *)0;
}

static void fs_protect(fs_t * fs)
{
    if(fs->mutex)
    {
        mutex_lock(fs->mutex);
    }
}
static void fs_unprotect(fs_t * fs)
{
    if(fs->mutex)
    {
        mutex_unlock(fs->mutex);
    }
}

int path_begin_with(const char * path,const char * str)
{
    const char * s1 = path;
    const char * s2 = str;
    while(*s1 && *s2 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *s2 == '\0';
}
int sys_open(const char *name, int flags, ...)
{
    file_t * file = file_alloc();
    if(!file)
    {
        return -1;
    }
    int fd = task_alloc_fd(file);
    if(fd < 0)
    {
        goto sys_open_failed;
    }
    fs_t * fs = (fs_t *)0;
    list_node_t * node = list_first(&mounted_list);
    while(node)
    {
        fs_t * curr = list_node_parent(node,fs_t,node);
        if(path_begin_with(name,curr->mount_point))
        {
            fs = curr;
            break;
        }
        node = list_node_next(node);
    }

    if(fs)
    {
        name = path_next_child(name);
    }
    else
    {
        fs = root_fs;
    }
    file->mode = flags;
    file->fs = fs;

    fs_protect(fs);
    kernel_strncpy(file->file_name,name,FILE_NAME_SIZE);
    int err = fs->op->open(fs,name,file);
    if(err < 0)
    {
        fs_unprotect(fs);
        goto sys_open_failed;
    }
    fs_unprotect(fs);
    return fd;
sys_open_failed:
    file_free(file);
    if(fd >= 0)
    {
        task_remove_fd(fd);
    }
    return -1;
}
int sys_read(int file, char *ptr, int len)
{
    
    if(is_bad_fd(file) || !ptr || !len)
    {
        return 0;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1;
    }
    if(p_file->mode == O_WRONLY)
    {
        return -1;
    }
    fs_t * fs = p_file->fs;
    fs_protect(fs);
    int err = fs->op->read(ptr,len,p_file);
    fs_unprotect(fs);
    return err;
}
int sys_write(int file, char *ptr, int len)
{
   if(is_bad_fd(file) || !ptr || !len)
    {
        return 0;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1;
    }
    if(p_file->mode == O_RDONLY)
    {
        return -1;
    }
    fs_t * fs = p_file->fs;
    fs_protect(fs);
    int err = fs->op->write(ptr,len,p_file);
    fs_unprotect(fs);
    return err;
}
int sys_lseek(int file, int offset, int dir)
{

    if(is_bad_fd(file))
    {
        return 0;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1;
    }
    fs_t * fs = p_file->fs;
    fs_protect(fs);
    int err = fs->op->seek(p_file,offset,dir);
    fs_unprotect(fs);
    return err;
}
int sys_close(int file)
{
    if(is_bad_fd(file))
    {
        return 0;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1;
    }
    ASSERT(p_file->ref > 0);
    
    if(p_file->ref-- == 1)
    {
        fs_t * fs = p_file->fs;
        fs_protect(fs);
        fs->op->close(p_file);
        fs_unprotect(fs);
        file_free(p_file);
    }

    task_remove_fd(file);
    return 0;
}

int sys_isatty(int file)
{
    if(is_bad_fd(file))
    {
        return 0;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1;
    }
    return p_file->type == FILE_TTY;
}

int sys_fstat(int file, struct stat * st)
{
    if(is_bad_fd(file))
    {
        return 0;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1;
    }
    fs_t * fs = p_file->fs;
    kernel_memset(st,0,sizeof(struct stat));
    fs_protect(fs);
    int err = fs->op->stat(p_file,st);
    fs_unprotect(fs);
    return err;
}

static void mount_list_init()
{
    list_init(&free_list);
    for(int i = 0; i < FS_TABLE_SIZE; i++)
    {
        list_insert_first(&free_list,&fs_table[i].node);
    }
    list_init(&mounted_list);
}

static fs_op_t * get_fs_op (fs_type_t type, int major)
{
    switch(type)
    {
        case FS_FAT16:
            return &fatfs_op;
        case FS_DEVFS:
            return &devfs_op;
        default:
            return (fs_op_t *)0;
    }
}
static fs_t * mount(fs_type_t type,const char * mount_point,int dev_major,int dev_minor)
{
    fs_t * fs = (fs_t *)0;
    log_printf("mount %s %d %d\n",mount_point,dev_major,dev_minor);

    list_node_t * cur_node = list_first(&mounted_list);
    while(cur_node)
    {
        fs_t * cur_fs = list_node_parent(cur_node,fs_t,node);
        if(kernel_strncmp(cur_fs->mount_point,mount_point,FS_MOUNTP_SIZE) == 0)
        {
            log_printf("mount point %s already mounted\n",mount_point);
            goto mount_failed;
        }
        cur_node = list_node_next(cur_node);
    }

    list_node_t * free_node = list_remove_first(&free_list);
    if(!free_node)
    {
        log_printf("no free fs\n");
        goto mount_failed;
    }
    fs = list_node_parent(free_node,fs_t,node);
    fs_op_t * op = get_fs_op(type,dev_major);
    if(!op)
    {
        log_printf("get fs op failed\n");
        goto mount_failed;
    }
    kernel_memset(fs,0,sizeof(fs_t));
    kernel_strncpy(fs->mount_point,mount_point,FS_MOUNTP_SIZE);
    fs->op = op;
    if(op->mount(fs,dev_major,dev_minor) < 0)
    {
        log_printf("mount failed\n");
        goto mount_failed;
    }
    list_insert_last(&mounted_list,&fs->node);
    return fs;
mount_failed:
    if(fs)
    {
        list_insert_last(&free_list,&fs->node);
    }
    return (fs_t *)0;
    
}
void fs_init()
{
    file_table_init();
    mount_list_init();
    disk_init();
    fs_t * fs = mount(FS_DEVFS,"/dev",0,0);
    ASSERT(fs != (fs_t *)0);
    root_fs = mount(FS_FAT16,"/home",ROOT_DEV);
    ASSERT(root_fs != (fs_t *)0);
}

int is_bad_fd(int file)
{
    return file < 0 || file >= TASK_OFILE_NR;
}
int sys_dup(int file)
{
    if(is_bad_fd(file))
    {
        return -1;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1; 
    }
    int fd = task_alloc_fd(p_file);
    if(fd >= 0)
    {
        file_inc_ref(p_file);
        return fd;
    }
    return -1;
}

int sys_opendir(const char * path,DIR * dir)
{
    fs_protect(root_fs);
    int err = root_fs->op->opendir(root_fs,path,dir);
    fs_unprotect(root_fs);
    return err;
}

int sys_ioctl(int file,int cmd,int arg0,int arg1)
{
    if(is_bad_fd(file))
    {
        return -1;
    }
    file_t * p_file = task_file(file);
    if(!p_file)
    {
        return -1; 
    }
    fs_t * fs = p_file->fs;
    fs_protect(fs);
    int err = fs->op->ioctl(p_file,cmd,arg0,arg1);
    fs_unprotect(fs);
    return err;
}
int sys_readdir(DIR * dir,struct dirent * dirent)
{
    fs_protect(root_fs);
    int err = root_fs->op->readdir(root_fs,dir,dirent);
    fs_unprotect(root_fs);
    return err;
}
int sys_closedir(DIR * dir)
{
    fs_protect(root_fs);
    int err = root_fs->op->closedir(root_fs,dir);
    fs_unprotect(root_fs);
    return err;
}

int sys_unlink(const char * pathname)
{
    fs_protect(root_fs);
    int err = root_fs->op->unlink(root_fs,pathname);
    fs_unprotect(root_fs);
    return err;
}