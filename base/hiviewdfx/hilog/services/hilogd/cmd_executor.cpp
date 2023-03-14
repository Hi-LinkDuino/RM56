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
#include "cmd_executor.h"
#include "service_controller.h"

#include <seq_packet_socket_server.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <poll.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace OHOS {
namespace HiviewDFX {
static const int MAX_CLIENT_CONNECTIONS = 100;

CmdExecutor::~CmdExecutor()
{
    std::lock_guard<std::mutex> lg(m_clientAccess);
    for (auto& client : m_clients) {
        client->m_stopThread.store(true);
    }
    for (auto& client : m_clients) {
        if (client->m_clientThread.joinable()) {
            client->m_clientThread.join();
        }
    }
}

void CmdExecutor::MainLoop()
{
    SeqPacketSocketServer cmdServer(CONTROL_SOCKET_NAME, MAX_CLIENT_CONNECTIONS);
    if (cmdServer.Init() < 0) {
        std::cerr << "Failed to init control socket ! \n";
        return;
    }
    std::cout << "Begin to cmd accept !\n";
    int listeningStatus = cmdServer.Listen(MAX_CLIENT_CONNECTIONS);
    if (listeningStatus < 0) {
        std::cerr << "Socket listen failed: ";
        HilogPrintError(listeningStatus);
        return;
    }
    std::cout << "Server started to listen !\n";

    using namespace std::chrono_literals;
    for (;;) {
        const auto maxtime = 3000ms;
        short outEvent = 0;
        auto pollResult = cmdServer.Poll(POLLIN, outEvent, maxtime);
        if (pollResult == 0) { // poll == 0 means timeout
            CleanFinishedClients();
            continue;
        } else if (pollResult < 0) {
            std::cerr << "Socket polling error: ";
            HilogPrintError(errno);
            break;
        } else if (pollResult != 1 || outEvent != POLLIN) {
            std::cerr << "Wrong poll result data."
                         " Result: " << pollResult <<
                         " OutEvent: " << outEvent << "\n";
            break;
        }

        int acceptResult = cmdServer.Accept();
        if (acceptResult > 0) {
            int acceptedSockedFd = acceptResult;
            std::unique_ptr<Socket> handler = std::make_unique<Socket>(SOCK_SEQPACKET);
            if (handler != nullptr) {
                handler->setHandler(acceptedSockedFd);
                OnAcceptedConnection(std::move(handler));
            }
        } else {
            std::cerr << "Socket accept failed: ";
            HilogPrintError(errno);
            break;
        }
    }
}

void CmdExecutor::OnAcceptedConnection(std::unique_ptr<Socket> handler)
{
    std::lock_guard<std::mutex> lg(m_clientAccess);
    auto newVal = std::make_unique<ClientThread>();
    if (newVal != nullptr) {
        newVal->m_stopThread.store(false);
        newVal->m_clientThread = std::thread(&CmdExecutor::ClientEventLoop, this, std::move(handler));
        m_clients.push_back(std::move(newVal));
    }
}

void CmdExecutor::ClientEventLoop(std::unique_ptr<Socket> handler)
{
    decltype(m_clients)::iterator clientInfoIt;
    {
        std::lock_guard<std::mutex> lg(m_clientAccess);
        clientInfoIt = std::find_if(m_clients.begin(), m_clients.end(),
            [](const std::unique_ptr<ClientThread>& ct) {
                return ct->m_clientThread.get_id() == std::this_thread::get_id();
            });
    }
    if (clientInfoIt == m_clients.end()) {
        std::cerr << "Failed to find client\n";
        return;
    }

    prctl(PR_SET_NAME, "hilogd.query");
    ServiceController serviceCtrl(std::move(handler), m_hilogBuffer);
    serviceCtrl.CommunicationLoop((*clientInfoIt)->m_stopThread);

    std::lock_guard<std::mutex> ul(m_finishedClientAccess);
    m_finishedClients.push_back(std::this_thread::get_id());
}

void CmdExecutor::CleanFinishedClients()
{
    std::list<std::thread> threadsToJoin;
    {
        // select clients to clean up - pick threads that we have to be sure are ended
        std::scoped_lock sl(m_finishedClientAccess, m_clientAccess);
        for (auto threadId : m_finishedClients) {
            auto clientInfoIt = std::find_if(m_clients.begin(), m_clients.end(),
                [&threadId](const std::unique_ptr<ClientThread>& ct) {
                    return ct->m_clientThread.get_id() == threadId;
                });
            if (clientInfoIt != m_clients.end()) {
                threadsToJoin.push_back(std::move((*clientInfoIt)->m_clientThread));
                m_clients.erase(clientInfoIt);
            }
        }
        m_finishedClients.clear();
    }
    for (auto& thread : threadsToJoin) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS
