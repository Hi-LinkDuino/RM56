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

#ifndef CMD_EXECUTOR_H
#define CMD_EXECUTOR_H

#include <list>
#include <memory>
#include <atomic>
#include <thread>

#include <socket.h>
#include "log_buffer.h"

namespace OHOS {
namespace HiviewDFX {
struct ClientThread {
    std::thread m_clientThread;
    std::atomic<bool> m_stopThread;
};

class CmdExecutor {
public:
    explicit CmdExecutor(HilogBuffer& buffer) : m_hilogBuffer(buffer) {}
    ~CmdExecutor();
    void MainLoop();
private:
    void OnAcceptedConnection(std::unique_ptr<Socket> handler);
    void ClientEventLoop(std::unique_ptr<Socket> handler);
    void CleanFinishedClients();

    HilogBuffer& m_hilogBuffer;
    std::list<std::unique_ptr<ClientThread>> m_clients;
    std::mutex m_clientAccess;
    std::vector<std::thread::id> m_finishedClients;
    std::mutex m_finishedClientAccess;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
