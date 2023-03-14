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

#ifndef OHOS_WIFI_HAL_BASE_INTERFACE_H
#define OHOS_WIFI_HAL_BASE_INTERFACE_H

#include "wifi_hal_define.h"
#include "wifi_hal_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Obtains the name of a Wi-Fi network device.
 *
 * @param ifname - Ifname name.
 * @param size - ifname size.
 * @return WifiErrorNo
 */
WifiErrorNo GetName(char *ifname, int32_t size);

/**
 * @Description Obtains the type of a Wi-Fi network device.
 *
 * @param type - Setting type.
 * @return WifiErrorNo
 */
WifiErrorNo GetType(int32_t *type);

#ifdef __cplusplus
}
#endif
#endif