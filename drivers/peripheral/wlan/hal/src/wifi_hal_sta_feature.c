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

#include "wifi_hal_sta_feature.h"
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

static int32_t SetScanningMacAddressInner(const struct IWiFiSta *staFeature, unsigned char *scanMac, uint8_t len)
{
    if (staFeature == NULL || scanMac == NULL || len != WIFI_MAC_ADDR_LENGTH) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdSetScanningMacAddress(staFeature->baseFeature.ifName, scanMac, len);
}

static int32_t SetScanningMacAddress(const struct IWiFiSta *staFeature, unsigned char *scanMac, uint8_t len)
{
    HalMutexLock();
    int32_t ret = SetScanningMacAddressInner(staFeature, scanMac, len);
    HalMutexUnlock();
    return ret;
}

static int32_t StartScanInner(const char *ifName, WifiScan *scan)
{
    if (ifName == NULL || scan == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdStartScanInner(ifName, scan);
}

static int32_t StartScan(const char *ifName, WifiScan *scan)
{
    HalMutexLock();
    int32_t ret = StartScanInner(ifName, scan);
    HalMutexUnlock();
    return ret;
}

int32_t InitStaFeature(struct IWiFiSta **fe)
{
    if (fe == NULL || *fe == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (InitBaseFeature((struct IWiFiBaseFeature **)fe) != HDF_SUCCESS) {
        HDF_LOGE("%s: init base feature, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    (*fe)->setScanningMacAddress = SetScanningMacAddress;
    (*fe)->startScan = StartScan;
    return HDF_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif