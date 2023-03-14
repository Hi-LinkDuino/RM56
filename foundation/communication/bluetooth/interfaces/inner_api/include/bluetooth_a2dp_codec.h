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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief A2dp codec information and status define.
 *
 * @since 6
 */

#ifndef BLUETOOTH_A2DP_CODEC_H
#define BLUETOOTH_A2DP_CODEC_H

#include <vector>
#include "bluetooth_def.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief A2dp codec configuration information of a2dp source
 *
 * @since 6.0
 */
struct A2dpCodecInfo {
    // Codec priority
    uint32_t codecPriority;

    // Codec type
    uint8_t codecType;

    // Codec sample
    uint32_t sampleRate;

    // Codec bits per sample
    uint8_t bitsPerSample;

    // Codec channel mode
    uint8_t channelMode;

    // Codec specific value1
    uint64_t codecSpecific1;

    // Codec specific value2
    uint64_t codecSpecific2;

    // Codec specific value3
    uint64_t codecSpecific3;

    // Codec specific value4
    uint64_t codecSpecific4;
};

/**
 * @brief The codec configuration and capability information of a2dp source
 *
 * @since 6.0
 */
struct A2dpCodecStatus {
    // current codec information
    A2dpCodecInfo codecInfo;

    // local codec information
    std::vector<A2dpCodecInfo> codecInfoLocalCap;

    // Local device and peer confirmed codec information
    std::vector<A2dpCodecInfo> codecInfoConfirmedCap;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_A2DP_CODEC_H