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

#ifndef A2DP_DECODER_AAC_H
#define A2DP_DECODER_AAC_H

#include <cstdint>
#include "../../include/a2dp_codec_wrapper.h"

namespace OHOS {
namespace bluetooth {
class A2dpAacDecoder : public A2dpDecoder {
public:
    A2dpAacDecoder(A2dpDecoderObserver *observer) : A2dpDecoder(observer)
    {}
    ~A2dpAacDecoder() = default;
    bool DecodePacket(uint8_t *data, uint16_t size)
    {
        return false;
    }
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // !A2DP_DECODER_AAC_H