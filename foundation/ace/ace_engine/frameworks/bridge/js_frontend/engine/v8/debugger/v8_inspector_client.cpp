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

#include "frameworks/bridge/js_frontend/engine/v8/debugger/v8_inspector_client.h"

#include "frameworks/bridge/js_frontend/engine/v8/debugger/ws_server.h"

namespace V8Debugger {

void V8InspectorClient::InitializeClient(const std::unique_ptr<v8::Platform>& platform,
    const v8::Local<v8::Context>& context, const std::function<void(std::string)>& onResponse)
{
    v8Platform = platform.get();
    v8Isolate = context->GetIsolate();
    v8Channel.reset(new V8InspectorChannel(onResponse));
    v8Inspector = v8_inspector::V8Inspector::create(v8Isolate, this);
    v8Session = v8Inspector->connect(CONTEXT_GROUP_ID, v8Channel.get(), v8_inspector::StringView());
    context->SetAlignedPointerInEmbedderData(SAVE_LOCATION, this);
    v8_inspector::StringView contextName = ConvertToStringView("inspector");
    v8Inspector->contextCreated(v8_inspector::V8ContextInfo(context, CONTEXT_GROUP_ID, contextName));
    terminatedFlag = true;
    runNestedLoop = false;
}

void V8InspectorClient::DispatchProtocolMessage(const v8_inspector::StringView& messageView) const
{
    LOGI("DispatchProtocolMessage");
    if (v8Session == nullptr) {
        LOGE("v8Session=nullptr!");
        return;
    }
    v8Session->dispatchProtocolMessage(messageView);
}

void V8InspectorClient::runMessageLoopOnPause(int contextGroupId)
{
    terminatedFlag = false;
    LOGI("runMessageLoopOnPause !,%d", terminatedFlag);
    if (v8Channel == nullptr) {
        LOGE("v8Channel = nullptr");
        return;
    }
    LOGI("pause on loop!, %d", terminatedFlag);
    if (runNestedLoop) {
        LOGE("pause on loop runNestedLoop!, %d", terminatedFlag);
        return;
    }
    runNestedLoop = true;
    while (!terminatedFlag) {
        DispatchMsgToV8(0);
        if (v8Platform == nullptr || v8Isolate == nullptr) {
            LOGE("v8Platform == nullptr || v8Isolate == nullptr !");
            return;
        }
        while (v8::platform::PumpMessageLoop(v8Platform, v8Isolate)) {
            LOGI("pause on msgloop !, %d", terminatedFlag);
        }
    }
    LOGI("pause on msgloop out !,%d", terminatedFlag);
    terminatedFlag = true;
    runNestedLoop = false;
}

void V8InspectorClient::quitMessageLoopOnPause()
{
    LOGI("quit Message LoopOnPause !");
    terminatedFlag = true;
}

v8::Local<v8::Context> V8InspectorClient::ensureDefaultContextInGroup(int contextGroupId)
{
    return v8::Local<v8::Context>();
}

void V8InspectorClient::SchedulePauseOnNextStatement(const v8_inspector::StringView& reason) const
{
    if (v8Session == nullptr) {
        LOGE("v8Session=nullptr!");
        return;
    }
    v8Session->schedulePauseOnNextStatement(reason, reason);
}

void V8InspectorClient::WaitFrontendMessageOnPause()
{
    LOGI("wait Frontend Message OnPause !");
    terminatedFlag = false;
}

uint8_t V8InspectorClient::GetPausedFlag() const
{
    return terminatedFlag;
}

} // namespace V8Debugger