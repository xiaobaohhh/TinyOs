#ifndef DEV_DEV_H
#define DEV_DEV_H

#include "comm/types.h"
#define DEV_NAME_SIZE 32

enum
{
    DEV_UNKNOW = 0,
    DEV_TTY,
    DEV_DISK,
};

struct _dev_desc_t;
typedef struct _device_t
{
    struct _dev_desc_t *desc;
    int mode;
    int minor;
    void *data;
    int open_count;
}device_t;

typedef struct _dev_desc_t
{
    char name[DEV_NAME_SIZE];
    int major;

    int (*open)(device_t *dev);
    void (*close)(device_t *dev);
    int (*read)(device_t *dev, int addr, char * buf, int size);
    int (*write)(device_t *dev, int addr, char * buf, int size);
    int (*control)(device_t *dev, int cmd, int arg0, int arg1);
}dev_desc_t;


int dev_open(int major, int minor, void * data);
void dev_close(int dev_id);
int dev_read(int dev_id, int addr, char * buf, int size);
int dev_write(int dev_id, int addr, char * buf, int size);
int dev_control(int dev_id, int cmd, int arg0, int arg1);
#endif