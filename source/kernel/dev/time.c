/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-05-28 20:24:48
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-06 22:18:11
 * @FilePath: \start\source\kernel\dev\time.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "dev/time.h"
#include "comm/types.h"
#include "cpu/irq.h"
#include "os_cfg.h"
#include "comm/cpu_instr.h"
static uint32_t sys_tick;
void do_handler_time(exception_frame_t *frame)
{
    sys_tick++;
    pic_send_eoi(IRQ0_TIMER);
    task_time_tick();

    
}
static void init_pit(void)
{
    uint32_t reload_count = (PIT_OSC_FREQ * OS_TICK_MS )/ 1000.0;
    outb(PIT_COMMAND_MODE_PORT,PIT_CHANNEL | PIT_LOAD_LOHI | PIT_MODE3);
    outb(PIT_CHANNEL0_DATA_PORT,reload_count & 0xff);
    outb(PIT_CHANNEL0_DATA_PORT,(reload_count >> 8) & 0xff);
    irq_install(IRQ0_TIMER,exception_handler_time);

    irq_enable(IRQ0_TIMER);
}
void time_init(void)
{
    sys_tick = 0;
    init_pit();
}
