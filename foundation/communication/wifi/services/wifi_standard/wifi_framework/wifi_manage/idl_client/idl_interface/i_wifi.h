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

#ifndef OHOS_IDL_IWIFI_H
#define OHOS_IDL_IWIFI_H

#include "wifi_error_no.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Obtains the chip object by ID.
 *
 * @param id
 * @param chip
 * @return WifiErrorNo
 */
WifiErrorNo GetWifiChip(uint8_t id, IWifiChip *chip);

/**
 * @Description Obtains the ID of a Wi-Fi chip.
 *
 * @param ids
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo GetWifiChipIds(uint8_t *ids, int32_t *size);

/**
 * @Description Start WiFi.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Start(void);

/**
 * @Description Stop WiFi.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Stop(void);

/**
 * @Description When the interface service exits, the HAL layer is
 *              instructed to clear the data.
 *
 * @return WifiErrorNo
 */
WifiErrorNo NotifyClear(void);

#ifdef __cplusplus
}
#endif
#endif