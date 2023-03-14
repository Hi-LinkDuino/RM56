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

#ifndef OHOS_DM_SUBSCRIBE_INFO_H
#define OHOS_DM_SUBSCRIBE_INFO_H

#include <stdbool.h>

#define DM_MAX_DEVICE_CAPABILITY_LEN 65

namespace OHOS {
namespace DistributedHardware {
typedef enum DmDiscoverMode {
    /* Passive */
    DM_DISCOVER_MODE_PASSIVE = 0x55,
    /* Proactive */
    DM_DISCOVER_MODE_ACTIVE = 0xAA
} DmDiscoverMode;

typedef enum DmExchangeMedium {
    /** Automatic medium selection */
    DM_AUTO = 0,
    /** Bluetooth */
    DM_BLE = 1,
    /** Wi-Fi */
    DM_COAP = 2,
    /** USB */
    DM_USB = 3,
    DM_MEDIUM_BUTT
} DmExchangeMedium;

/**
 * @brief Enumerates frequencies for publishing services.
 *
 * This enumeration applies only to Bluetooth and is not supported currently.
 */
typedef enum DmExchangeFreq {
    /** Low */
    DM_LOW = 0,
    /** Medium */
    DM_MID = 1,
    /** High */
    DM_HIGH = 2,
    /** Super-high */
    DM_SUPER_HIGH = 3,
    DM_FREQ_BUTT
} DmExchangeFreq;

typedef struct DmSubscribeInfo {
    /** Service ID */
    uint16_t subscribeId;
    /** Discovery mode for service subscription. For details, see {@link DmDiscoverMode}. */
    DmDiscoverMode mode;
    /** Service subscription medium. For details, see {@link DmExchangeMedium}. */
    DmExchangeMedium medium;
    /** Service subscription frequency. For details, see {@link DmExchangeFreq}. */
    DmExchangeFreq freq;
    /** only find the device with the same account */
    bool isSameAccount;
    /** find the sleeping devices */
    bool isWakeRemote;
    /** Service subscription capability. */
    char capability[DM_MAX_DEVICE_CAPABILITY_LEN];
} DmSubscribeInfo;
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_SUBSCRIBE_INFO_H
