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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_ARK_DEBUGGER_INSPECTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_ARK_DEBUGGER_INSPECTOR_H

#include <string>
#include "frameworks/bridge/js_frontend/engine/jsi/debugger/ws_server.h"

namespace panda::ecmascript {
class EcmaVM;
}  // namespace panda::ecmascript

namespace OHOS::Ace::Framework {
using EcmaVM = panda::ecmascript::EcmaVM;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

bool StartDebug(const std::string& componentName, void *vm, bool isDebugMode);

void StopDebug(const std::string& componentName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

class Inspector {
public:
    Inspector() = default;
    ~Inspector() {}

    pthread_t tid_;
    std::unique_ptr<WsServer> websocketServer_;
    static constexpr int DEBUGGER_WAIT_SLEEP_TIME = 100;
    volatile bool waitingForDebugger_ = true;
    volatile bool isDispatchingMsg_ = false;

};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_ARK_DEBUGGER_INSPECTOR_H