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

#ifndef A2DP_ENCODER_AAC_H
#define A2DP_ENCODER_AAC_H

#include <cstdint>
#include "../../include/a2dp_codec_wrapper.h"

namespace OHOS {
namespace bluetooth {
class A2dpAacEncoder : public A2dpEncoder {
public:
    A2dpAacEncoder(const A2dpEncoderInitPeerParams *peerParams, A2dpCodecConfig *config) : A2dpEncoder(config)
    {}
    ~A2dpAacEncoder() = default;
    void ResetFeedingState(void) override
    {}
    void SetTransmitQueueLength(size_t length) override
    {}
    void SendFrames(uint64_t timeStampUs) override
    {}
    void UpdateEncoderParam() override
    {}
    void GetRenderPosition(uint16_t &sendDataSize, uint32_t &timeStamp) override
    {}
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_ENCODER_AAC_H