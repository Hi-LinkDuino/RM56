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

#ifndef WIFI_HAL_COMMON_H
#define WIFI_HAL_COMMON_H

#include <stdint.h>
#include "hdf_dlist.h"
#include "wifi_hal_base_feature.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct IWiFiList {
    char ifName[IFNAME_MAX_LEN + 1];
    uint8_t supportMode[PROTOCOL_80211_IFTYPE_NUM];
    struct IWiFiBaseFeature *ifeature;
    struct DListHead entry;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* WIFI_HAL_COMMON_H */
