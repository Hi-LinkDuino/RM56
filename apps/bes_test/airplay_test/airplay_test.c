#include <string.h>
#include "hal_trace.h"
#include "hal_location.h"
#include "bwifi_interface.h"
#include "cmsis_os.h"
#include "audioflinger.h"

#define CONNECT_SSID    "airplay_test"
#define CONNECT_PWD     "12345678"

#ifdef AIRPLAY_SUPPORT
extern void test_shairport();
#endif

#if BLE_AUDIO_ENABLED && defined(BLE_V2)
extern void bes_bt_init(void);
extern bool app_is_stack_ready(void);
extern void app_bap_bc_src_start(uint8_t big_idx);
extern void app_bap_bc_scan_start(uint8_t scan_trigger_method);
extern void app_bis_set_src_pcm_input_type(uint8_t type);
extern void app_bap_bc_src_stop(uint8_t big_idx);
extern void app_bis_sink_stop(void);
extern void app_set_src_cmd(uint8_t src_cmd);
extern void app_set_sink_cmd(uint8_t sink_cmd);
extern void app_bap_bc_scan_stop(void);

void bap_bis_master_init()
{
    if(!app_is_stack_ready()) {
        bes_bt_init();
    }
    app_set_src_cmd(0);//check bis status,set stop
    app_bis_set_src_pcm_input_type(1);//1-wifi,0-linein, 2-bt
    app_bap_bc_src_start(0);//start
}

void bap_bis_slave_init()
{
    if(!app_is_stack_ready()) {
        bes_bt_init();
    }
    app_set_sink_cmd(0);
    app_bap_bc_scan_start(0);
}
#endif


static osSemaphoreId connect_sem;
osSemaphoreDef(connect_sem);

void airplay_wait_conn_event(void)
{
    osSemaphoreWait(connect_sem, osWaitForever);
}

void airplay_finish_conn_event()
{
    osSemaphoreRelease(connect_sem);
}

void airplay_create_sem()
{
    connect_sem = osSemaphoreCreate(osSemaphore(connect_sem), 0);
    if (connect_sem == NULL)
    {
        TRACE(0,"create connect_sem fail\n");
        return ;
    }
}

void airplay_handle(void const *pArg)
{
    while (1) {
        airplay_wait_conn_event();
#ifdef AIRPLAY_SUPPORT
        test_shairport();
        osDelay(100);
#endif

#ifdef _BIS_MASTER_TEST_
        bap_bis_master_init();
#endif
    }
}

static osThreadId airplay_tid = NULL;
osThreadDef(airplay_handle, osPriorityAboveNormal, 1, (64 * 1024), "airplay_hdl");

void airplay_wifi_connect_callback(WIFI_USER_EVT_ID event_id, void *arg)
{
    static int sta_num = 0;
    printf("%s event_id:%d\n", __func__, event_id);

    if (event_id == WIFI_USER_EVT_CONN_INTER_STATE) {
        BWIFI_CONNECT_INTER_STATE state = *(BWIFI_CONNECT_INTER_STATE *)arg;

        switch (state) {
        case INTER_STATE_AUTHENTICATING:
        case INTER_STATE_ASSOCIATING:
        case INTER_STATE_ASSOCIATED:
        case INTER_STATE_4WAY_HANDSHAKE:
        case INTER_STATE_GROUP_HANDSHAKE:
            break;
        case INTER_STATE_AUTH_REJECT:
            break;
        case INTER_STATE_AUTH_TIMEOUT:
        case INTER_STATE_ASSOC_REJECT:
        case INTER_STATE_ASSOC_TIMEOUT:
        case INTER_STATE_HANDSHAKE_FAIL:
            break;
        default:
            /* Unhandled intermediate states */
            return;
        }
    } else if (event_id == WIFI_USER_EVT_CONNECTED) {
        TRACE(0, "####wifi connect connected");
#if LWIP_SUPPORT
    } else if (event_id == WIFI_USER_EVT_GOT_IP) {
        TRACE(0, "####wifi device got ip");
        airplay_finish_conn_event();
#endif
    }else if (event_id == WIFI_USER_EVT_DISCONNECTED) {
        TRACE(0, "####wifi connect disconnect");
    }
}

void airplay_with_bis_entry()
{
#ifdef _BIS_SLAVE_TEST_
    bap_bis_slave_init();
    while (1) {
        osDelay(1000);
    }
#endif

    BWIFI_STATUS_T ret = BWIFI_STATUS_IDLE;
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONN_INTER_STATE, airplay_wifi_connect_callback);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONNECTED, airplay_wifi_connect_callback);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_GOT_IP, airplay_wifi_connect_callback);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_DISCONNECTED, airplay_wifi_connect_callback);

    airplay_create_sem();
    if (airplay_tid == NULL) {
        airplay_tid = osThreadCreate(osThread(airplay_handle), NULL);
    }

    bwifi_connect_to_ssid(CONNECT_SSID, CONNECT_PWD, 0, 0, NULL);

    ret = bwifi_get_current_status();
    while (BWIFI_STATUS_GOT_IP != ret) {
        bwifi_connect_to_ssid(CONNECT_SSID, CONNECT_PWD, 0, 0, NULL);
        osDelay(1000);
    }

    while (1) {
        osDelay(1000);
    }

}

