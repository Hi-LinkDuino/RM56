#include "audioflinger.h"
#include <string.h>
#include "hal_trace.h"
#include "hal_location.h"
#include "bwifi_interface.h"
#include "cmsis_os.h"

#define CONNECT_SSID    "CMW-AP"
#define CONNECT_PWD     "12345678"
#define DEVICE_AS_MASTER 0

extern void dlna_run(int is_master);
extern void dlna_stop();
extern void set_porting_socket_interface_default();

static osSemaphoreId connect_sem;
osSemaphoreDef(connect_sem);

void dlna_wait_conn_event(void)
{
    osSemaphoreWait(connect_sem, osWaitForever);
}

void dlna_finish_conn_event()
{
    osSemaphoreRelease(connect_sem);
}

void dlna_create_sem()
{
    connect_sem = osSemaphoreCreate(osSemaphore(connect_sem), 0);
    if (connect_sem == NULL)
    {
        TRACE(0,"create connect_sem fail\n");
        return ;
    }
}

void dlna_device_as_slave()
{
    dlna_run(0);
}

void dlna_device_as_master()
{
    dlna_run(1);
}

void dlna_handle(void const *pArg)
{
    while (1) {
        dlna_wait_conn_event();
        set_porting_socket_interface_default();
#if DEVICE_AS_MASTER
        dlna_device_as_master();
#else
        dlna_device_as_slave();
#endif
    }
}

static osThreadId dlna_tid = NULL;
osThreadDef(dlna_handle, osPriorityAboveNormal, 1, (8 * 1024), "dlna_handle");

void dlna_wifi_connect_callback(WIFI_USER_EVT_ID event_id, void *arg)
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
        dlna_finish_conn_event();
#endif
    }else if (event_id == WIFI_USER_EVT_DISCONNECTED) {
        TRACE(0, "####wifi connect disconnect");
    }
}

void dlna_test_entry()
{
    BWIFI_STATUS_T ret = BWIFI_STATUS_IDLE;

    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONN_INTER_STATE, dlna_wifi_connect_callback);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONNECTED, dlna_wifi_connect_callback);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_GOT_IP, dlna_wifi_connect_callback);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_DISCONNECTED, dlna_wifi_connect_callback);

    dlna_create_sem();
    if (dlna_tid == NULL) {
        dlna_tid = osThreadCreate(osThread(dlna_handle), NULL);
    }

    bwifi_connect_to_ssid(CONNECT_SSID, CONNECT_PWD, 0, 0, NULL);

    ret = bwifi_get_current_status();
    while (BWIFI_STATUS_GOT_IP != ret) {
        bwifi_connect_to_ssid(CONNECT_SSID, CONNECT_PWD, 0, 0, NULL);
        osDelay(3000);
        break;
    }
}