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

#ifndef SOFTBUS_BUS_CENTER_ADAPTER_H
#define SOFTBUS_BUS_CENTER_ADAPTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COMM_DEVICE_KEY_UDID,
    COMM_DEVICE_KEY_DEVTYPE,
    COMM_DEVICE_KEY_DEVNAME,
    COMM_DEVICE_KEY_BT_MAC,
    COMM_DEVICE_KEY_BUTT
} CommonDeviceKey;

#define TYPE_UNKNOWN "UNKNOWN"
#define TYPE_PHONE "PHONE"
#define TYPE_PAD "PAD"
#define TYPE_TV "TV"
#define TYPE_CAR "CAR"
#define TYPE_WATCH "WATCH"
#define TYPE_IPCAMERA "WiFiCamara"

int32_t GetCommonDevInfo(const CommonDeviceKey key, char *value, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif // SOFTBUS_BUS_CENTER_ADAPTER_H
