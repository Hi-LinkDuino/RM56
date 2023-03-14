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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_V8_INSPECTOR_CLIENT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_V8_INSPECTOR_CLIENT_H

#include <iostream>

#include "third_party/v8/include/libplatform/libplatform.h"
#include "third_party/v8/include/v8-inspector.h"
#include "third_party/v8/include/v8.h"

#include "frameworks/bridge/js_frontend/engine/v8/debugger/utils.h"
#include "frameworks/bridge/js_frontend/engine/v8/debugger/v8_inspector_channel.h"

namespace V8Debugger {

void DispatchMsgToV8(int sign);

class V8InspectorClient final : public v8_inspector::V8InspectorClient {
public:
    V8InspectorClient() = default;
    ~V8InspectorClient() {}
    void InitializeClient(const std::unique_ptr<v8::Platform>& platform, const v8::Local<v8::Context>& context,
        const std::function<void(std::string)>& onResponse);
    void DispatchProtocolMessage(const v8_inspector::StringView& messageView) const;
    void runMessageLoopOnPause(int contextGroupId) override;
    void quitMessageLoopOnPause() override;
    void SchedulePauseOnNextStatement(const v8_inspector::StringView& reason) const;
    void WaitFrontendMessageOnPause();
    uint8_t GetPausedFlag() const;

private:
    v8::Local<v8::Context> ensureDefaultContextInGroup(int contextGroupId) override;
    std::function<int(void)> pauseAndReadMsg;
    v8::Platform* v8Platform;
    std::unique_ptr<v8_inspector::V8Inspector> v8Inspector;
    std::unique_ptr<v8_inspector::V8InspectorSession> v8Session;
    std::unique_ptr<V8InspectorChannel> v8Channel;
    v8::Isolate* v8Isolate;
    static constexpr int CONTEXT_GROUP_ID = 1;
    static constexpr int SAVE_LOCATION = 4;
    uint8_t terminatedFlag = 1;
    uint8_t runNestedLoop = 0;
};

} // namespace V8Debugger

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_V8_INSPECTOR_CLIENT_H
