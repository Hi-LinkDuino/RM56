/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wifi_hal_ap_interface.h"
#include <errno.h>
#include <securec.h>
#include "wifi_hal.h"
#include "wifi_hal_ap_feature.h"
#include "wifi_hal_module_manage.h"
#include "wifi_hal_common_func.h"
#include "wifi_log.h"
#include "wifi_wpa_hal.h"
#include "wifi_hostapd_hal.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalApInterface"

#define NUMS_BAND 2
#define BUFF_SIZE 1024
#define DISABLE_AP_WAIT_MS 50000
static const char *g_serviceName = "hostapd";
static const char *g_startCmd = "hostapd /data/misc/wifi/wpa_supplicant/hostapd.conf";

WifiErrorNo StartSoftAp(void)
{
    LOGD("Ready to start hostapd");
    if (StartHostapd() != WIFI_HAL_SUCCESS) {
        LOGE("hostapd start failed!");
        return WIFI_HAL_OPEN_HOSTAPD_FAILED;
    }

    if (StartHostapdHal() != WIFI_HAL_SUCCESS) {
        LOGE("hostapd init failed!");
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }

    LOGD("AP start successfully!");
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartHostapd(void)
{
    const char *pConf = "/data/misc/wifi/wpa_supplicant/hostapd.conf";
    if ((access(pConf, F_OK)) != -1) {
        LOGD("wpa configure file %s is exist.", pConf);
    } else {
        char szCmd[BUFF_SIZE] = {0};
        const char *cpConfCmd = "cp /system/etc/wifi/hostapd.conf /data/misc/wifi/wpa_supplicant/";
        int iRet = snprintf_s(szCmd, sizeof(szCmd), sizeof(szCmd) - 1, "%s", cpConfCmd);
        if (iRet < 0) {
            return -1;
        }

        ExcuteCmd(szCmd);
    }
    ModuleManageRetCode ret = StartModule(g_serviceName, g_startCmd);
    if (ret == MM_SUCCESS) {
        return WIFI_HAL_SUCCESS;
    }

    LOGE("start hostapd failed!");
    return WIFI_HAL_FAILED;
}

WifiErrorNo StartHostapdHal(void)
{
    LOGD("Ready to init hostapd");
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopSoftAp(void)
{
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice != NULL) {
        int ret = hostapdHalDevice->disableAp();
        usleep(DISABLE_AP_WAIT_MS);
        if (ret != 0) {
            LOGE("disableAp failed! ret=%{public}d", ret);
        }
    } else {
        LOGE("can not get hostapd dev");
    }

    if (StopHostapd() != WIFI_HAL_SUCCESS) {
        LOGE("hostapd stop failed!");
        return WIFI_HAL_FAILED;
    }

    if (StopHostapdHal() != WIFI_HAL_SUCCESS) {
        LOGE("hostapd_hal stop failed!");
        return WIFI_HAL_FAILED;
    }

    LOGD("AP stop successfully!");
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopHostapd(void)
{
    ModuleManageRetCode ret;
    do {
        ret = StopModule(g_serviceName);
        if (ret == MM_FAILED) {
            LOGE("stop hostapd failed!");
            return WIFI_HAL_FAILED;
        }
    } while (ret == MM_REDUCE_REFERENCE);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopHostapdHal(void)
{
    ReleaseHostapdDev();
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetStaInfos(char *infos, int32_t *size)
{
    if (infos == NULL || size == NULL) {
        LOGE("GetStaInfos infos or size is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("GetStaInfos:Start");
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }
    if (hostapdHalDevice->showConnectedDevList(infos, *size) != 0) {
        LOGE("ShowConnectedDevList failed!");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetCountryCode(const char *code)
{
    if (code == NULL || strlen(code) != WIFI_COUNTRY_CODE_MAXLEN) {
        LOGE("SetCountryCode code is invalid");
        return WIFI_HAL_INVALID_PARAM;
    }
    LOGD("SetCountryCode() code: %{public}s", code);
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }
    if (hostapdHalDevice->setCountryCode(code) != 0) {
        LOGE("SetCountryCode failed!");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetHostapdConfig(HostapdConfig *config)
{
    if (config == NULL) {
        LOGE("SetHostapdConfig config is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("SetHostapdConfig()");
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }
    int ret = hostapdHalDevice->setApInfo(config);
    if (ret != 0) {
        LOGE("SetApInfo failed!");
        return WIFI_HAL_FAILED;
    }
    ret = hostapdHalDevice->reloadApConfigInfo();
    if (ret != 0) {
        LOGE("ReloadApConfigInfo failed!");
        return WIFI_HAL_FAILED;
    }
    ret = hostapdHalDevice->disableAp();
    if (ret != 0) {
        LOGE("DisableAp failed!");
        return WIFI_HAL_FAILED;
    }
    ret = hostapdHalDevice->enableAp();
    if (ret != 0) {
        LOGE("EnableAp failed!");
        return WIFI_HAL_FAILED;
    }
    LOGD("SetHostapdConfig successfully!");
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetMacFilter(const unsigned char *mac, int lenMac)
{
    if (mac == NULL) {
        LOGE("SetMacFilter is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("SetMacFilter:mac: %{private}s, len_mac: %{public}d", (const char *)mac, lenMac);
    if (strlen((const char *)mac) != WIFI_MAC_LENGTH || lenMac != WIFI_MAC_LENGTH) {
        LOGE("Mac size not correct! mac len %{public}zu, request lenMac %{public}d", strlen((const char *)mac), lenMac);
        return WIFI_HAL_FAILED;
    }
    if (CheckMacIsValid((const char *)mac) != 0) {
        return WIFI_HAL_INPUT_MAC_INVALID;
    }
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }
    if (hostapdHalDevice->addBlocklist((const char *)mac) != 0) {
        LOGE("AddBlocklist failed!");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DelMacFilter(const unsigned char *mac, int lenMac)
{
    if (mac == NULL) {
        LOGE("DelMacFilter is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("DelMacFilter:mac: %{private}s, len_mac: %{public}d", (const char *)mac, lenMac);
    if (strlen((const char *)mac) != WIFI_MAC_LENGTH || lenMac != WIFI_MAC_LENGTH) {
        LOGE("Mac size not correct! mac len %{public}zu, request lenMac %{public}d", strlen((const char *)mac), lenMac);
        return WIFI_HAL_FAILED;
    }
    if (CheckMacIsValid((const char *)mac) != 0) {
        return WIFI_HAL_INPUT_MAC_INVALID;
    }
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }
    if (hostapdHalDevice->delBlocklist((const char *)mac) != 0) {
        LOGE("DelBlocklist failed!");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DisassociateSta(const unsigned char *mac, int lenMac)
{
    if (mac == NULL) {
        LOGE("DisassociateSta is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("DisassociateSta:mac: %{private}s, len_mac: %{public}d", (const char *)mac, lenMac);
    if (strlen((const char *)mac) != WIFI_MAC_LENGTH || lenMac != WIFI_MAC_LENGTH) {
        LOGE("Mac size not correct! mac len %{public}zu, request lenMac %{public}d", strlen((const char *)mac), lenMac);
        return WIFI_HAL_FAILED;
    }
    if (CheckMacIsValid((const char *)mac) != 0) {
        return WIFI_HAL_INPUT_MAC_INVALID;
    }
    WifiHostapdHalDevice *hostapdHalDevice = GetWifiHostapdDev();
    if (hostapdHalDevice == NULL) {
        return WIFI_HAL_HOSTAPD_NOT_INIT;
    }
    if (hostapdHalDevice->disConnectedDev((const char *)mac) != 0) {
        LOGE("DisConnectedDev failed!");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

static int32_t ConvertToNl80211Band(int32_t band)
{
    return (band > 0 && band <= NUMS_BAND) ? (band - 1) : band;
}

WifiErrorNo GetValidFrequenciesForBand(int32_t band, int *frequencies, int32_t *size)
{
    int32_t ret;
    uint32_t count = 0;
    struct IWiFi *wifi = NULL;
    struct IWiFiAp *apFeature = NULL;

    if (frequencies == NULL || size == NULL) {
        LOGE("GetValidFrequenciesForBand  frequencies or size is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("GetValidFrequenciesForBand");

    ret = WifiConstruct(&wifi);
    if (ret != 0 || wifi == NULL) {
        LOGE("%{public}s WifiConstruct failed", __func__);
        return WIFI_HAL_FAILED;
    }

    ret = wifi->start(wifi);
    if (ret != 0) {
        (void)WifiDestruct(&wifi);
        LOGE("%{public}s start failed", __func__);
        return WIFI_HAL_FAILED;
    }

    ret = wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    if (ret != 0 || apFeature == NULL) {
        (void)wifi->stop(wifi);
        (void)WifiDestruct(&wifi);
        LOGE("%{public}s createFeature failed", __func__);
        return WIFI_HAL_FAILED;
    }

    ret = apFeature->baseFeature.getValidFreqsWithBand((struct IWiFiBaseFeature *)apFeature,
        ConvertToNl80211Band(band), frequencies, *size, &count);
    *size = count;

    (void)wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    (void)wifi->stop(wifi);
    (void)WifiDestruct(&wifi);
    if (ret != 0) {
        LOGE("%{public}s failed", __func__);
    }

    return (ret == 0) ? WIFI_HAL_SUCCESS : WIFI_HAL_FAILED;
}
