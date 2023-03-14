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

#include <stdlib.h>
#include "hdf_io_service.h"
#include "hdf_sbuf.h"
#include "hilog/log.h"
#include "securec.h"
#include "../wifi_common_cmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

const char *DRIVER_SERVICE_NAME = "hdfwifi";

struct HdfIoService *g_wifiService = NULL;
static struct HdfDevEventlistener g_wifiDevEventListener;
static bool g_isHasRegisterListener = false;

static int32_t SendCmdSync(const uint32_t cmd, struct HdfSBuf *reqData, struct HdfSBuf *respData)
{
    if (reqData == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (g_wifiService == NULL || g_wifiService->dispatcher == NULL ||
        g_wifiService->dispatcher->Dispatch == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s:bad remote service found!", __FUNCTION__);
        return RET_CODE_MISUSE;
    }
    int32_t ret = g_wifiService->dispatcher->Dispatch(&g_wifiService->object, cmd, reqData, respData);
    HILOG_INFO(LOG_DOMAIN, "%s: cmd=%u, ret=%d", __FUNCTION__, cmd, ret);
    return ret;
}

static int32_t ParserNetworkInfo(struct HdfSBuf *reply, struct NetworkInfoResult *result)
{
    uint32_t i;
    const char *ifName = NULL;
    uint8_t *mode = NULL;
    uint32_t replayDataSize;

    if (!HdfSbufReadUint32(reply, &result->nums)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get networkNum failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (result->nums > MAX_IFACE_NUM) {
        result->nums = MAX_IFACE_NUM;
    }
    for (i = 0; i < result->nums; i++) {
        ifName = HdfSbufReadString(reply);
        if (ifName == NULL) {
            HILOG_ERROR(LOG_DOMAIN, "%s: get ifName failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
        if (!HdfSbufReadBuffer(reply, (const void **)&mode, &replayDataSize) ||
            mode == NULL || replayDataSize != WIFI_IFTYPE_MAX) {
            HILOG_ERROR(LOG_DOMAIN, "%s: get mode failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
        if (strncpy_s(result->infos[i].name, IFNAMSIZ, ifName, strlen(ifName)) != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memcpy_s name failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
        if (memcpy_s(result->infos[i].supportMode, WIFI_IFTYPE_MAX, mode, replayDataSize) != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memcpy_s supportMode failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
    }
    return RET_CODE_SUCCESS;
}

static int32_t ParserDeviceMacAddr(struct HdfSBuf *reply,
    uint8_t *mac, uint8_t len)
{
    uint8_t isEfuseSavedMac;
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;

    if (!HdfSbufReadUint8(reply, &isEfuseSavedMac)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadUint8 failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (!isEfuseSavedMac) {
        HILOG_ERROR(LOG_DOMAIN, "%s: not support to get device mac addr", __FUNCTION__);
        return RET_CODE_NOT_SUPPORT;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData),
        &replayDataSize) || replayDataSize != len) {
        HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadBuffer failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (memcpy_s(mac, len, replayData, replayDataSize) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    return RET_CODE_SUCCESS;
}

static int32_t ParserFreqInfo(struct HdfSBuf *reply, struct FreqInfoResult *result, uint32_t size)
{
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;

    if (result == NULL || result->freqs == NULL || result->txPower == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s:  Invalid input parameter", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(reply, &result->nums)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: read num failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (result->nums > size) {
        HILOG_ERROR(LOG_DOMAIN, "%s: num valid", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: read freqs failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (memcpy_s(result->freqs, size * sizeof(int32_t), replayData, replayDataSize) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    return RET_CODE_SUCCESS;
}

static int32_t ParserAssociatedStas(struct HdfSBuf *reply,
    struct AssocStaInfoResult *result)
{
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;

    if (!HdfSbufReadUint32(reply, &result->num)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: read num failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (result->num > MAX_ASSOC_STA_NUM) {
        HILOG_ERROR(LOG_DOMAIN, "%s: num invalid", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (result->num != 0) {
        if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize) ||
            replayDataSize > sizeof(result->infos)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: read AssocStaInfo failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
        if (memcpy_s(result->infos, sizeof(result->infos), replayData, replayDataSize) != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
    }
    return RET_CODE_SUCCESS;
}

static int32_t WifiMsgRegisterEventListener(struct HdfDevEventlistener *listener)
{
    if (g_wifiService == NULL || listener == NULL) {
        return RET_CODE_FAILURE;
    }
    if (HdfDeviceRegisterEventListener(g_wifiService, listener) != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to register event listener, line: %d", __FUNCTION__, __LINE__);
        return RET_CODE_FAILURE;
    }
    g_isHasRegisterListener = true;
    return RET_CODE_SUCCESS;
}

static void WifiMsgUnregisterEventListener(struct HdfDevEventlistener *listener)
{
    if (listener == NULL) {
        return;
    }
    if (HdfDeviceUnregisterEventListener(g_wifiService, listener)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to unregister event listener, line: %d", __FUNCTION__, __LINE__);
    }
    g_isHasRegisterListener = false;
}

extern int OnWiFiEvents(struct HdfDevEventlistener *listener,
    struct HdfIoService *service, uint32_t eventId, struct HdfSBuf *data);

int32_t WifiDriverClientInit(void)
{
    int32_t ret;
    if (g_wifiService == NULL) {
        g_wifiService = HdfIoServiceBind(DRIVER_SERVICE_NAME);
    }
    if (g_wifiService == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to get remote service!", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    g_wifiDevEventListener.onReceive  = OnWiFiEvents;
    if (g_isHasRegisterListener) {
        HILOG_INFO(LOG_DOMAIN, "%s:has register listener!", __FUNCTION__);
        return RET_CODE_SUCCESS;
    }
    ret = WifiMsgRegisterEventListener(&g_wifiDevEventListener);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: register event listener faild, line: %d", __FUNCTION__, __LINE__);
    }
    return ret;
}

void WifiDriverClientDeinit(void)
{
    if (g_wifiService == NULL) {
        return;
    }
    WifiMsgUnregisterEventListener(&g_wifiDevEventListener);
    if (HdfIoserviceGetListenerCount(g_wifiService) != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: the current EventListener is not empty. cancel the listener registration first.",
            __FUNCTION__);
        return;
    }
    HdfIoServiceRecycle(g_wifiService);
    g_wifiService = NULL;
}

static int32_t HdfSbufObtainDefault(struct HdfSBuf **data, struct HdfSBuf **reply)
{
    *data = HdfSbufObtainDefaultSize();
    if (*data == NULL) {
        return RET_CODE_FAILURE;
    }
    *reply = HdfSbufObtainDefaultSize();
    if (*reply == NULL) {
        HdfSbufRecycle(*data);
        return RET_CODE_FAILURE;
    }
    return RET_CODE_SUCCESS;
}

int32_t GetUsableNetworkInfo(struct NetworkInfoResult *result)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (result == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    ret = SendCmdSync(WIFI_HAL_CMD_GET_NETWORK_INFO, data, reply);
    if (ret == RET_CODE_SUCCESS) {
        ret = ParserNetworkInfo(reply, result);
    } else {
        ret = RET_CODE_FAILURE;
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t IsSupportCombo(uint8_t *isSupportCombo)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (isSupportCombo == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    ret = SendCmdSync(WIFI_HAL_CMD_IS_SUPPORT_COMBO, data, reply);
    do {
        if (ret != RET_CODE_SUCCESS) {
            break;
        }
        if (!HdfSbufReadUint8(reply, isSupportCombo)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadUint8 failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
        } else {
            ret = RET_CODE_SUCCESS;
        }
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t GetComboInfo(uint64_t *comboInfo, uint32_t size)
{
    int32_t ret;
    uint8_t isComboValid;
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (comboInfo == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    ret = SendCmdSync(WIFI_HAL_CMD_GET_SUPPORT_COMBO, data, reply);
    do {
        if (ret != RET_CODE_SUCCESS) {
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufReadUint8(reply, &isComboValid)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: read combo valid flag failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!isComboValid) {
            HILOG_ERROR(LOG_DOMAIN, "%s: not support combo mode", __FUNCTION__);
            ret = RET_CODE_NOT_SUPPORT;
            break;
        }
        if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadBuffer failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (memcpy_s(comboInfo, size, replayData, replayDataSize) != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t SetMacAddr(const char *ifName, unsigned char *mac, uint8_t len)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || mac == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteBuffer(data, mac, len)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write mac failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_SET_MAC_ADDR, data, reply);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t GetDevMacAddr(const char *ifName, int32_t type, uint8_t *mac, uint8_t len)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || mac == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteInt32(data, type)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write type failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_GET_DEV_MAC_ADDR, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = ParserDeviceMacAddr(reply, mac, len);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t GetValidFreqByBand(const char *ifName, int32_t band, struct FreqInfoResult *result, uint32_t size)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || result == NULL || band >= IEEE80211_NUM_BANDS) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteInt32(data, band)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write band failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_GET_VALID_FREQ, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = ParserFreqInfo(reply, result, size);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t SetTxPower(const char *ifName, int32_t power)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteInt32(data, power)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufWriteInt32 failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_SET_TX_POWER, data, reply);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t GetAssociatedStas(const char *ifName, struct AssocStaInfoResult *result)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || result == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_GET_ASSOC_STA, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = ParserAssociatedStas(reply, result);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t WifiSetCountryCode(const char *ifName, const char *code, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || code == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteBuffer(data, code, len)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write code failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_SET_COUNTRY_CODE, data, reply);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t SetScanMacAddr(const char *ifName, uint8_t *scanMac, uint8_t len)
{
    int32_t ret;
    uint8_t isFuncValid;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || scanMac == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write ifName failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteBuffer(data, scanMac, len)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: write scan mac failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_SET_SCAN_MAC_ADDR, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            break;
        }
        if (!HdfSbufReadUint8(reply, &isFuncValid)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: read valid flag failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!isFuncValid) {
            HILOG_ERROR(LOG_DOMAIN, "%s: not support to set scan mac addr", __FUNCTION__);
            ret = RET_CODE_NOT_SUPPORT;
            break;
        }
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t AcquireChipId(const char *ifName, uint8_t *chipId)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || chipId == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufWriteString failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_GET_CHIPID, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            break;
        }
        if (!HdfSbufReadUint8(reply, chipId)) {
            HILOG_ERROR(LOG_DOMAIN, "%s:  HdfSbufReadUint8 failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

static int32_t GetIfNames(struct HdfSBuf *reply, char **ifNames, uint32_t *num)
{
    uint32_t i;
    uint32_t replayDataSize = 0;
    const char *replayData = NULL;

    if (!HdfSbufReadUint32(reply, num)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadUint32 failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    *ifNames = (char *)calloc(*num, IFNAMSIZ);
    if (*ifNames == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: calloc failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize) ||
        replayDataSize < (*num * IFNAMSIZ)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadBuffer failed", __FUNCTION__);
        free(*ifNames);
        *ifNames = NULL;
        return RET_CODE_FAILURE;
    }

    for (i = 0; i < *num; i++) {
        if (memcpy_s(*ifNames + i * IFNAMSIZ, IFNAMSIZ, replayData + i * IFNAMSIZ,
            replayDataSize) != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
            free(*ifNames);
            *ifNames = NULL;
            return RET_CODE_FAILURE;
        }
    }
    return RET_CODE_SUCCESS;
}

int32_t GetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifNames == NULL || num == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do {
        if (!HdfSbufWriteUint8(data, chipId)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufWriteUint8 failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_GET_IFNAMES, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            break;
        }
        ret = GetIfNames(reply, ifNames, num);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t SetResetDriver(const uint8_t chipId, const char *ifName)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do{
        if (!HdfSbufWriteUint8(data, chipId)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufWriteUint8 failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        if (!HdfSbufWriteString(data, ifName)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        ret = SendCmdSync(WIFI_HAL_CMD_RESET_DRIVER, data, reply);
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t GetNetDeviceInfo(struct NetDeviceInfoResult *netDeviceInfoResult)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t netdevNum = 0;
    uint32_t ifNameSize;
    uint32_t macSize;
    uint32_t i;
    const uint8_t *replayData = NULL;
    const char *ifName = NULL;

    if (netDeviceInfoResult == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    if (HdfSbufObtainDefault(&data, &reply) != RET_CODE_SUCCESS) {
        return RET_CODE_FAILURE;
    }
    do{
        ret = SendCmdSync(WIFI_HAL_CMD_GET_NETDEV_INFO, data, reply);
        if (ret != RET_CODE_SUCCESS) {
            break;
        }
        if (!HdfSbufReadUint32(reply, &netdevNum)) {
            HILOG_ERROR(LOG_DOMAIN, "%s: HdfSbufReadUint32 failed", __FUNCTION__);
            ret = RET_CODE_FAILURE;
            break;
        }
        for (i = 0; i < netdevNum; i++) {
            if (!HdfSbufReadUint32(reply, &(netDeviceInfoResult->deviceInfos[i].index)) ||
                !HdfSbufReadBuffer(reply, (const void **)(&ifName), &ifNameSize) ||
                !HdfSbufReadUint8(reply, &(netDeviceInfoResult->deviceInfos[i].iftype)) ||
                !HdfSbufReadBuffer(reply, (const void **)(&replayData), &macSize)) {
                HILOG_ERROR(LOG_DOMAIN, "%s: read fail!", __FUNCTION__);
                ret = RET_CODE_FAILURE;
                break;
            }
            if (memcpy_s(netDeviceInfoResult->deviceInfos[i].ifName, ifNameSize, ifName, ifNameSize) != EOK) {
                HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
                ret = RET_CODE_FAILURE;
                break;
            }
            if (memcpy_s(netDeviceInfoResult->deviceInfos[i].mac, macSize, replayData, macSize) != EOK) {
                HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
                ret = RET_CODE_FAILURE;
                break;
            }
        }
    } while (0);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t WifiEapolPacketSend(const char *ifName, const uint8_t *srcAddr, const uint8_t *dstAddr, uint8_t *buf,
    uint32_t length)
{
    (void)srcAddr;
    (void)dstAddr;
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || buf == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Init HdfSBuf failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, buf, length);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SEND_EAPOL, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

#define DEFAULT_EAPOL_PACKAGE_SIZE 800

int32_t WifiEapolPacketReceive(const char *ifName, WifiRxEapol *rxEapol)
{
    int32_t ret;
    WifiRxEapol eapol = { 0 };
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *respData = NULL;

    if (ifName == NULL || rxEapol == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    respData = HdfSbufObtain(DEFAULT_EAPOL_PACKAGE_SIZE);
    if (data == NULL || respData == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Init HdfSBuf failed", __FUNCTION__);
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    ret = SendCmdSync(WIFI_WPA_CMD_RECEIVE_EAPOL, data, respData);
    if (ret != HDF_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: WifiEapolPacketReceive failed ret = %d", __FUNCTION__, ret);
        goto RELEASE_DATA;
    }
    if (!HdfSbufReadBuffer(respData, (const void **)(&(eapol.buf)), &(eapol.len))) {
        ret = RET_CODE_FAILURE;
        HILOG_ERROR(LOG_DOMAIN, "%s: WifiEapolPacketReceive HdfSbufReadBuffer failed", __FUNCTION__);
        goto RELEASE_DATA;
    }
    rxEapol->buf = NULL;
    rxEapol->len = 0;
    if (eapol.len != 0) {
        rxEapol->buf = malloc(eapol.len);
        if (rxEapol->buf == NULL) {
            ret = RET_CODE_FAILURE;
            goto RELEASE_DATA;
        }
        if (memcpy_s(rxEapol->buf, eapol.len, eapol.buf, eapol.len) != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
        }
        rxEapol->len = eapol.len;
    }

RELEASE_DATA:
    HdfSbufRecycle(respData);
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiEapolEnable(const char *ifName)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    if (HdfSbufWriteString(data, ifName)) {
        ret = SendCmdSync(WIFI_WPA_CMD_ENALBE_EAPOL, data, NULL);
    } else {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    }
    HdfSbufRecycle(data);

    return ret;
}

int32_t WifiEapolDisable(const char *ifName)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    if (HdfSbufWriteString(data, ifName)) {
        ret = SendCmdSync(WIFI_WPA_CMD_DISABLE_EAPOL, data, NULL);
    } else {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdSetAp(const char *ifName, WifiApSetting *apsettings)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || apsettings == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.mode);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.freq);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.channel);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.htEnabled);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.secChannelOffset);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.vhtEnabled);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.centerFreq1);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.centerFreq2);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->freqParams.bandwidth);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, apsettings->freqParams.band);

    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->beaconInterval);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, apsettings->dtimPeriod);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, apsettings->hiddenSsid);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, apsettings->authType);
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->beaconData.head, apsettings->beaconData.headLen);
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->beaconData.tail, apsettings->beaconData.tailLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->ssid, apsettings->ssidLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->meshSsid, apsettings->meshSsidLen);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SET_AP, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdChangeBeacon(const char *ifName, WifiApSetting *apsettings)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || apsettings == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->beaconData.head, apsettings->beaconData.headLen);
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->beaconData.tail, apsettings->beaconData.tailLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->ssid, apsettings->ssidLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, apsettings->meshSsid, apsettings->meshSsidLen);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_CHANGE_BEACON, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdSendMlme(const char *ifName, WifiMlmeData *mlme)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || mlme == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, mlme, sizeof(WifiMlmeData));
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, mlme->data, mlme->dataLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, mlme->cookie, sizeof(*mlme->cookie));
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SEND_MLME, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

static int32_t WifiCmdOperKey(const char *ifName, uint32_t cmd, WifiKeyExt *keyExt)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || keyExt == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, keyExt->type);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(data, keyExt->keyIdx);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(data, keyExt->cipher);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, keyExt->def);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, keyExt->defMgmt);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, keyExt->defaultTypes);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, keyExt->resv);
    if (keyExt->addr == NULL) {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, keyExt->addr, 0);
    } else {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, keyExt->addr, ETH_ADDR_LEN);
    }
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, keyExt->key, keyExt->keyLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, keyExt->seq, keyExt->seqLen);

    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(cmd, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdDelKey(const char *ifName, WifiKeyExt *keyExt)
{
    return WifiCmdOperKey(ifName, WIFI_WPA_CMD_DEL_KEY, keyExt);
}

int32_t WifiCmdNewKey(const char *ifName, WifiKeyExt *keyExt)
{
    return WifiCmdOperKey(ifName, WIFI_WPA_CMD_NEW_KEY, keyExt);
}

int32_t WifiCmdSetKey(const char *ifName, WifiKeyExt *keyExt)
{
    return WifiCmdOperKey(ifName, WIFI_WPA_CMD_SET_KEY, keyExt);
}

int32_t WifiCmdSetMode(const char *ifName, WifiSetMode *setMode)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || setMode == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, setMode, sizeof(*setMode));
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SET_MODE, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdGetOwnMac(const char *ifName, void *buf, uint32_t len)
{
    (void)len;
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || buf == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    reply = HdfSbufObtainDefaultSize();
    if (data == NULL || reply == NULL) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    if (HdfSbufWriteString(data, ifName)) {
        ret = SendCmdSync(WIFI_WPA_CMD_GET_ADDR, data, reply);
    } else {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    }
    if (ret) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;
    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize) || replayDataSize != ETH_ADDR_LEN) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail or data size mismatch", __FUNCTION__);
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    if (memcpy_s(buf, len, replayData, replayDataSize) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __func__);
        ret = RET_CODE_FAILURE;
    }

RELEASE_DATA:
    HdfSbufRecycle(reply);
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdGetHwFeature(const char *ifName, WifiHwFeatureData *hwFeatureData)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (ifName == NULL || hwFeatureData == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    reply = HdfSbufObtain(sizeof(WifiHwFeatureData) + sizeof(uint64_t));
    if (data == NULL || reply == NULL) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    if (HdfSbufWriteString(data, ifName)) {
        ret = SendCmdSync(WIFI_WPA_CMD_GET_HW_FEATURE, data, reply);
    } else {
        ret = RET_CODE_FAILURE;
    }
    if (ret) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    const WifiHwFeatureData *respFeaturenData = NULL;
    uint32_t dataSize = 0;
    if (!HdfSbufReadBuffer(reply, (const void **)(&respFeaturenData), &dataSize) ||
        dataSize != sizeof(WifiHwFeatureData)) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    if (memcpy_s(hwFeatureData, sizeof(WifiHwFeatureData), respFeaturenData, dataSize) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __func__);
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
RELEASE_DATA:
    HdfSbufRecycle(reply);
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdScan(const char *ifName, WifiScan *scan)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || scan == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    if (scan->bssid == NULL) {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, scan->bssid, 0);
    } else {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, scan->bssid, ETH_ADDR_LEN);
    }
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, scan->ssids, sizeof(scan->ssids[0]) * scan->numSsids);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, scan->extraIes, scan->extraIesLen);
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, scan->freqs, sizeof(scan->freqs[0]) * scan->numFreqs);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, scan->prefixSsidScanFlag);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, scan->fastConnectFlag);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SCAN, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdDisconnet(const char *ifName, int32_t reasonCode)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint16(data, reasonCode);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_DISCONNECT, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdAssoc(const char *ifName, WifiAssociateParams *assocParams)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || assocParams == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    if (assocParams->bssid == NULL) {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, assocParams->bssid, 0);
    } else {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, assocParams->bssid, ETH_ADDR_LEN);
    }
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, assocParams->ssid, assocParams->ssidLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, assocParams->ie, assocParams->ieLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, assocParams->key, assocParams->keyLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, assocParams->authType);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, assocParams->privacy);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, assocParams->keyIdx);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, assocParams->mfp);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(data, assocParams->freq);
    isSerializeFailed =
        isSerializeFailed || !HdfSbufWriteBuffer(data, assocParams->crypto, sizeof(assocParams->crypto[0]));
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_ASSOC, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdSetNetdev(const char *ifName, WifiSetNewDev *info)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || info == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, info, sizeof(WifiSetNewDev));
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SET_NETDEV, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdStaRemove(const char *ifName, const uint8_t *addr, uint32_t addrLen)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || addr == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, addr, addrLen);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_STA_REMOVE, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdSendAction(const char *ifName, WifiActionData *actionData)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (ifName == NULL || actionData == NULL) {
        return RET_CODE_INVALID_PARAM;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);

    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, actionData->bssid, ETH_ADDR_LEN);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, actionData->dst, ETH_ADDR_LEN);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, actionData->src, ETH_ADDR_LEN);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, actionData->data, actionData->dataLen);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SEND_ACTION, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdSetClient(uint32_t clientNum)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, clientNum)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: sbuf write failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_CLIENT_CMD_SET_CLIENT, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdProbeReqReport(const char* ifName, const int32_t *report)
{
    if (ifName == NULL || report == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(data, *report);
    int32_t ret;
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "Serialize failed.");
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_PROBE_REQ_REPORT, data, NULL);
        HdfSbufRecycle(data);
        return ret;
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdRemainOnChannel(const char* ifName, const WifiOnChannel *onChannel)
{
    if (ifName == NULL || onChannel == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(data, onChannel->freq);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(data, onChannel->duration);
    int32_t ret;
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "Serialize failed.");
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_REMAIN_ON_CHANNEL, data, NULL);
        HdfSbufRecycle(data);
        return ret;
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdCancelRemainOnChannel(const char* ifName)
{
    if (ifName == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifName);
    int32_t ret;
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "Serialize failed.");
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_CANCEL_REMAIN_ON_CHANNEL, data, NULL);
        HdfSbufRecycle(data);
        return ret;
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdAddIf(const char *ifname, const WifiIfAdd *ifAdd)
{
    if (ifname == NULL || ifAdd == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifname);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, ifAdd->type);
    int32_t ret;
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_ADD_IF, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdRemoveIf(const char *ifname, const WifiIfRemove *ifRemove)
{
    if (ifname == NULL || ifRemove == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifname);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, ifRemove, sizeof(WifiIfRemove));
    int32_t ret;
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_REMOVE_IF, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdSetApWpsP2pIe(const char *ifname, const WifiAppIe *appIe)
{
    if (ifname == NULL || appIe == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return RET_CODE_FAILURE;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifname);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(data, appIe->ieLen);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint8(data, appIe->appIeType);
    if (appIe->ie == NULL) {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, appIe->ie, 0);
    } else {
        isSerializeFailed = isSerializeFailed || !HdfSbufWriteBuffer(data, appIe->ie, appIe->ieLen);
    }
    int32_t ret;
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_SET_AP_WPS_P2P_IE, data, NULL);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t WifiCmdGetDrvFlags(const char *ifname, WifiGetDrvFlags *params)
{
    int32_t ret;

    if (ifname == NULL || params == NULL) {
        return RET_CODE_FAILURE;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    struct HdfSBuf *reply = HdfSbufObtainDefaultSize();
    if (ifname == NULL || reply == NULL) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteString(data, ifname);
    if (isSerializeFailed) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Serialize failed!", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    } else {
        ret = SendCmdSync(WIFI_WPA_CMD_GET_DRIVER_FLAGS, data, reply);
    }
    if (ret) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
    if (!HdfSbufReadUint64(reply, &(params->drvFlags))) {
        ret = RET_CODE_FAILURE;
        goto RELEASE_DATA;
    }
RELEASE_DATA:
    HdfSbufRecycle(reply);
    HdfSbufRecycle(data);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
