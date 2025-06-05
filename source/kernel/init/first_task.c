#include "core/task.h"
#include "tools/log.h"

int first_task_main()
{
    for(;;)
    {
        log_printf("first task\n");
        sys_sleep(1000);
    }
    return 0;
} 