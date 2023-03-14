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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_V8_INSPECTOR_CHANNEL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_V8_INSPECTOR_CHANNEL_H

#include <functional>

#include "third_party/v8/include/v8-inspector.h"
#include "third_party/v8/include/v8.h"

namespace V8Debugger {

class V8InspectorChannel final : public v8_inspector::V8Inspector::Channel {
public:
    explicit V8InspectorChannel(const std::function<void(std::string)>& onResponse) : v8OnResponse(onResponse) {}
    ~V8InspectorChannel() {}
    void sendResponse(int callId, std::unique_ptr<v8_inspector::StringBuffer> message) override;
    void sendNotification(std::unique_ptr<v8_inspector::StringBuffer> message) override;
    void flushProtocolNotifications() override;

private:
    std::function<void(std::string)> v8OnResponse;
};

} // namespace V8Debugger

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_V8_INSPECTOR_CHANNEL_H
