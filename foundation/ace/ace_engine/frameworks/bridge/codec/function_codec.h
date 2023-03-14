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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_FUNCTION_CODEC_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_FUNCTION_CODEC_H

#include <cstdint>
#include <vector>

#include "base/utils/noncopyable.h"
#include "frameworks/bridge/codec/codec_data.h"
#include "frameworks/bridge/codec/function_call.h"

namespace OHOS::Ace::Framework {

class FunctionCodec {
public:
    FunctionCodec() = default;
    virtual ~FunctionCodec() = default;

    virtual bool EncodeFunctionCall(const FunctionCall& functionCall, std::vector<uint8_t>& resultBuffer) = 0;
    virtual bool DecodeFunctionCall(const std::vector<uint8_t>& buffer, FunctionCall& functionCall) = 0;
    virtual bool DecodePlatformMessage(const std::vector<uint8_t>& buffer, CodecData& platformMessage) = 0;

private:
    ACE_DISALLOW_COPY_AND_MOVE(FunctionCodec);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_FUNCTION_CODEC_H
