#include "cmsis_os2.h"
#include "hal_trace.h"
#include "hal_timer.h"


uint32_t lv_sys_time(void)
{
    return GET_CURRENT_MS();
}
