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

#include "wifi_hal_base_interface.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalBaseInterface"

WifiErrorNo GetName(char *ifname, int32_t size)
{
    LOGD("GetName() size: %{public}d", size);
    if (ifname != NULL) {
        *ifname = '\0'; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetType(int32_t *type)
{
    LOGD("GetType()");
    if (type != NULL) {
        *type = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return WIFI_HAL_SUCCESS;
}
