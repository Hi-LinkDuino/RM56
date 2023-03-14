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

#include "frameworks/bridge/codec/standard_function_codec.h"

#include "base/log/log.h"
#include "frameworks/bridge/codec/standard_codec_buffer_operator.h"

namespace OHOS::Ace::Framework {

bool StandardFunctionCodec::EncodeFunctionCall(const FunctionCall& functionCall, std::vector<uint8_t>& resultBuffer)
{
    if (functionCall.GetArgs().size() > MAX_PARAMETERS_COUNT) {
        LOGW("Too many args for a function call");
        return false;
    }

    StandardCodecBufferWriter bufferWriter(resultBuffer);
    bufferWriter.WriteData(CodecData(functionCall.GetFuncName()));
    bufferWriter.WriteDataList(functionCall.GetArgs());
    return true;
}

bool StandardFunctionCodec::DecodeFunctionCall(const std::vector<uint8_t>& buffer, FunctionCall& functionCall)
{
    StandardCodecBufferReader bufferReader(buffer);
    CodecData funcName;
    if (!bufferReader.ReadData(funcName)) {
        LOGW("Decode funcName failed");
        return false;
    }

    std::vector<CodecData> args;
    if (!bufferReader.ReadDataList(args)) {
        LOGW("Decode args failed");
        return false;
    }

    functionCall.SetFuncName(funcName.GetStringValue());
    functionCall.SetArgs(std::move(args));
    return true;
}

bool StandardFunctionCodec::DecodePlatformMessage(const std::vector<uint8_t>& buffer, CodecData& platformMessage)
{
    StandardCodecBufferReader bufferReader(buffer);
    if (!bufferReader.ReadData(platformMessage)) {
        LOGW("Decode platform message failed");
        return false;
    }
    return true;
}

} // namespace OHOS::Ace::Framework