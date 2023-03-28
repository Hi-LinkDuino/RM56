#include <stdio.h>
#include "besfd_api.h"

#include "bwifi_interface.h"
#include "hal_norflash.h"
#include "cmsis_os2.h"
#ifdef osCMSIS_FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "hal_timer.h"

#include "nvrecord_extension.h"
#ifdef CHIP_HAS_RGB_LED
#include "app_led.h"
#endif

static osTimerId g_wifi_timer = NULL;
static uint32_t g_wifi_timer_start = 0; //0 means timer stopped

static void _wifi_timer_handler(void const *param){
  int status = bwifi_get_current_status();
  TRACE(0, "%s: status=%d", __func__, status);
  if(status == BWIFI_STATUS_GOT_IP){
    osTimerStop(g_wifi_timer);
    osTimerDelete(g_wifi_timer);
    g_wifi_timer = NULL;
#if defined(PRJ_SWIFT) && !defined(SWIFT_HW_KEY_TEST)
    swift_led_scene_update();
#endif
  }
}

osTimerDef(WIFI_TIMER, _wifi_timer_handler);


extern void httpd_init(void);

/*
  1. switch to ap mode
  3. input 192.168.0.1/pw.html
  4. input ssid and password in pw.html
  5. submit
*/

#define DEFAULT_AP_SSID (char *)"swift wifi"
#define DEFAULT_AP_PASSWD (char *)"swift888"

bool wifi_in_ap_mode(void)
{
    return bwifi_get_softap_status() == BWIFI_SOFTAP_STATUS_ON;
}

static int __wifi_ap_mode(void){
    char *ssid, *passwd;
    int channel = 0;
    int sec_channel_offset = 0;
    int hidden = 0;

    if (bwifi_get_current_status() >= BWIFI_STATUS_CONNECTING)
    {
        bwifi_disconnect();
    }

    nv_record_get_softap_ssid_passwd(&ssid, &passwd);
    if (*ssid == 0)
    {
        ssid = DEFAULT_AP_SSID;
        passwd = DEFAULT_AP_PASSWD;
    }
    BWIFI_SEC_TYPE_T sec = *passwd ? BWIFI_SECURITY_WPA2 : BWIFI_SECURITY_NONE;

    int ret = bwifi_set_softap_config(ssid, (u8)channel, sec_channel_offset, (u8)hidden, sec, passwd);
    if (ret)
    {
        TRACE(2, "%s.bwifi_set_softap_config() returns %d", __func__, ret);
        return ret;
    }
    ret = bwifi_softap_start();
    if (ret)
    {
        TRACE(2, "%s.bwifi_softap_start() returns %d", __func__, ret);
        return ret;
    }
    httpd_init();
    // wait user input ap ssid&passwd via 192.168.0.1/pw.html

    return 0;
}


enum WIFI_ACTION
{
    actAP,
    actSTA,
    actToggle,
    actError,
};
/**
 * @brief switch wifi mode
 *
 * @param string    action to switch mode:
 *      - 'ap'      - switch to AP mode
 *      - 'sta'     - switch to STA mode
 *      - 'toggle'  - toggle between AP and STA mode
 *      - NULL      - same as 'toggle'
 */
int wifi_switch_mode(char *action)
{
    bool is_ap = wifi_in_ap_mode();

    enum WIFI_ACTION act = (!action || strcmp(action, "toggle") == 0) ? actToggle : (!action || strcmp(action, "ap") == 0) ? actAP
                                                                                : (!action || strcmp(action, "sta") == 0)  ? actSTA
                                                                                                                           : actError;
    if (act == actError)
    {
        TRACE(0, "%s unsupported action: %s", __func__, action);
        return -1;
    }

    TRACE(0, "%s: %s mode ====> %s", __func__, (char*)(is_ap ? "AP" : "STA"), action);
    if (!is_ap && (act == actAP || act == actToggle))
    {
        // switch to ap mode
#if defined(PRJ_SWIFT) && !defined(SWIFT_HW_KEY_TEST)
        swift_led_scene_update();
#endif
        return __wifi_ap_mode();
    }
    else if (is_ap && (act == actSTA || act == actToggle))
    {
        // switch to sta mode
        TRACE(2, "%s.bwifi_softap_stop()", __func__);
        bwifi_softap_stop();
    }

    if(act == actSTA || (is_ap && act == actToggle)){
#if defined(PRJ_SWIFT) && !defined(SWIFT_HW_KEY_TEST)
        swift_led_scene_update();
#endif
        if (BWIFI_STATUS_IDLE != bwifi_get_current_status()){
            TRACE(0, "%s: sta busy, state=%d", __func__, bwifi_get_current_status());
            return -1;
        }
        // load the last sta config from nv
        struct bwifi_station_config *p = nv_record_get_ap_info();
        if (!p)return 0;
        osDelay(1000);
        TRACE(2, "%s.connect ap: %s, %s", __func__, p->ssid, p->passwd);
        if (NULL == g_wifi_timer){
          g_wifi_timer = osTimerCreate(osTimer(WIFI_TIMER), osTimerPeriodic, NULL);
        }
        osTimerStart(g_wifi_timer, 500);        
        return bwifi_connect(p);
    }
    return -1;
}

/**
 * @brief wifi in STA mode and connected
 */
bool wifi_sta_connected(void){
    return !wifi_in_ap_mode() && bwifi_get_current_status() >= BWIFI_STATUS_GOT_IP;
}