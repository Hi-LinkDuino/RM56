#ifndef __HEART_BEAT_H__
#define __HEART_BEAT_H__

#ifdef __cplusplus
extern "C" {
#endif

void heartbeat_init(void);
#define A7_DSP_BOOT_RETRY_NUM       3
//#define A7_RECOVER_TEST_CASE    1
typedef enum {
    A7_RECOVERING = 1,
    A7_RECOVER_DONE,
    A7_USER_DISABLE,
    A7_USER_ENABLE,
}A7_RECOVER_EVENT;

#define A7_RESET_RECOVER_CB_MAX     5
/* Register a7 reset recover subscribe func, callback function don't block*/
typedef void (*a7_recover_cb_t)(int status);
a7_recover_cb_t *a7_dsp_get_recover_cb(void);
void a7_dsp_recover_subscribe(a7_recover_cb_t cbf);
void a7_dsp_recover_unsubscribe(a7_recover_cb_t cbf);
void a7_dsp_recover_report_event(A7_RECOVER_EVENT event);
void heartbeat_stop_timer(void);
void a7_dsp_set_crash_status(int crash);
A7_RECOVER_EVENT a7_dsp_status_get(void);
#ifdef __cplusplus
}
#endif

#endif /* __HEART_BEAT_H__ */