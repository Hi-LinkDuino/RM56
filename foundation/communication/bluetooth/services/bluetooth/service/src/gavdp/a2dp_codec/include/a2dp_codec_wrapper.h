/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef A2DP_CODEC_WRAPPER_H
#define A2DP_CODEC_WRAPPER_H

#include <cstddef>
#include <cstdint>
#include <list>

#include "a2dp_codec_config.h"
#include "base_def.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
// A2dp encoder interface
class A2dpEncoder {
public:
    explicit A2dpEncoder(A2dpCodecConfig *config)
        : config_(config), transmitQueueLength_(0)
    {}
    virtual ~A2dpEncoder() = default;
    virtual void ResetFeedingState(void) = 0;
    virtual void SetTransmitQueueLength(size_t length)
    {
        transmitQueueLength_ = length;
    }
    virtual void SendFrames(uint64_t timeStampUs) = 0;
    virtual void UpdateEncoderParam() = 0;
    virtual void GetRenderPosition(uint16_t &sendDataSize, uint32_t &timeStamp) = 0;

protected:
    BT_DISALLOW_COPY_AND_ASSIGN(A2dpEncoder);
    A2dpCodecConfig *config_;
    size_t transmitQueueLength_;
};

// A2dpDecoderObserver is responsible of receiving decoded audio data from a2dp
// decoder
class A2dpDecoderObserver {
public:
    virtual ~A2dpDecoderObserver() = default;
    virtual void DataAvailable(uint8_t *buf, uint32_t size) = 0;
};

// A2dp decoder interface
class A2dpDecoder {
public:
    A2dpDecoder(A2dpDecoderObserver *observer) : observer_(observer)
    {}
    virtual ~A2dpDecoder() = default;
    virtual bool DecodePacket(uint8_t *data, uint16_t size) = 0;

protected:
    BT_DISALLOW_COPY_AND_ASSIGN(A2dpDecoder);
    A2dpDecoderObserver *observer_;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !A2DP_CODEC_WRAPPER_H
