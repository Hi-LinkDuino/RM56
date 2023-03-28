#ifndef __VOW_KEY_H__
#define __VOW_KEY_H__

typedef enum {
    KEY_RECORD_IDLE = 0,
    KEY_RECORD_START =1,
    KEY_RECORD_STOP =2,
    KEY_QTY_NUM,
} KEY_STATUS_T;


KEY_STATUS_T vow_get_key_status ();
void vow_key_init();

#endif //__APP_KEY_H__
