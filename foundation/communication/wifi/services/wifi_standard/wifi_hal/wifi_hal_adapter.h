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

#ifndef OHOS_WIFI_HAL_ADAPTER_H
#define OHOS_WIFI_HAL_ADAPTER_H

#include "wifi_hal_vendor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StWifiHalVendorInterface {
    void *handle;
    WifiHalVendorFunc func;
} WifiHalVendorInterface;

/**
 * @Description Get the Wifi Hal Vendor Interface object.
 *
 * @return WifiHalVendorInterface*
 */
WifiHalVendorInterface *GetWifiHalVendorInterface(void);
/**
 * @Description Release the Wifi Hal Vendor Interface object.
 *
 */
void ReleaseWifiHalVendorInterface(void);

#ifdef __cplusplus
}
#endif
#endif