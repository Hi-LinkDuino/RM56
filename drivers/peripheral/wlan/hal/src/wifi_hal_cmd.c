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

#include "wifi_hal_cmd.h"
#include <osal_mem.h>
#include "hdf_log.h"
#include "securec.h"
#include "wifi_hal_sta_feature.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static struct DListHead g_networkHead = {0};

struct DListHead *GetNetworkHead(void)
{
    return &g_networkHead;
}

int32_t HalCmdGetAvailableNetwork(void)
{
    int32_t ret;
    struct NetworkInfoResult networkInfo;
    uint32_t i;

    ret = GetUsableNetworkInfo(&networkInfo);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get network info failed", __FUNCTION__);
        return ret;
    }
    if (!DListIsEmpty(&g_networkHead)) {
        ClearIWiFiList();
    }
    for (i = 0; i < networkInfo.nums; i++) {
        struct IWiFiList *networkList = (struct IWiFiList *)malloc(sizeof(struct IWiFiList));
        if (networkList == NULL) {
            HDF_LOGE("%s: malloc failed, line: %d", __FUNCTION__, __LINE__);
            ClearIWiFiList();
            return HDF_FAILURE;
        }
        (void)memset_s(networkList, sizeof(struct IWiFiList), 0, sizeof(struct IWiFiList));
        DListInsertTail(&networkList->entry, &g_networkHead);
        if (memcpy_s(networkList->ifName, IFNAME_MAX_LEN, networkInfo.infos[i].name,
            strlen(networkInfo.infos[i].name)) != EOK) {
            HDF_LOGE("%s: memcpy_s failed, line: %d", __FUNCTION__, __LINE__);
            ClearIWiFiList();
            return HDF_FAILURE;
        }
        if (memcpy_s(networkList->supportMode, PROTOCOL_80211_IFTYPE_NUM,
            networkInfo.infos[i].supportMode, PROTOCOL_80211_IFTYPE_NUM) != EOK) {
            HDF_LOGE("%s: memcpy_s failed, line: %d", __FUNCTION__, __LINE__);
            ClearIWiFiList();
            return HDF_FAILURE;
        }
        networkList->ifeature = NULL;
    }
    return ret;
}

static void GetSupportTypeByList(uint8_t *supType)
{
    int32_t i;
    struct IWiFiList *networkList = NULL;

    DLIST_FOR_EACH_ENTRY(networkList, &g_networkHead, struct IWiFiList, entry) {
        for (i = 0; i < PROTOCOL_80211_IFTYPE_NUM; i++) {
            if (networkList->supportMode[i] == 1) {
                supType[i] = 1;
            }
        }
    }
}

int32_t HalCmdGetSupportType(uint8_t *supType)
{
    int32_t ret;
    uint8_t isComboValid;

    GetSupportTypeByList(supType);
    ret = IsSupportCombo(&isComboValid);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:IsSupportCombo failed, line: %d", __FUNCTION__, __LINE__);
        return ret;
    }
    supType[PROTOCOL_80211_IFTYPE_NUM] = isComboValid;

    return ret;
}

int32_t HalCmdGetSupportCombo(uint64_t *supCombo, uint32_t size)
{
    int32_t ret;

    ret = GetComboInfo(supCombo, size);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GetComboInfo failed, line: %d", __FUNCTION__, __LINE__);
    }
    return ret;
}

int32_t HalCmdGetDevMacAddr(const char *ifName, int32_t type, unsigned char *mac, uint8_t len)
{
    int32_t ret;

    ret = GetDevMacAddr(ifName, type, (uint8_t *)mac, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:GetDevMacAddr failed, line: %d", __FUNCTION__, __LINE__);
    }
    return ret;
}

int32_t HalCmdSetMacAddr(const char *ifName, unsigned char *mac, uint8_t len)
{
    int32_t ret;

    ret = SetMacAddr(ifName, mac, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetMacAddr failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdGetValidFreqWithBand(const char *ifName, int32_t band, int32_t *freqs,
    uint32_t size, uint32_t *num)
{
    int32_t ret;
    struct FreqInfoResult result;

    result.freqs = OsalMemCalloc(size * sizeof(int32_t));
    if (result.freqs == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __FUNCTION__);
        return HDF_FAILURE;
    }

    result.txPower = OsalMemCalloc(size * sizeof(int32_t));
    if (result.txPower == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __FUNCTION__);
        OsalMemFree(result.freqs);
        return HDF_FAILURE;
    }

    do {
        ret = GetValidFreqByBand(ifName, band, &result, size);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: GetValidFreqByBand failed", __FUNCTION__);
            break;
        }
        if (memcpy_s(freqs, size * sizeof(int32_t), result.freqs, result.nums * sizeof(int32_t)) != EOK) {
            HDF_LOGE("%s: memcpy failed, line: %d", __FUNCTION__, __LINE__);
            ret = HDF_FAILURE;
            break;
        }
        *num = result.nums;
    } while (0);

    OsalMemFree(result.txPower);
    OsalMemFree(result.freqs);
    return ret;
}

int32_t HalCmdSetTxPower(const char *ifName, int32_t power)
{
    int32_t ret;
    ret = SetTxPower(ifName, power);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetTxPower failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdGetAsscociatedStas(const char *ifName, struct StaInfo *staInfo, uint32_t count, uint32_t *num)
{
    if (num == NULL) {
        HDF_LOGE("%s: HalCmdGetAsscociatedStas num NULL!", __FUNCTION__);
        return HDF_FAILURE;
    }
    int32_t ret;
    struct AssocStaInfoResult result;

    ret = GetAssociatedStas(ifName, &result);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GetAssociatedStas failed", __FUNCTION__);
        return ret;
    }
    if (memcpy_s(staInfo, count * sizeof(*staInfo), result.infos,
        result.num * sizeof(struct AssocStaInfo)) != EOK) {
        HDF_LOGE("%s: memcpy staInfo failed", __FUNCTION__);
        return HDF_FAILURE;
    }
    *num = result.num;
    return ret;
}

int32_t HalCmdSetCountryCode(const char *ifName, const char *code, uint32_t len)
{
    int32_t ret;
    ret = WifiSetCountryCode(ifName, code, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiSetCountryCode failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdSetScanningMacAddress(const char *ifName, unsigned char *scanMac, uint8_t len)
{
    int32_t ret;
    ret = SetScanMacAddr(ifName, scanMac, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetScanMacAddr failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdStartScanInner(const char *ifName, WifiScan *scan)
{
    int32_t ret;
    ret = WifiCmdScan(ifName, (WifiScan *)scan);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetScanMacAddr failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdGetChipId(const char *ifName, uint8_t *chipId)
{
    int32_t ret;
    ret = AcquireChipId(ifName, chipId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcquireChipId failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdGetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num)
{
    int32_t ret;
    ret = GetIfNamesByChipId(chipId, ifNames, num);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GetIfNamesByChipId failed", __FUNCTION__);
    }
    return ret;
}

int32_t HalCmdSetResetDriver(const uint8_t chipId, const char *ifName)
{
    int32_t ret;
    ret = SetResetDriver(chipId, ifName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetResetDriver failed", __FUNCTION__);
    }
    return ret;
}

void ClearIWiFiList(void)
{
    struct IWiFiList *networkList = NULL;
    struct IWiFiList *tmp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(networkList, tmp, &g_networkHead, struct IWiFiList, entry) {
        DListRemove(&networkList->entry);
        free(networkList);
        networkList = NULL;
    }
    InitIWiFiList();
}

void InitIWiFiList(void)
{
    DListHeadInit(&g_networkHead);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif