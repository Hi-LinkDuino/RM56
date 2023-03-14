/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_DEVICE_INFO_H
#define OHOS_DM_DEVICE_INFO_H

#include <cstdint>

#include "dm_app_image_info.h"

#define DM_MAX_DEVICE_ID_LEN (96)
#define DM_MAX_DEVICE_NAME_LEN (128)

namespace OHOS {
namespace DistributedHardware {
typedef enum DmDeviceType {
    DEVICE_TYPE_UNKNOWN = 0x00,
    DEVICE_TYPE_WIFI_CAMERA = 0x08,
    DEVICE_TYPE_AUDIO = 0x0A,
    DEVICE_TYPE_PC = 0x0C,
    DEVICE_TYPE_PHONE = 0x0E,
    DEVICE_TYPE_PAD = 0x11,
    DEVICE_TYPE_WATCH = 0x6D,
    DEVICE_TYPE_CAR = 0x83,
    DEVICE_TYPE_TV = 0x9C,
} DmDeviceType;

typedef enum DmDeviceState {
    DEVICE_STATE_UNKNOWN = -1,
    DEVICE_STATE_ONLINE = 0,
    DEVICE_INFO_READY = 1,
    DEVICE_STATE_OFFLINE = 2,
    DEVICE_INFO_CHANGED = 3,
} DmDeviceState;

typedef struct DmDeviceInfo {
    char deviceId[DM_MAX_DEVICE_ID_LEN];
    char deviceName[DM_MAX_DEVICE_NAME_LEN];
    uint16_t deviceTypeId;
    char networkId[DM_MAX_DEVICE_ID_LEN];
} DmDeviceInfo;

typedef struct DmAuthParam {
    std::string authToken;
    std::string packageName;
    std::string appName;
    std::string appDescription;
    int32_t authType;
    int32_t business;
    int32_t pincode;
    int32_t direction;
    int32_t pinToken;
    DmAppImageInfo imageinfo;
} DmAuthParam;
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_DEVICE_INFO_H
