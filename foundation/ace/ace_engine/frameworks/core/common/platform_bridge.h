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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_BRIDGE_H

#include <functional>
#include <map>
#include <vector>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "bridge/codec/codec_data.h"
#include "core/common/js_message_dispatcher.h"

namespace OHOS::Ace {

using PlatformCallbackHandler = std::function<void(const std::string&)>;
using PlatformCallbackMap = std::map<int32_t, PlatformCallbackHandler>;

class PlatformBridge : public AceType {
    DECLARE_ACE_TYPE(PlatformBridge, AceType);

public:
    PlatformBridge() = default;
    virtual ~PlatformBridge() = default;

    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
    {
        dispatcher_ = dispatcher;
    }

    void SendMessage(const std::vector<Framework::CodecData>& args, const PlatformCallbackHandler& handler);
    void HandleCallback(int32_t callbackId, std::vector<uint8_t>&& messageData);

private:
    WeakPtr<JsMessageDispatcher> dispatcher_;
    PlatformCallbackMap callBackHandlers_;
    std::atomic_int callbackIds_ = 1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_BRIDGE_H
