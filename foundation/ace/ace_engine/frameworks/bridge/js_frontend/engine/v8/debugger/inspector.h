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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_INSPECTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_INSPECTOR_H

#include <functional>
#include <iostream>
#include <queue>
#include <vector>

#include "frameworks/bridge/js_frontend/engine/v8/debugger/utils.h"
#include "frameworks/bridge/js_frontend/engine/v8/debugger/v8_inspector_client.h"
#include "frameworks/bridge/js_frontend/engine/v8/debugger/ws_server.h"

namespace V8Debugger {

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
void StartDebug(
    const std::unique_ptr<v8::Platform>& platform,
    const v8::Local<v8::Context>& context,
    const std::string& componentName,
    const bool isDebugMode,
    const int32_t instanceId);

void WaitingForIde();

void StopDebug();
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

class Inspector {
public:
    Inspector() = default;
    ~Inspector() {}
    void InitializeInspector(const std::unique_ptr<v8::Platform>& platform, const v8::Local<v8::Context>& context,
        std::string sockName);
    void StartAgent() const;

    std::unique_ptr<V8InspectorClient> inspectorClient;
    std::unique_ptr<WsServer> websocketServer;
    static constexpr int DEBUGGER_WAIT_SLEEP_TIME = 100;
    volatile bool waitingForDebugger;
    volatile bool isDispatchingMsg;
    std::queue<const std::string> ideMsgQueue;

private:
    void OnMessage(const std::string& message);
    void SendMessage(const std::string& message) const;
};

} // namespace V8Debugger

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_INSPECTOR_H
