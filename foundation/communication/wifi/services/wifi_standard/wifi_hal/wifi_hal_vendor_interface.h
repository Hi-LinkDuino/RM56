/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_HAL_VENDOR_INTERFACE_H
#define OHOS_WIFI_HAL_VENDOR_INTERFACE_H

#include <stdint.h>
#include "wifi_hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_VENDOR_SUCCESS = 0,
    HAL_VENDOR_ERROR_NONE = 1,
    HAL_VENDOR_ERROR_UNKNOWN = -1,
    HAL_VENDOR_ERROR_UNINITIALIZED = -2,
    HAL_VENDOR_ERROR_NOT_SUPPORTED = -3,
    HAL_VENDOR_ERROR_NOT_AVAILABLE = -4, /* Not available right now, but try later */
    HAL_VENDOR_ERROR_INVALID_ARGS = -5,
    HAL_VENDOR_ERROR_INVALID_REQUEST_ID = -6,
    HAL_VENDOR_ERROR_TIMED_OUT = -7,
    HAL_VENDOR_ERROR_TOO_MANY_REQUESTS = -8, /* Too many instances of this request */
    HAL_VENDOR_ERROR_OUT_OF_MEMORY = -9,
    HAL_VENDOR_ERROR_BUSY = -10,
} HalVendorError;

typedef struct {
    HalVendorError (*wifiInitialize)(void);
    HalVendorError (*wifiCleanUp)(void);
    HalVendorError (*wifiGetSupportedFeature)(long *feature);
    HalVendorError (*wifiSetScanningMacAddress)(const char *szMac, int macSize);
    HalVendorError (*wifiDeauthLastRoamingBssid)(const char *szMac, int macSize);
    HalVendorError (*wifiSetWifiTxPower)(int32_t power);
    HalVendorError (*wifiConfigComboModes)(int32_t mode);
    HalVendorError (*wifiGetComboModes)(int32_t *id);
    HalVendorError (*wifiRequestFirmwareDebugDump)(unsigned char *bytes, int32_t *size);
    HalVendorError (*wifiSetMiracastMode)(int32_t mode);
} WifiHalVendorFunc;

typedef HalVendorError (*pInitHalVendorFunc)(WifiHalVendorFunc *func);
/**
 * @Description Init Default Hal Vendor Func.
 *
 * @param func
 * @return HalVendorError
 */
HalVendorError InitDefaultHalVendorFunc(WifiHalVendorFunc *func);
/**
 * @Description Convert Error Code.
 *
 * @param code
 * @return WifiErrorNo
 */
WifiErrorNo ConvertErrorCode(HalVendorError code);

/* vendor hal so need implement these functions */
/**
 * @Description Init Hal Vendor Func.
 *
 * @param func
 * @return HalVendorError
 */
HalVendorError InitHalVendorFunc(WifiHalVendorFunc *func);

#ifdef __cplusplus
}
#endif
#endif