#include "lv_app.h"
#include "cmsis_os2.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "examples/lv_examples.h"
#include "demos/benchmark/lv_demo_benchmark.h"
#include "demos/widgets/lv_demo_widgets.h"

#include "cmsis_os.h"

#if LV_USE_LOG && !LV_LOG_PRINTF
static void print_handler(const char *buf)
{
    hal_trace_output((const unsigned char *)buf, strlen(buf));
}
#endif
static    uint32_t tick;
static void lgvl_thread(const void *argument){
    printf("lv_main: %s %d ms\n", __func__, lv_tick_elaps(tick));

#if LV_USE_LOG && !LV_LOG_PRINTF
    lv_log_register_print_cb(print_handler);
#endif

    // lv_example_msgbox_1();
    lv_demo_benchmark();
    // lv_demo_widgets();
    bool flag = true;
    while (1) {
        wait_for_vsync();
        if(flag){
            printf("lv_main: %s(wait_for_vsync) %d ms\n", __func__, lv_tick_elaps(tick));
        }
        lv_task_handler();
        if(flag){
            printf("lv_main: %s(lv_task_handler) %d ms\n", __func__, lv_tick_elaps(tick));
            flag = false;
        }        
    }
}
osThreadId lvgl_thread_id;
osThreadDef(lgvl_thread, osPriorityAboveNormal, 1, (32 * 1024), "lvgl");

#ifdef LV_SHOW_LOGO
extern void lv_demo_logo(void);
#endif
void lv_main(void)
{
    tick = lv_tick_get();
    lv_init();
    printf("%s lv_init: %d ms\n", __func__, lv_tick_elaps(tick));
    lv_port_disp_init();
    printf("%s lv_port_disp_init: %d ms\n", __func__, lv_tick_elaps(tick));
    lv_port_indev_init();
    printf("%s lv_port_indev_init: %d ms\n", __func__, lv_tick_elaps(tick));
#ifdef LV_SHOW_LOGO
    lv_demo_logo();
    printf("%s lv_demo_logo: %d ms\n", __func__, lv_tick_elaps(tick));
#endif
// ASSERT(0, "lv_main");
    lvgl_thread_id = osThreadCreate(osThread(lgvl_thread), NULL);
    printf("%s %d lvgl_thread:%p\n", __func__, __LINE__, lvgl_thread_id);
}
