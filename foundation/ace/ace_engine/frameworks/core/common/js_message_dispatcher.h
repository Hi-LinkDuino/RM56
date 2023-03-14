/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_JS_MESSAGE_DISPATCHER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_JS_MESSAGE_DISPATCHER_H

#include "base/memory/ace_type.h"
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
#include "adapter/preview/osal/request_data.h"
#endif

namespace OHOS::Ace {

class JsMessageDispatcher : public virtual AceType {
    DECLARE_ACE_TYPE(JsMessageDispatcher, AceType);

public:
    JsMessageDispatcher() = default;
    ~JsMessageDispatcher() override = default;

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    virtual void CallCurlFunction(const RequestData requestData, int32_t callbackId) const = 0;
#endif

    virtual void Dispatch(const std::string& channel, std::vector<uint8_t>&& data, int32_t id,
        bool replyToComponent = false) const = 0;

    virtual void DispatchSync(
        const std::string& group, std::vector<uint8_t>&& data, uint8_t** resData, int64_t& position) const = 0;

    virtual void DispatchPluginError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_JS_MESSAGE_DISPATCHER_H
