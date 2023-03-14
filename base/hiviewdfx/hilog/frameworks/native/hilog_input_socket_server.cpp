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

#include "hilog_input_socket_server.h"
#include <sys/prctl.h>
#include <thread>

namespace OHOS {
namespace HiviewDFX {
HilogInputSocketServer::~HilogInputSocketServer()
{
    StopServingThread();
}

HilogInputSocketServer::ServerThreadState HilogInputSocketServer::RunServingThread()
{
    if (m_serverThread.get_id() != std::thread().get_id()) {
        return ServerThreadState::ALREADY_STARTED;
    }
    m_stopServer.store(false);
    m_serverThread = std::thread(&HilogInputSocketServer::ServingThread, this);
    if (m_serverThread.get_id() != std::thread().get_id()) {
        return ServerThreadState::JUST_STARTED;
    }
    return ServerThreadState::CAN_NOT_START;
}

void HilogInputSocketServer::StopServingThread()
{
    if (m_serverThread.get_id() == std::thread().get_id()) {
        return;
    }
    std::thread tmp;
    std::swap(m_serverThread, tmp);
    if (tmp.joinable()) {
        m_stopServer.store(true);
        tmp.join();
    }
}

void HilogInputSocketServer::ServingThread()
{
    prctl(PR_SET_NAME, "hilogd.server");
    int ret;
    std::vector<char> data;
#ifndef __RECV_MSG_WITH_UCRED_
    while ((ret = RecvPacket(data)) >= 0) {
        if (ret > 0) {
            m_packetHandler(data);
        }
        if (m_stopServer.load()) {
            break;
        }
    }
#else
    ucred cred;
    while ((ret = RecvPacket(data, &cred)) >= 0) {
        if (ret > 0) {
            m_packetHandler(cred, data);
        }
        if (m_stopServer.load()) {
            break;
        }
    }
#endif
}
} // namespace HiviewDFX
} // namespace OHOS
