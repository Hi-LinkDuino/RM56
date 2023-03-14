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

#include "core/common/platform_bridge.h"

#include "base/log/log.h"
#include "frameworks/bridge/codec/function_call.h"
#include "frameworks/bridge/codec/standard_function_codec.h"

namespace OHOS::Ace {

const char INTERNAL_MODULE_GROUP[] = "AcePluginGroup/FeatureAbility";
const char FUNCTION_NAME[] = "callAbility";

using namespace Framework;

void PlatformBridge::SendMessage(const std::vector<CodecData>& args, const PlatformCallbackHandler& handler)
{
    int32_t callbackId = callbackIds_++;
    auto result = callBackHandlers_.try_emplace(callbackId, handler);
    if (!result.second) {
        LOGE("module callback function has been existed!");
        return;
    }

    FunctionCall functionCall(FUNCTION_NAME, args);
    StandardFunctionCodec codec;
    std::vector<uint8_t> dataBuf;
    codec.EncodeFunctionCall(functionCall, dataBuf);

    auto dispatcher = dispatcher_.Upgrade();
    if (dispatcher) {
        dispatcher->Dispatch(INTERNAL_MODULE_GROUP, std::move(dataBuf), callbackId, true);
    } else {
        LOGE("SendMessage failed: dispatcher is null");
    }
}

void PlatformBridge::HandleCallback(int32_t callbackId, std::vector<uint8_t>&& messageData)
{
    std::string resultString;
    CodecData codecResult;
    StandardFunctionCodec codec;
    if (codec.DecodePlatformMessage(messageData, codecResult)) {
        resultString = codecResult.GetStringValue();
        if (resultString.empty()) {
            LOGE("reply message is empty!");
            return;
        }
    } else {
        LOGE("decode platform reply message failed!");
        return;
    }

    auto itFunc = callBackHandlers_.find(callbackId);
    if (itFunc != callBackHandlers_.end()) {
        auto handler = itFunc->second;
        if (handler) {
            handler(resultString);
        }
        callBackHandlers_.erase(itFunc);
    }
}

} // namespace OHOS::Ace
