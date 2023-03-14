/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "../wifi_common_cmd.h"
#include "hdf_io_service.h"
#include "hdf_sbuf.h"
#include "hilog/log.h"
#include "securec.h"
#include "wifi_driver_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static void WifiEventNewStaProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiNewStaInfo staInfo;
    uint32_t len = 0;

    if (!HdfSbufReadInt32(reqData, &staInfo.reassoc)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get reassoc", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&staInfo.ie), &staInfo.ieLen)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get ie", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&staInfo.macAddr), &len) || (len != ETH_ADDR_LEN)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get macAddr", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &staInfo);
}

static void WifiEventDelStaProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    uint8_t *addr = NULL;
    uint32_t len = 0;

    if ((!HdfSbufReadBuffer(reqData, (const void **)(&addr), &len)) || (len != ETH_ADDR_LEN)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get macAddr", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, addr);
}

static void WifiEventRxMgmtProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiRxMgmt rxMgmt;

    if (!HdfSbufReadInt32(reqData, &rxMgmt.freq)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get freq", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadInt32(reqData, &rxMgmt.sigMbm)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get sigMbm", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&rxMgmt.buf), &rxMgmt.len)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get buf", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &rxMgmt);
}

static void WifiEventTxStatusProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiTxStatus txStatus;

    if (!HdfSbufReadUint8(reqData, &txStatus.ack)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get ack", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&txStatus.buf), &txStatus.len)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get buf", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &txStatus);
}

static void WifiEventScanDoneProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    uint32_t status;

    if (!HdfSbufReadUint32(reqData, &status)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get status", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &status);
}

static void WifiEventScanResultProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiScanResult scanResult = { 0 };
    uint32_t len = 0;

    if (!HdfSbufReadInt16(reqData, &(scanResult.beaconInt))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get beaconInt", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadInt16(reqData, &(scanResult.caps))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get caps", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadInt32(reqData, &(scanResult.level))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get level", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadInt32(reqData, &(scanResult.freq))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get freq", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadInt32(reqData, &(scanResult.flags))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get flags", __FUNCTION__);
        return;
    }
    if ((!HdfSbufReadBuffer(reqData, (const void **)(&(scanResult.bssid)), &len)) || len != ETH_ADDR_LEN) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get bssid", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&(scanResult.ie)), &(scanResult.ieLen))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get ie", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&(scanResult.beaconIe)), &(scanResult.beaconIeLen))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get beaconIe", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &scanResult);
}

static void WifiEventConnectResultProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiConnectResult result;
    uint32_t len = 0;

    if (!HdfSbufReadUint16(reqData, &(result.status))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get status", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadUint16(reqData, &(result.freq))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get freq", __FUNCTION__);
        return;
    }
    if ((!HdfSbufReadBuffer(reqData, (const void **)(&(result.bssid)), &len)) || len != ETH_ADDR_LEN) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get bssid", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&(result.reqIe)), &(result.reqIeLen))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get reqIe", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&(result.respIe)), &(result.respIeLen))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get respIe", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &result);
}

static void WifiEventDisconnectProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiDisconnect result;

    if (!HdfSbufReadUint16(reqData, &(result.reason))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get reason", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadBuffer(reqData, (const void **)(&(result.ie)), &(result.ieLen))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get bssid", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &result);
}

static void WifiDriverEventEapolRecvProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiEventReport(ifName, event, reqData);
}

static void WifiEventResetDriverProcess(const char *ifName, int32_t event, struct HdfSBuf *reqData)
{
    unsigned char chipId;
    int resetStatus;

    if (!HdfSbufReadInt32(reqData, &resetStatus)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get resetStatus, line: %d", __FUNCTION__, __LINE__);
        return;
    }
    if (!HdfSbufReadUint8(reqData, &chipId)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get chipId, line: %d", __FUNCTION__, __LINE__);
        return;
    }
    WifiEventReport(ifName, event, &resetStatus);
}

static void WifiDriverEventRemainOnChannelProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiOnChannel result = {0};
    if (!HdfSbufReadUint32(reqData, &(result.freq))) {
        HILOG_ERROR(LOG_DOMAIN, "%s failed to get frequency.", __FUNCTION__);
        return;
    }
    if (!HdfSbufReadUint32(reqData, &(result.duration))) {
        HILOG_ERROR(LOG_DOMAIN, "%s failed to get duration.", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &result);
}

static void WifiDriverEventCancelRemainOnChannelProcess(const char *ifName, uint32_t event, struct HdfSBuf *reqData)
{
    WifiOnChannel result = {0};
    if (!HdfSbufReadUint32(reqData, &(result.freq))) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get freq", __FUNCTION__);
        return;
    }
    WifiEventReport(ifName, event, &result);
}

int OnWiFiEvents(struct HdfDevEventlistener *listener,
    struct HdfIoService *service, uint32_t eventId, struct HdfSBuf *data)
{
    (void)listener;
    (void)service;
    (void)data;

    if (data == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: params is NULL, line: %d", __FUNCTION__, __LINE__);
        return RET_CODE_FAILURE;
    }
    const char *ifName = HdfSbufReadString(data);
    if (ifName == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s fail to get ifname", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    HILOG_INFO(LOG_DOMAIN, "%s: WifiDriverEventProcess event=%d", __FUNCTION__, eventId);
    switch (eventId) {
        case WIFI_EVENT_NEW_STA:
            WifiEventNewStaProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_DEL_STA:
            WifiEventDelStaProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_RX_MGMT:
            WifiEventRxMgmtProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_TX_STATUS:
            WifiEventTxStatusProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_SCAN_DONE:
            WifiEventScanDoneProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_SCAN_RESULT:
            WifiEventScanResultProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_CONNECT_RESULT:
            WifiEventConnectResultProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_DISCONNECT:
            WifiEventDisconnectProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_EAPOL_RECV:
            WifiDriverEventEapolRecvProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_RESET_DRIVER:
            WifiEventResetDriverProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_REMAIN_ON_CHANNEL:
            WifiDriverEventRemainOnChannelProcess(ifName, eventId, data);
            break;
        case WIFI_EVENT_CANCEL_REMAIN_ON_CHANNEL:
            WifiDriverEventCancelRemainOnChannelProcess(ifName, eventId, data);
            break;
        default:
            break;
    }
    return RET_CODE_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
