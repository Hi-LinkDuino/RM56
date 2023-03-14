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

#include "wifi_hal_base_feature.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "securec.h"
#include "wifi_hal_cmd.h"
#include "wifi_hal_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static const char *GetNetworkIfaceNameInner(const struct IWiFiBaseFeature *baseFeature)
{
    if (baseFeature == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return NULL;
    }
    return baseFeature->ifName;
}

static int32_t GetFeatureTypeInner(const struct IWiFiBaseFeature *baseFeature)
{
    if (baseFeature == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return baseFeature->type;
}

static int32_t SetMacAddressInner(const struct IWiFiBaseFeature *baseFeature, unsigned char *mac, uint8_t len)
{
    if (baseFeature == NULL || mac == NULL || len != WIFI_MAC_ADDR_LENGTH) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdSetMacAddr(baseFeature->ifName, mac, len);
}

static int32_t GetDeviceMacAddressInner(const struct IWiFiBaseFeature *baseFeature, unsigned char *mac, uint8_t len)
{
    if (baseFeature == NULL || mac == NULL || len != WIFI_MAC_ADDR_LENGTH) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetDevMacAddr(baseFeature->ifName, baseFeature->type, mac, len);
}

static int32_t GetValidFreqsWithBandInner(const struct IWiFiBaseFeature *baseFeature,
    int32_t band, int32_t *freqs, uint32_t size, uint32_t *num)
{
    if (baseFeature == NULL || freqs == NULL || size < MAX_CHANNEL_NUM || num == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetValidFreqWithBand(baseFeature->ifName, band, freqs, size, num);
}

static int32_t SetTxPowerInner(const struct IWiFiBaseFeature *baseFeature, int32_t power)
{
    if (baseFeature == NULL || power <= 0) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdSetTxPower(baseFeature->ifName, power);
}

static int32_t GetChipIdInner(const struct IWiFiBaseFeature *baseFeature, uint8_t *chipId)
{
    if (baseFeature == NULL || chipId == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetChipId(baseFeature->ifName, chipId);
}

static int32_t GetIfNamesByChipIdInner(const uint8_t chipId, char **ifNames, uint32_t *num)
{
    if (ifNames == NULL || num == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetIfNamesByChipId(chipId, ifNames, num);
}

static const char *GetNetworkIfaceName(const struct IWiFiBaseFeature *baseFeature)
{
    HalMutexLock();
    const char *ifName = GetNetworkIfaceNameInner(baseFeature);
    HalMutexUnlock();
    return ifName;
}

static int32_t GetFeatureType(const struct IWiFiBaseFeature *baseFeature)
{
    HalMutexLock();
    int32_t type = GetFeatureTypeInner(baseFeature);
    HalMutexUnlock();
    return type;
}

static int32_t SetMacAddress(const struct IWiFiBaseFeature *baseFeature, unsigned char *mac, uint8_t len)
{
    HalMutexLock();
    int32_t ret = SetMacAddressInner(baseFeature, mac, len);
    HalMutexUnlock();
    return ret;
}

static int32_t GetDeviceMacAddress(const struct IWiFiBaseFeature *baseFeature, unsigned char *mac, uint8_t len)
{
    HalMutexLock();
    int32_t ret = GetDeviceMacAddressInner(baseFeature, mac, len);
    HalMutexUnlock();
    return ret;
}

static int32_t GetValidFreqsWithBand(const struct IWiFiBaseFeature *baseFeature,
    int32_t band, int32_t *freqs, uint32_t size, uint32_t *num)
{
    HalMutexLock();
    int32_t ret = GetValidFreqsWithBandInner(baseFeature, band, freqs, size, num);
    HalMutexUnlock();
    return ret;
}

static int32_t HalSetTxPower(const struct IWiFiBaseFeature *baseFeature, int32_t power)
{
    HalMutexLock();
    int32_t ret = SetTxPowerInner(baseFeature, power);
    HalMutexUnlock();
    return ret;
}

static int32_t HalGetChipId(const struct IWiFiBaseFeature *baseFeature, uint8_t *chipId)
{
    HalMutexLock();
    int32_t ret = GetChipIdInner(baseFeature, chipId);
    HalMutexUnlock();
    return ret;
}

static int32_t HalGetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num)
{
    HalMutexLock();
    int32_t ret = GetIfNamesByChipIdInner(chipId, ifNames, num);
    HalMutexUnlock();
    return ret;
}

int32_t InitBaseFeature(struct IWiFiBaseFeature **fe)
{
    if (fe == NULL || *fe == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    (*fe)->getNetworkIfaceName = GetNetworkIfaceName;
    (*fe)->getFeatureType = GetFeatureType;
    (*fe)->setMacAddress = SetMacAddress;
    (*fe)->getDeviceMacAddress = GetDeviceMacAddress;
    (*fe)->getValidFreqsWithBand = GetValidFreqsWithBand;
    (*fe)->setTxPower = HalSetTxPower;
    (*fe)->getChipId = HalGetChipId;
    (*fe)->getIfNamesByChipId = HalGetIfNamesByChipId;
    return HDF_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif