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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_STANDARD_FUNCTION_CODEC_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_STANDARD_FUNCTION_CODEC_H

#include "base/utils/macros.h"
#include "frameworks/bridge/codec/function_codec.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT StandardFunctionCodec final : public FunctionCodec {
public:
    StandardFunctionCodec() = default;
    ~StandardFunctionCodec() override = default;

    bool EncodeFunctionCall(const FunctionCall& functionCall, std::vector<uint8_t>& resultBuffer) override;
    bool DecodeFunctionCall(const std::vector<uint8_t>& buffer, FunctionCall& functionCall) override;
    bool DecodePlatformMessage(const std::vector<uint8_t>& buffer, CodecData& platformMessage) override;

private:
    ACE_DISALLOW_COPY_AND_MOVE(StandardFunctionCodec);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_STANDARD_FUNCTION_CODEC_H
