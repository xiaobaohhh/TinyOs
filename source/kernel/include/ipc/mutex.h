#ifndef MUTEX_H
#define MUTEX_H

#include "tools/list.h"
#include "core/task.h"

typedef struct _mutex_t
{
    list_t wait_list;
    task_t *owner;
    int locked_count;
}mutex_t;

void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);



#endif


