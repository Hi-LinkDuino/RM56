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

#include "frameworks/bridge/js_frontend/engine/jsi/debugger/inspector.h"

#include <dlfcn.h>

#include "base/log/log.h"
#include "frameworks/bridge/js_frontend/engine/jsi/debugger/ws_server.h"

namespace OHOS::Ace::Framework {
namespace {
std::unique_ptr<Inspector> g_inspector = nullptr;
void* g_handle = nullptr;
constexpr char ARK_DEBUGGER_SHARED_LIB[] = "libark_ecma_debugger.so";

void* HandleClient(void* const server)
{
    LOGI("HandleClient");
    if (server == nullptr) {
        LOGE("HandleClient server nullptr");
        return nullptr;
    }
    static_cast<WsServer*>(server)->RunServer();
    return nullptr;
}

void* GetArkDynFunction(const char* symbol)
{
    if (g_handle == nullptr) {
        LOGE("Failed to open shared library %{public}s, reason: %{public}sn", ARK_DEBUGGER_SHARED_LIB, dlerror());
        return nullptr;
    }

    auto function = dlsym(g_handle, symbol);
    if (function == nullptr) {
        LOGE("Failed to get symbol %{public}s in %{public}s", symbol, ARK_DEBUGGER_SHARED_LIB);
    }
    return function;
}

void DispatchMsgToArk(int sign)
{
    if (g_inspector->isDispatchingMsg_) {
        return;
    }
    auto processMsg = reinterpret_cast<void (*)(const std::string &)>(GetArkDynFunction("DispatchProtocolMessage"));
    if (processMsg == nullptr) {
        LOGE("processMessage is empty");
        return;
    }
    g_inspector->isDispatchingMsg_ = true;
    while (!g_inspector->websocketServer_->ideMsgQueue.empty()) {
        const std::string message = g_inspector->websocketServer_->ideMsgQueue.front();
        g_inspector->websocketServer_->ideMsgQueue.pop();
        processMsg(message);
        std::string startDebugging("Runtime.runIfWaitingForDebugger");
        if (message.find(startDebugging, 0) != std::string::npos) {
            g_inspector->waitingForDebugger_ = false;
        }
    }
    g_inspector->isDispatchingMsg_ = false;
}

void OnMessage()
{
    pthread_kill(g_inspector->tid_, SIGALRM);
    return;
}

void SendReply(const std::string& message)
{
    if (g_inspector->websocketServer_ != nullptr) {
        g_inspector->websocketServer_->SendReply(message);
    }
}

void ResetService()
{
    if (g_handle != nullptr) {
        dlclose(g_handle);
        g_handle = nullptr;
    }
    if (g_inspector->websocketServer_ != nullptr) {
        g_inspector->websocketServer_->StopServer();
        g_inspector.reset();
    }
}

} // namespace

bool StartDebug(const std::string& componentName, void *vm, bool isDebugMode)
{
    LOGI("StartDebug: %{private}s", componentName.c_str());
    g_inspector = std::make_unique<Inspector>();
    g_inspector->websocketServer_ = std::make_unique<WsServer>(componentName, std::bind(&OnMessage));
    g_inspector->tid_ = pthread_self();
    g_inspector->waitingForDebugger_ = isDebugMode;

    g_handle = dlopen(ARK_DEBUGGER_SHARED_LIB, RTLD_LAZY);
    if (g_handle == nullptr) {
        LOGE("handle is empty");
        return false;
    }
    auto initialize = reinterpret_cast<void (*)(const std::function<void(std::string)> &, void *)>(
        GetArkDynFunction("InitializeDebugger"));
    if (initialize == nullptr) {
        ResetService();
        return false;
    }
    initialize(std::bind(&SendReply, std::placeholders::_1), vm);

    pthread_t tid;
    if (pthread_create(&tid, nullptr, &HandleClient, static_cast<void*>(g_inspector->websocketServer_.get())) != 0) {
        LOGE("pthread_create fail!");
        ResetService();
        return false;
    }
    signal(SIGALRM, &DispatchMsgToArk);
    while (g_inspector->waitingForDebugger_) {
        usleep(g_inspector->DEBUGGER_WAIT_SLEEP_TIME);
    }
    LOGI("StartDebug Continue");
    return true;
}

void StopDebug(const std::string& componentName)
{
    LOGI("StopDebug: %{private}s", componentName.c_str());
    if (g_inspector == nullptr) {
        return;
    }
    if (g_inspector->websocketServer_ != nullptr) {
        g_inspector->websocketServer_->StopServer();
        g_inspector.reset();
    }
    auto uninitialize = reinterpret_cast<void (*)()>(GetArkDynFunction("UninitializeDebugger"));
    if (uninitialize == nullptr) {
        return;
    }
    uninitialize();
    if (g_handle != nullptr) {
        dlclose(g_handle);
        g_handle = nullptr;
    }
    LOGI("StopDebug end");
}

} // namespace OHOS::Ace::Framework
