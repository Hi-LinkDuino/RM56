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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "wifi_log.h"
#include "wifi_hal_vendor_interface.h"
#include "securec.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalVendorInterfaceTest"

const int FEATURE_VALUE = 2;

HalVendorError WifiInitializeTest(void)
{
    return HAL_VENDOR_SUCCESS;
}

HalVendorError WifiCleanUpTest(void)
{
    return HAL_VENDOR_SUCCESS;
}

HalVendorError WifiGetSupportedFeatureTest(long *feature)
{
    *feature = FEATURE_VALUE;
    return HAL_VENDOR_SUCCESS;
}

HalVendorError InitHalVendorFunc(WifiHalVendorFunc *func)
{
    func->wifiInitialize = WifiInitializeTest;
    func->wifiCleanUp = WifiCleanUpTest;
    func->wifiGetSupportedFeature = WifiGetSupportedFeatureTest;
    return HAL_VENDOR_SUCCESS;
}
