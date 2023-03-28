/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "bwifi_interface.h"
#ifdef __SNIFFER_MODE__
#include "bes_sniffer.h"
#endif
#if LWIP_SUPPORT
#include "lwip/apps/sntp.h"
#include "sys_time.h"
#endif
#include "hal_trace.h"

int bes_softap_test(void)
{
#if defined(__AP_MODE__)
    char *ssid = "bes_softapd";
    char *passwd = "12345678";
    BWIFI_SEC_TYPE_T sec = BWIFI_SECURITY_WPA2;

    if (bwifi_set_softap_config(ssid, 0, 0, 0, sec, passwd)) {
        TRACE(0, "%s %s config failed", __func__, ssid);
        return -1;
    }
    if (bwifi_softap_start()) {
        TRACE(0, "%s %s open failed", __func__, ssid);
        return -1;
    }
    TRACE(0, "%s %s open success", __func__, ssid);
    osDelay(200000);
    bwifi_softap_stop();
    TRACE(0, "%s %s closed", __func__, ssid);
    return 0;
#else
    TRACE(0, "SoftAP mode is not supported!!");
    return -1;
#endif
}

#ifdef __SNIFFER_MODE__
static int wifi_sniffer_handler(const u8 *data, u16 data_len, int signal)
{
    const u8 *frame = data;

    if (data == NULL) {
        TRACE(0, "%s ldpc:%d", __func__, data_len);
        return 0;
    }

    TRACE(0, "%s data:%p, len:%d, rssi:%d", __func__, data, data_len, signal);
    TRACE(0, "	%02x %02x %02x %02x",
          frame[0], frame[1], frame[2], frame[3]);
    TRACE(0, "	%02x %02x %02x %02x %02x %02x",
          frame[4], frame[5], frame[6], frame[7], frame[8], frame[9]);
    TRACE(0, "	%02x %02x %02x %02x %02x %02x",
          frame[10], frame[11], frame[12], frame[13], frame[14], frame[15]);
    TRACE(0, "	%02x %02x %02x %02x %02x %02x",
          frame[16], frame[17], frame[18], frame[19], frame[20], frame[21]);
    TRACE(0, "	%02x %02x\n", frame[22], frame[23]);

    return 0;
}
#endif

int bes_sniffer_test(void)
{
#ifdef __SNIFFER_MODE__
    if (bes_sniffer_start(wifi_sniffer_handler) ||
        bes_sniffer_set_filter(1,1,1,1,0,0,0))
        return -1;

    osDelay(20000);

    return bes_sniffer_stop();
#else
    TRACE(0, "Sniffer mode is not supported!!");
    return -1;
#endif
}

int bes_ntp_test(void)
{
#if LWIP_SUPPORT
    set_ntp_update();
    sntp_setservername(0,"203.107.6.88"); // ntp5.aliyun.com
    sntp_setservername(1,"cn.pool.ntp.org");
    sntp_setservername(2,"tw.pool.ntp.org");
    sntp_init();
    while (get_ntp_ready() == 0)
    {
        osDelay(100);
    }
    TRACE(0, "%s get ntp time %ds [%s]!", __func__, (u32_t)hal_sys_time_get(), hal_sys_format_time(hal_sys_time_get()));
    sntp_stop();
    return 0;
#else
    TRACE(0, "LWIP function in SDK is disabled!!");
    return -1;
#endif
}
