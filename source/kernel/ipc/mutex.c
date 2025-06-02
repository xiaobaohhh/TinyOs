/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-02 21:40:02
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-02 22:00:40
 * @FilePath: \start\source\kernel\ipc\mutex.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ipc/mutex.h"
#include "cpu/irq.h"

void mutex_init(mutex_t *mutex)
{
    mutex->owner = (task_t *)0;
    mutex->locked_count = 0;
    list_init(&mutex->wait_list);
}

void mutex_lock(mutex_t *mutex)
{
    irq_state_t state = irq_enter_protection();
    task_t *curr = task_current();
    if(mutex->locked_count == 0)
    {
        mutex->locked_count++;
        mutex->owner = curr;
    }else if(mutex->owner == curr)
    {
        mutex->locked_count++;
    }else
    {
        task_set_block(curr);
        list_insert_last(&mutex->wait_list,&curr->wait_node);
        schedule_switch();
    }
    irq_leave_protection(state);
}

void mutex_unlock(mutex_t *mutex)
{
    irq_state_t state = irq_enter_protection();

    task_t * curr = task_current();
    if(mutex->owner == curr)
    {
        if(--mutex->locked_count == 0)
        {
            mutex->owner = (task_t *)0;
            if(list_count(&mutex->wait_list))
            {
                list_node_t * node = list_remove_first(&mutex->wait_list);
                task_t * task = list_node_parent(node,task_t,wait_node);
                task_set_ready(task);
                mutex->locked_count = 1;
                mutex->owner = task;
                schedule_switch();
            }
        }
    }
    irq_leave_protection(state);
}

