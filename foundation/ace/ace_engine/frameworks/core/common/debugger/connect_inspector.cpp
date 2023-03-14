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

#include "frameworks/core/common/debugger/connect_inspector.h"
#include "base/log/log.h"
#include "frameworks/core/common/debugger/connect_server.h"
namespace OHOS::Ace {
std::unique_ptr<ConnectServer> g_connectServer = nullptr;
void* g_debugger = nullptr;
std::map<int32_t, std::string> g_infoBuffer;
bool g_isAttachStart = false;
int g_ideWaitTime = 100;

void* HandleDebugManager(void* const server)
{
    LOGI("HandleDebugManager");
    if (server == nullptr) {
        LOGE("HandleDebugManager server nullptr");
        return nullptr;
    }
    static_cast<ConnectServer*>(server)->RunServer();
    return nullptr;
}

void* HandleHDC(void* const server)
{
    LOGI("HandleHDC");
    if (server == nullptr) {
        LOGE("HandleHDC server nullptr");
        return nullptr;
    }
    int32_t pid = getpid();
    static_cast<ConnectServer*>(server)->Register(pid);
    return nullptr;
}

void OnMessage(const std::string& message)
{
    if (message.empty()) {
        LOGE("message is empty");
        return;
    }
    LOGI("message will be sent");

    std::string checkMessage = "connected";
    if (message.find(checkMessage, 0) != std::string::npos) {
        LOGI("Find the targeted string: %{private}s", message.c_str());
        if (g_connectServer != nullptr) {
            g_connectServer->waitingForDebugger_ = false;
        }
    }

    checkMessage = "attachStart";
    if (message.find(checkMessage, 0) != std::string::npos) {
        LOGI("Find the attach command: %{private}s", message.c_str());
        g_isAttachStart = true;
        if (g_connectServer != nullptr) {
            for (auto it = g_infoBuffer.begin(); it != g_infoBuffer.end(); it++) {
                g_connectServer->SendMessage(it->second);
            }
        }
    }
}

void ResetService()
{
    if (g_connectServer != nullptr) {
        g_connectServer->StopServer();
        g_connectServer.reset();
    }
}

void StartServer(const std::string& componentName, const bool flagNeedDebugBreakPoint)
{
    LOGI("StartServer: %{private}s", componentName.c_str());
    g_connectServer = std::make_unique<ConnectServer>(componentName, std::bind(&OnMessage, std::placeholders::_1),
                      flagNeedDebugBreakPoint);
    pthread_t tid;
    if (pthread_create(&tid, nullptr, &HandleDebugManager, static_cast<void*>(g_connectServer.get())) != 0) {
        LOGE("pthread_create fail!");
        ResetService();
        return;
    }

    while (g_connectServer->waitingForDebugger_) {
        usleep(g_ideWaitTime);
    }
    LOGI("StartServer connected.");
}

void StartUnixSocket(const std::string& componentName)
{
    LOGI("StartUnixSocket: %{private}s", componentName.c_str());
    while (g_connectServer == nullptr) {
        LOGI("Waiting for the Connect Server");
        usleep(g_ideWaitTime);
    }
    LOGI("pid is %{private}d", getpid());
    pthread_t tid;
    if (pthread_create(&tid, nullptr, &HandleHDC, static_cast<void*>(g_connectServer.get())) != 0) {
        LOGE("pthread_create fail!");
        return;
    }
}

void StopServer(const std::string& componentName)
{
    LOGI("StopServer: %{private}s", componentName.c_str());
    if (g_connectServer != nullptr) {
        g_connectServer->StopServer();
        g_connectServer.reset();
    }
    LOGI("StopServer end");
}

void AddMessage(const int32_t instanceId, const std::string& message)
{
    LOGI("Add message to information buffer.");
    if (g_infoBuffer.count(instanceId) == 1) {
        LOGE("The message with the current instance id has been existed.");
        return;
    }
    g_infoBuffer[instanceId] = message;
}

void RemoveMessage(const int32_t instanceId)
{
    LOGI("Remove message from information buffer.");
    if (g_infoBuffer.count(instanceId) != 1) {
        LOGE("The message with the current instance id does not exist.");
        return;
    }
    g_infoBuffer.erase(instanceId);
}

void SendMessage(const std::string& message)
{
    LOGI("Enter SendMessage");
    if (g_connectServer != nullptr) {
        g_connectServer->SendMessage(message);
    }
}

bool IsAttachStart()
{
    return g_isAttachStart;
}

} // namespace OHOS::Ace
