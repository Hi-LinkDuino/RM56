#include "app_key.h"
#include "vow_key.h"
#include "hal_key.h"

#include "hal_trace.h"

extern void record_evnet_happen();

KEY_STATUS_T vow_key_evnet = KEY_RECORD_IDLE;

KEY_STATUS_T vow_get_key_status (void)
{
    return vow_key_evnet;
}

int app_key_switch_source(uint32_t key_code, uint8_t key_event)
{
    if (key_event == APP_KEY_EVENT_DOWN) {
        if(key_code == APP_KEY_CODE_FN1) {
            TRACE(0, "######APP_KEY_CODE_FN1 DOWN");
            vow_key_evnet = KEY_RECORD_START;
            record_evnet_happen();
        } else if(key_code == APP_KEY_CODE_FN2) {
            TRACE(0, "######APP_KEY_CODE_FN2 DOWN");
        } else if(key_code == APP_KEY_CODE_FN3) {
            TRACE(0, "######APP_KEY_CODE_FN3 DOWN");
        } else if(key_code == APP_KEY_CODE_FN4) {
            TRACE(0, "######APP_KEY_CODE_FN4 DOWN");
        }
    } else if (key_event == APP_KEY_EVENT_UP) {
        if(key_code == APP_KEY_CODE_FN1) {
            TRACE(0, "######APP_KEY_CODE_FN1 UP");
            vow_key_evnet = KEY_RECORD_STOP;
            record_evnet_happen();

        }
    }

    return 0;
}


void vow_key_init()
{
     hal_key_open(false, app_key_switch_source);
}

