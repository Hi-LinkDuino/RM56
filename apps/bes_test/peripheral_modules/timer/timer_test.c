#include "hal_timer.h"
#include "hal_trace.h"
#include "hwtimer_list.h"

#ifdef TIMER_PERIPHERAL_TEST

#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))

static void hwtimer_handler(void *arg)
{
    unsigned int last = *(unsigned int *)arg;
    printf("%s: used time %u ms\r\n", __func__, GET_CURRENT_MS() - last);
    // This hwtimer is HAL_TIMER_TYPE_ONESHOT
    // To restart it, You can use: hwtimer_start(hw_timer, MS_TO_TICKS(1000));
}

void hwtimer_test(void)
{
    static unsigned int now;
    now = GET_CURRENT_MS();
    HWTIMER_ID hw_timer = hwtimer_alloc(hwtimer_handler, &now);
    hwtimer_start(hw_timer, MS_TO_TICKS(1000));
    mdelay(2000);
    hwtimer_free(hw_timer);
    printf("%s done\r\n", __func__);
}
#endif
