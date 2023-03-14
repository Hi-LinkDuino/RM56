/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ap.h"
#include <osal_mem.h>
#include <securec.h>
#include "message/message_router.h"
#include "message/sidecar.h"
#include "wifi_base.h"
#include "wifi_mac80211_ops.h"
#include "hdf_wlan_services.h"
#include "hdf_wlan_utils.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

#ifdef __cplusplus
extern "C" {
#endif
static uint32_t ChangeBeacon(struct NetDevice *netDev, WifiApSetting *apSettings)
{
    struct WlanBeaconConf beaconConf = { 0 };
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    if (netDev == NULL || apSettings == NULL) {
        HDF_LOGE("%s: parameter null", __func__);
        return HDF_FAILURE;
    }
    beaconConf.interval = apSettings->beaconInterval;
    beaconConf.DTIMPeriod = apSettings->dtimPeriod;
    beaconConf.hiddenSSID = (apSettings->hiddenSsid == 1);
    beaconConf.headIEs = apSettings->beaconData.head;
    beaconConf.headIEsLength = apSettings->beaconData.headLen;
    beaconConf.tailIEs = apSettings->beaconData.tail;
    beaconConf.tailIEsLength = apSettings->beaconData.tailLen;
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, ConfigBeacon);
    return chipDriver->apOps->ConfigBeacon(netDev, &beaconConf);
}

static int32_t StartAp(struct NetDevice *netDev, WifiApSetting *apSettings)
{
    struct WlanAPConf apConf = { 0 };
    int32_t ret;
    struct HdfChipDriver *chipDriver = NULL;
    errno_t err;
    HDF_LOGI("%s:starting ap...", __func__);
    chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    err = memcpy_s(apConf.ssidConf.ssid, IEEE80211_MAX_SSID_LEN, apSettings->ssid, apSettings->ssidLen);
    if (err != EOK) {
        HDF_LOGE("%s: memcpy_s failed!ret=%d", __func__, err);
        return HDF_FAILURE;
    }
    apConf.ssidConf.ssidLen = apSettings->ssidLen;
    apConf.centerFreq1 = apSettings->freqParams.centerFreq1;
    apConf.channel = apSettings->freqParams.channel;
    apConf.width = apSettings->freqParams.bandwidth;
    apConf.band = apSettings->freqParams.band;
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, ConfigAp);
    ret = chipDriver->apOps->ConfigAp(netDev, &apConf);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:ConfigAp failed!ret=%d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = ChangeBeacon(netDev, apSettings);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:ChangeBeacon failed!ret=%d", __func__, ret);
        return HDF_FAILURE;
    }
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, StartAp);
    ret = chipDriver->apOps->StartAp(netDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:StartAp failed!ret=%d", __func__, ret);
        return HDF_FAILURE;
    }
    return NetIfSetStatus(netDev, NETIF_UP);
}

static uint32_t StopAp(struct NetDevice *netDev)
{
    uint32_t ret;
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, StopAp);
    ret = chipDriver->apOps->StopAp(netDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("StopAp:failed, error[%d]", ret);
        return ret;
    }
    return NetIfSetStatus(netDev, NETIF_DOWN);
}

static uint32_t DelStation(struct NetDevice *netDev, struct StationDelParameters *params)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, DelStation);
    return chipDriver->apOps->DelStation(netDev, params->mac);
}

static uint32_t SetCountryCode(struct NetDevice *netDev, const char *code, uint32_t len)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, SetCountryCode);
    return chipDriver->apOps->SetCountryCode(netDev, code, len);
}

static uint32_t GetAssociatedStasCount(struct NetDevice *netDev, uint32_t *num)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, GetAssociatedStasCount);
    return chipDriver->apOps->GetAssociatedStasCount(netDev, num);
}

static uint32_t GetAssociatedStasInfo(struct NetDevice *netDev, WifiStaInfo *staInfo, uint32_t num)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }
    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->apOps, GetAssociatedStasInfo);
    return chipDriver->apOps->GetAssociatedStasInfo(netDev, staInfo, num);
}

static int32_t WifiFillApSettingsParams(struct HdfSBuf *reqData,  WifiApSetting *apSettings)
{
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.mode))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "mode");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s:apSettings->freqParams.mode=%d", __func__, apSettings->freqParams.mode);
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.freq))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "freq");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.channel))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "channel");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.htEnabled))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "htEnabled");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.secChannelOffset))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "secChannelOffset");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.vhtEnabled))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "vhtEnabled");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.centerFreq1))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "centerFreq1");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.centerFreq2))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "centerFreq2");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->freqParams.bandwidth))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "bandwidth");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint8(reqData, &(apSettings->freqParams.band))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "band");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t WifiCmdSetApInner(struct HdfSBuf *reqData,  WifiApSetting *apSettings)
{
    if (WifiFillApSettingsParams(reqData, apSettings) != HDF_SUCCESS) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "apSettings");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->beaconInterval))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "beaconInterval");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(apSettings->dtimPeriod))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "dtimPeriod");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint8(reqData, &(apSettings->hiddenSsid))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "hiddenSsid");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint8(reqData, &(apSettings->authType))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "authType");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings->beaconData.head), &(apSettings->beaconData.headLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "head");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings->beaconData.tail), &(apSettings->beaconData.tailLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "tail");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings->ssid), &(apSettings->ssidLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ssid");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings->meshSsid), &(apSettings->meshSsidLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "meshSsid");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t WifiCmdSetAp(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    WifiApSetting apSettings;
    const char *ifName = NULL;
    struct NetDevice *netdev = NULL;
    int32_t ret = 0;
    (void)context;
    (void)rspData;
    if (reqData == NULL) {
        HDF_LOGE("%s: reqData is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }

    if (WifiCmdSetApInner(reqData, &apSettings) != HDF_SUCCESS) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "apSettings");
        return HDF_FAILURE;
    }

    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s:%s starting AP ...", __func__, ifName);
    ret = StartAp(netdev, &apSettings);
    return ret;
}

static int32_t WifiCmdStopAp(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    const char *ifName = NULL;
    struct NetDevice *netdev = NULL;
    (void)context;
    (void)rspData;
    if (reqData == NULL) {
        HDF_LOGE("%s: reqData is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s:%s stopping AP ...", __func__, ifName);
    return StopAp(netdev);
}

static int32_t WifiCmdChangeBeacon(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    struct NetDevice *netdev = NULL;
    WifiApSetting apSettings;
    const char *ifName = NULL;
    (void)context;
    (void)rspData;
    if (reqData == NULL) {
        HDF_LOGE("%s: reqData is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings.beaconData.head), &(apSettings.beaconData.headLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "head");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings.beaconData.tail), &(apSettings.beaconData.tailLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "tail");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings.ssid), &(apSettings.ssidLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ssid");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&(apSettings.meshSsid), &(apSettings.meshSsidLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "meshSsid");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    return ChangeBeacon(netdev, &apSettings);
}

static int32_t WifiCmdStaRemove(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    struct NetDevice *netdev = NULL;
    struct StationDelParameters params;
    const char *ifName = NULL;
    uint32_t dataSize = 0;
    int32_t ret;
    (void)context;
    (void)rspData;
    if (reqData == NULL) {
        HDF_LOGE("%s: reqData is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    params.subtype = 0;
    params.reasonCode = 0;
    if (!HdfSbufReadBuffer(reqData, (const void **)&params.mac, &dataSize) || dataSize != ETH_ADDR_LEN) {
        HDF_LOGE("%s: %s!ParamName=%s,readSize=%u", __func__, ERROR_DESC_READ_REQ_FAILED, "mac", dataSize);
        return HDF_FAILURE;
    }
    ret = DelStation(netdev, &params);
    HDF_LOGI("%s:del station XX:XX:XX:XX:XX:%02X ret=%d", __func__, params.mac[ETH_ADDR_LEN - 1], ret);
    return ret;
}

static int32_t GetAssociatedStas(struct NetDevice *netdev, uint32_t num, struct HdfSBuf *rspData)
{
    int32_t ret;
    WifiStaInfo *staInfo = NULL;
    staInfo = (WifiStaInfo *)OsalMemCalloc(sizeof(WifiStaInfo) * num);
    if (staInfo == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed!", __func__);
        return HDF_FAILURE;
    }
    ret = GetAssociatedStasInfo(netdev, staInfo, num);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to get sta info,%d", __func__, ret);
        OsalMemFree(staInfo);
        return ret;
    }
    if (!HdfSbufWriteBuffer(rspData, staInfo, sizeof(WifiStaInfo) * num)) {
        HDF_LOGE("%s: %s!", __func__, ERROR_DESC_WRITE_RSP_FAILED);
        OsalMemFree(staInfo);
        return HDF_FAILURE;
    }
    OsalMemFree(staInfo);
    return HDF_SUCCESS;
}

static int32_t WifiCmdGetAssociatedStas(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    uint32_t num;
    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    ret = GetAssociatedStasCount(netdev, &num);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to get user num,ret=%d", __func__, ret);
        return ret;
    }
    if (!HdfSbufWriteUint32(rspData, num)) {
        HDF_LOGE("%s: %s!", __func__, ERROR_DESC_WRITE_RSP_FAILED);
        return HDF_FAILURE;
    }
    if (num == 0) {
        return HDF_SUCCESS;
    }
    ret = GetAssociatedStas(netdev, num, rspData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to GetAssociatedStas,%d", __func__, ret);
    }
    return ret;
}

static int32_t WifiCmdSetCountryCode(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    const char *code = NULL;
    uint32_t replayDataSize;
    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)&code, &replayDataSize)) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "code");
        return HDF_FAILURE;
    }
    ret = SetCountryCode(netdev, code, replayDataSize);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to set country code,%d", __func__, ret);
    }
    return ret;
}

static struct MessageDef g_wifiApFeatureCmds[] = {
    DUEMessage(CMD_AP_START, WifiCmdSetAp, 0),
    DUEMessage(CMD_AP_STOP, WifiCmdStopAp, 0),
    DUEMessage(CMD_AP_CHANGE_BEACON, WifiCmdChangeBeacon, 0),
    DUEMessage(CMD_AP_DEL_STATION, WifiCmdStaRemove, 0),
    DUEMessage(CMD_AP_GET_ASSOC_STA, WifiCmdGetAssociatedStas, 0),
    DUEMessage(CMD_AP_SET_COUNTRY_CODE, WifiCmdSetCountryCode, 0)
};
ServiceDefine(APService, AP_SERVICE_ID, g_wifiApFeatureCmds);

static Service *g_apService = NULL;

int32_t ApInit(struct WifiFeature *feature)
{
    (void)feature;
    if (g_apService == NULL) {
        ServiceCfg cfg = {
            .dispatcherId = DEFAULT_DISPATCHER_ID
        };
        g_apService = CreateService(APService, &cfg);
        if (g_apService == NULL) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t ApDeinit(struct WifiFeature *feature)
{
    (void)feature;
    if (g_apService != NULL && g_apService->Destroy != NULL) {
        g_apService->Destroy(g_apService);
        g_apService = NULL;
    }
    return HDF_SUCCESS;
}

struct WifiFeature g_apFeature = {
    .name = "ap",
    .init = ApInit,
    .deInit = ApDeinit
};

struct WifiFeature *GetWifiApFeature(void)
{
    return &g_apFeature;
}

#ifdef __cplusplus
}
#endif
