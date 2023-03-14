#ifndef APP_WIFI_BT_SLOT_TIME_THREAD_H
#define APP_WIFI_BT_TIME_SLOT_THREAD_H

#if !defined(__COEX_MODE_FDD__) || defined(__COEX_MODE_HYBRID__)
#include "stdint.h"
/******************************type defination******************************/
typedef enum {
    APP_BT_STOP          = 0, // bt a2dp/sco stop
    APP_BT_START         = 1, // bt a2dp/sco start
    APP_MESH_NONE        = 2, // no mesh device
    APP_MESH_IDLE        = 3, // mesh idle
    APP_MESH_BUSY        = 4, // control or connect mesh device
    APP_BT_ENABLE        = 5, // open bt scan
    APP_BT_ADV_ENABLE       = 6, // liteos only , BT close, only start bt adv boardcast
    APP_BT_TWS_CONNECTING   = 7, // try to connect tws

    APP_WIFI_BT_TS_NUM,
} APP_WIFI_BT_TS_E;

typedef struct {
    APP_WIFI_BT_TS_E type;
    uint8_t *p_data;
    uint32_t data_len;
} app_wifi_bt_ts_t;

typedef void (*app_wifi_bt_ts_cb_t)(uint8_t *data, uint32_t len);

/******************************function declaration******************************/
#ifdef __cplusplus
extern "C" {
#endif

void app_wifi_bt_ts_thread_init(void);
void app_wifi_bt_ts_cb_register(APP_WIFI_BT_TS_E type, app_wifi_bt_ts_cb_t cb);
int app_wifi_bt_ts_request(APP_WIFI_BT_TS_E type, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif  // #ifdef __COEX_MODE_HYBRID__
#endif

