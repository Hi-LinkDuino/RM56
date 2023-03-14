/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wifi_event.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "hdf_slist.h"
#include "hdf_wifi_product.h"
#include "osal_timer.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int32_t HdfWifiEventNewSta(const struct NetDevice *netDev, const uint8_t *macAddr, uint8_t addrLen,
    const struct StationInfo *info)
{
    struct HdfSBuf *data = NULL;
    const uint32_t reassoc = 0;
    int32_t ret;

    if (netDev == NULL || macAddr == NULL || info == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (info->assocReqIesLen == 0) {
        HDF_LOGE("%s assocReqIesLen is 0", __func__);
        return HDF_FAILURE;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint32(data, reassoc) ||
        !HdfSbufWriteBuffer(data, info->assocReqIes, info->assocReqIesLen) ||
        !HdfSbufWriteBuffer(data, macAddr, addrLen)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s netDev->name=%s, MAC=XX:XX:XX:XX:XX:%02X", __func__, netDev->name,
        (addrLen > 0) ? macAddr[addrLen - 1] : -1);

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_NEW_STA, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventDelSta(struct NetDevice *netDev, const uint8_t *macAddr, uint8_t addrLen)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if ((netDev == NULL) || (macAddr == NULL)) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteBuffer(data, macAddr, addrLen)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_DEL_STA, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventInformBssFrame(const struct NetDevice *netDev,
    const struct WlanChannel *channel, const struct ScannedBssInfo *bssInfo)
{
    struct HdfSBuf *data = NULL;
    uint32_t ieLen;
    uint32_t beaconLen;
    int32_t ret;

    if ((netDev == NULL) || (channel == NULL) || (bssInfo == NULL) || (bssInfo->mgmt == NULL)) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    ieLen = bssInfo->mgmtLen - (uint32_t)OFFSET_OF(struct Ieee80211Mgmt, u.probeResp.variable);
    beaconLen = bssInfo->mgmtLen - (uint32_t)OFFSET_OF(struct Ieee80211Mgmt, u.beacon.variable);
    if (!HdfSbufWriteString(data, netDev->name) ||
        !HdfSbufWriteUint16(data, (int16_t)bssInfo->mgmt->u.probeResp.beaconInt) ||
        !HdfSbufWriteUint16(data, (int16_t)bssInfo->mgmt->u.probeResp.capabInfo) ||
        !HdfSbufWriteUint32(data, bssInfo->signal) || !HdfSbufWriteUint32(data, channel->centerFreq) ||
        !HdfSbufWriteUint32(data, (int32_t)channel->flags) ||
        !HdfSbufWriteBuffer(data, bssInfo->mgmt->bssid, ETH_ADDR_LEN) ||
        !HdfSbufWriteBuffer(data, bssInfo->mgmt->u.probeResp.variable, ieLen) ||
        !HdfSbufWriteBuffer(data, bssInfo->mgmt->u.beacon.variable, beaconLen)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_SCAN_RESULT, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventScanDone(const struct NetDevice *netDev, WifiScanStatus status)
{
    uint32_t code = status;
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint32(data, code)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_SCAN_DONE, data);
    HdfSbufRecycle(data);
    return ret;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define DHCP_CHECK_CNT 30
#define DHCP_CHECK_TIME 1000
uint32_t g_ipCheckOut = 0;
OSAL_DECLARE_TIMER(g_dhcpTimer);

void HdfDhcpResultCheck(uintptr_t para)
{
    int32_t ret;
    struct NetDevice *dev = (struct NetDevice *)para;

    if (dev == NULL) {
        return;
    }
    ret = NetIfDhcpIsBound(dev);
    if (ret == 0) {
        OsalTimerDelete(&g_dhcpTimer);
        return;
    }

    if (g_ipCheckOut++ > DHCP_CHECK_CNT) {
        OsalTimerDelete(&g_dhcpTimer);
        return;
    }
}

static void HdfStartDhcpClient(const struct NetDevice *netDev)
{
    int32_t ret;

    /* start dhcp client */
    (void)NetIfDhcpStop((struct NetDevice *)netDev);
    ret = NetIfDhcpStart((struct NetDevice *)netDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdfWifiEventConnectResult::start dhcp client failed");
    }

    if (g_dhcpTimer.realTimer != NULL) {
        OsalTimerDelete(&g_dhcpTimer);
    }

    g_ipCheckOut = 0;
    OsalTimerCreate(&g_dhcpTimer, DHCP_CHECK_TIME, HdfDhcpResultCheck, (uintptr_t)netDev);
    OsalTimerStartLoop(&g_dhcpTimer);
}
#endif

int32_t HdfWifiEventConnectResult(const struct NetDevice *netDev, const struct ConnetResult *result)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL || result == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint16(data, result->connectStatus) ||
        !HdfSbufWriteUint16(data, result->freq) || !HdfSbufWriteBuffer(data, result->bssid, ETH_ADDR_LEN) ||
        !HdfSbufWriteBuffer(data, result->reqIe, result->reqIeLen) ||
        !HdfSbufWriteBuffer(data, result->rspIe, result->rspIeLen)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_CONNECT_RESULT, data);
    HdfSbufRecycle(data);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (result->connectStatus == WIFI_HMAC_MGMT_SUCCESS) {
        HdfStartDhcpClient(netDev);
    }
#endif

    return ret;
}

int32_t HdfWifiEventDisconnected(const struct NetDevice *netDev, uint16_t reason, const uint8_t *ie, uint32_t len)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint16(data, reason) ||
        !HdfSbufWriteBuffer(data, ie, len)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_DISCONNECT, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventMgmtTxStatus(const struct NetDevice *netDev, const uint8_t *buf, size_t len, uint8_t ack)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint8(data, ack) ||
        !HdfSbufWriteBuffer(data, buf, len)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_TX_STATUS, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventCsaChannelSwitch(const struct NetDevice *netDev, int32_t freq)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint32(data, freq)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_CHANNEL_SWITCH, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventTimeoutDisconnected(const struct NetDevice *netDev)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, netDev->name)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_TIMEOUT_DISCONN, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventRxMgmt(const struct NetDevice *netDev, int32_t freq, int32_t sigMbm, const uint8_t *buf, size_t len)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint32(data, freq) ||
        !HdfSbufWriteUint32(data, sigMbm) || !HdfSbufWriteBuffer(data, buf, len)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_RX_MGMT, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventEapolRecv(const char *name, void *context)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    (void)context;
    if (name == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, name)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_EAPOL_RECV, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventResetResult(const uint8_t chipId, int32_t resetStatus, const char *ifName)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: Serialize failed!", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, resetStatus)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint8(data, chipId)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_RESET_DRIVER, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventRemainOnChannel(const struct NetDevice *netDev, uint32_t freq, uint32_t duration)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint32(data, freq)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, duration)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_REMAIN_ON_CHANNEL, data);
    HdfSbufRecycle(data);
    return ret;
}

int32_t HdfWifiEventCancelRemainOnChannel(const struct NetDevice *netDev, uint32_t freq)
{
    struct HdfSBuf *data = NULL;
    int32_t ret;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, netDev->name) || !HdfSbufWriteUint32(data, freq)) {
        HDF_LOGE("%s sbuf write failed", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    ret = HdfWlanSendBroadcastEvent(WIFI_WPA_EVENT_CANCEL_REMAIN_ON_CHANNEL, data);
    HdfSbufRecycle(data);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
