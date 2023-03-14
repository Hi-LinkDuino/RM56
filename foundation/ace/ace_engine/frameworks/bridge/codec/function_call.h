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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_FUNCTION_CALL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_FUNCTION_CALL_H

#include <string>
#include <vector>

#include "base/utils/noncopyable.h"
#include "frameworks/bridge/codec/codec_data.h"

namespace OHOS::Ace::Framework {

constexpr int32_t MAX_PARAMETERS_COUNT = UINT8_MAX;

class FunctionCall final {
public:
    FunctionCall(const std::string& funcName, const std::vector<CodecData>& args) : funcName_(funcName), args_(args) {}
    FunctionCall(std::string&& funcName, std::vector<CodecData>&& args)
        : funcName_(std::move(funcName)), args_(std::move(args))
    {}
    FunctionCall() = default;
    ~FunctionCall() = default;

    const std::string& GetFuncName() const
    {
        return funcName_;
    }

    const std::vector<CodecData>& GetArgs() const
    {
        return args_;
    }

    void SetFuncName(const std::string& funcName)
    {
        funcName_ = funcName;
    }

    void SetArgs(const std::vector<CodecData>& args)
    {
        args_ = args;
    }

    void SetFuncName(std::string&& funcName)
    {
        funcName_ = std::move(funcName);
    }

    void SetArgs(std::vector<CodecData>&& args)
    {
        args_ = std::move(args);
    }

private:
    std::string funcName_;
    std::vector<CodecData> args_;

    ACE_DISALLOW_COPY_AND_MOVE(FunctionCall);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_FUNCTION_CALL_H
