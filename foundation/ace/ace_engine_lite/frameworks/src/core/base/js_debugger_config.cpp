/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "js_debugger_config.h"
#include <cstring>
#include "ace_log.h"
#include "debugger.h"
#include "handler.h"
#include "jerryscript-port-default.h"
#include "jerryscript-port.h"
#include "js_fwk_common.h"

namespace OHOS {
namespace ACELite {
Debugger &Debugger::GetInstance()
{
    static Debugger instance;
    return instance;
}

#if IS_ENABLED(ENGINE_DEBUGGER)
bool Debugger::IsDebuggerEnabled()
{
    return debuggerConfig_.startDebuggerServer;
}

void Debugger::ConfigEngineDebugger(DebuggerConfig &config)
{
    debuggerConfig_ = config;
}

void Debugger::StartDebugger()
{
    if ((!debuggerConfig_.startDebuggerServer) || !(jerry_is_feature_enabled(JERRY_FEATURE_DEBUGGER))) {
        return;
    }

    if (debuggerStarted_) {
        // already started, avoid creating tcp repeat
        return;
    }
    // increase jerry log level to see the Waiting for client connection message
    jerry_port_default_set_log_level(JERRY_LOG_LEVEL_DEBUG);
    // currently, only support tcp::websocket
    HILOG_INFO(HILOG_MODULE_ACE, "please connect to debugger server with localhost:%{public}d", debuggerConfig_.port);
    bool protocol = jerryx_debugger_tcp_create(debuggerConfig_.port);
    bool connected = jerryx_debugger_ws_create();
    jerryx_debugger_after_connect(protocol && connected);
    debuggerStarted_ = true;
}

void Debugger::TearDownDebugger()
{
    if ((!debuggerConfig_.startDebuggerServer) || !(jerry_is_feature_enabled(JERRY_FEATURE_DEBUGGER))) {
        return;
    }
    // no need to check debuggerStarted_ flag, as the lower interface is reenterable
    jerry_debugger_stop();
    debuggerStarted_ = false;
}

void Debugger::Output(const char * const str)
{
    // print out the log into stdout anyway
    printf("%s", str);
}

void Debugger::FlushOutput()
{
    fflush(stdout);
}

#if (JS_ENGINE_EXTERNAL_CONTEXT == 1)
static void *AllocEngineContext(size_t size, void *cbDataP)
{
    (void)(cbDataP);
    return ace_malloc(size);
}
#endif // JS_ENGINE_EXTERNAL_CONTEXT

void Debugger::SetupJSContext()
{
#if (JS_ENGINE_EXTERNAL_CONTEXT == 1)
    if (engineContext_ != nullptr) {
        // do not repeat the setup process
        return;
    }
    uint32_t heapSize = debuggerConfig_.heapSize;
    if (heapSize == 0) {
        // default is 48KB
        const uint32_t DEFAULT_HEAP_SIZE = 48 * 1024;
        heapSize = DEFAULT_HEAP_SIZE;
    }
    engineContext_ = jerry_create_context(heapSize, AllocEngineContext, nullptr);
    if (engineContext_ == nullptr) {
        return;
    }
    jerry_port_default_set_current_context(static_cast<jerry_context_t *>(engineContext_));
#endif // JS_ENGINE_EXTERNAL_CONTEXT
}

void Debugger::ReleaseJSContext()
{
#if (JS_ENGINE_EXTERNAL_CONTEXT == 1)
    if (engineContext_ == nullptr) {
        return;
    }
    ace_free(engineContext_);
    engineContext_ = nullptr;
#endif // JS_ENGINE_EXTERNAL_CONTEXT
}
#else  // ENABLED(ENGINE_DEBUGGER)
bool Debugger::IsDebuggerEnabled()
{
    return true;
}

void Debugger::ConfigEngineDebugger(DebuggerConfig &config)
{
    UNUSED(config);
}

void Debugger::StartDebugger() {}

void Debugger::TearDownDebugger() {}

void Debugger::Output(const char * const str)
{
    UNUSED(str);
}

void Debugger::FlushOutput() {}

void Debugger::SetupJSContext() {}

void Debugger::ReleaseJSContext() {}
#endif // ENABLED(ENGINE_DEBUGGER)
} // namespace ACELite
} // namespace OHOS
