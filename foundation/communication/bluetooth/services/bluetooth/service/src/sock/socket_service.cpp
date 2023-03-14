/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "socket_service.h"
#include "class_creator.h"
#include "log.h"
#include "socket_listener.h"

namespace OHOS {
namespace bluetooth {
const std::string SPP_VERSION = "0.4.1";

SocketService::SocketService() : utility::Context(PROFILE_NAME_SPP, SPP_VERSION)
{
    LOG_INFO("ProfileService:%{public}s Create", Name().c_str());
}

SocketService::~SocketService()
{
    LOG_INFO("ProfileService:%{public}s Destroy", Name().c_str());
}

utility::Context *SocketService::GetContext()
{
    return this;
}

void SocketService::Enable()
{
    LOG_INFO("[SocketService] %{public}s", __func__);

    SocketThread::GetInstance().Initialize();
    GetDispatcher()->PostTask(std::bind(&SocketService::EnableNative, this));
}

void SocketService::EnableNative()
{
    LOG_INFO("[SocketService] %{public}s", __func__);

    GetContext()->OnEnable(PROFILE_NAME_SPP, true);
}

void SocketService::Disable()
{
    LOG_INFO("[SocketService] %{public}s", __func__);

    SocketThread::GetInstance().Uninitialize();
    GetDispatcher()->PostTask(std::bind(&SocketService::DisableNative, this));
}

void SocketService::DisableNative()
{
    LOG_INFO("[SocketService] %{public}s", __func__);

    ShutDownInternal();
}

int SocketService::Connect(const RawAddress &device)
{
    (void)device;
    return 0;
}

int SocketService::Disconnect(const RawAddress &device)
{
    (void)device;
    return 0;
}

std::list<RawAddress> SocketService::GetConnectDevices()
{
    std::list<RawAddress> devList;

    return devList;
}

int SocketService::GetConnectState()
{
    return 0;
}

int SocketService::GetMaxConnectNum()
{
    return 0;
}

int SocketService::Connect(const std::string &addr, const Uuid &uuid, int securityFlag, int type)
{
    LOG_INFO("[SocketService]%{public}s", __func__);

    int socketFd = SOCK_INVALID_FD;
    auto socket = std::make_unique<Socket>();
    if (socket->Connect(addr, uuid, securityFlag, socketFd) < 0) {
        socketFd = SOCK_INVALID_FD;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        clientSockets_.push_back(std::move(socket));
        isClientPush_ = true;
    }

    return socketFd;
}

int SocketService::Listen(const std::string &name, const Uuid &uuid, int securityFlag, int type)
{
    LOG_INFO("[SocketService]%{public}s", __func__);

    int socketFd = SOCK_INVALID_FD;
    auto socket = std::make_unique<Socket>();
    if (socket->Listen(name, uuid, securityFlag, socketFd) < 0) {
        socketFd = SOCK_INVALID_FD;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        serverSockets_.push_back(std::move(socket));
    }

    return socketFd;
}

void SocketService::ShutDownInternal()
{
    LOG_INFO("[SocketService]%{public}s", __func__);

    isShutdown_ = true;
    if (clientSockets_.empty() && acceptSockets_.empty()) {
        if (!serverSockets_.empty()) {
            CleanupServerSocket();
        }
        LOG_INFO("[SocketService]%{public}s shutdown end", __func__);
        Socket::ClearUpAllSocket();
        GetContext()->OnDisable(PROFILE_NAME_SPP, true);
        isShutdown_ = false;
        return;
    }
    if (!clientSockets_.empty()) {
        LOG_INFO("[SocketService]%{public}s close client socket start", __func__);
        auto cIt = clientSockets_.begin();
        while (cIt != clientSockets_.end()) {
            uint16_t totalClient = clientSockets_.size();
            (*cIt)->CloseSocket(false);
            if (clientSockets_.size() < totalClient) {
                continue;
            }
            cIt++;
        }
    }

    if (!acceptSockets_.empty()) {
        LOG_INFO("[SocketService]%{public}s close accpet socket start", __func__);
        auto aIt = acceptSockets_.begin();
        while (aIt != acceptSockets_.end()) {
            uint16_t totalAccept = acceptSockets_.size();
            (*aIt)->CloseSocket(false);
            if (acceptSockets_.size() < totalAccept) {
                continue;
            }
            aIt++;
        }
    }
}

void SocketService::ProcessMessage(const utility::Message &msg)
{
    Socket *socket = static_cast<Socket *>(msg.arg2_);
    std::lock_guard<std::mutex> lock(mutex_);
    switch (msg.what_) {
        case SOCKET_SDP_DISCOVERY_RESULT:
            if (clientSockets_.empty() && isClientPush_) {
                isClientPush_ = false;
                break;
            }
            if ((msg.arg1_ <= 0) || (msg.arg1_ > SOCK_MAX_SERVER)) {
                LOG_ERROR("[SocketService]%{public}s scn invalid", __func__);
                SocketThread::GetInstance().DeleteSocket(*socket);
                socket->CloseSocketFd();
                break;
            }
            socket->ReceiveSdpResult(msg.arg1_);
            break;
        case SOCKET_ACCEPT_NEW:
            LOG_INFO("[SocketService]%{public}s SOCKET_ACCEPT_NEW", __func__);
            acceptSockets_.push_back(socket);
            break;
        case SOCKET_CLOSE:
            LOG_INFO("[SocketService]%{public}s SOCKET_CLOSE", __func__);
            CleanupClientSocket(socket);
            break;
        default:
            break;
    }
}

void SocketService::CleanupServerSocket()
{
    LOG_INFO("[SocketService]%{public}s close server socket end", __func__);

    for (auto &it : serverSockets_) {
        it->RemoveServerSocket();
    }

    serverSockets_.clear();
}

void SocketService::CleanupClientSocket(Socket *socket)
{
    LOG_INFO("[SocketService]%{public}s", __func__);

    std::vector<std::unique_ptr<Socket>>::iterator clientIt;
    for (clientIt = clientSockets_.begin(); clientIt != clientSockets_.end(); ++clientIt) {
        if (clientIt->get() == socket) {
            LOG_INFO("[SocketService]%{public}s close client socket end", __func__);
            clientSockets_.erase(clientIt);
            break;
        }
    }

    std::vector<Socket *>::iterator acceptIt;
    for (acceptIt = acceptSockets_.begin(); acceptIt != acceptSockets_.end(); ++acceptIt) {
        if (*acceptIt == socket) {
            LOG_INFO("[SocketService]%{public}s close accept socket end", __func__);
            acceptSockets_.erase(acceptIt);
            break;
        }
    }

    if (clientSockets_.empty()) {
        isClientPush_ = false;
    }

    if (isShutdown_) {
        if (clientSockets_.empty() && acceptSockets_.empty()) {
            LOG_INFO("[SocketService]%{public}s shutdown end", __func__);
            CleanupServerSocket();
            Socket::ClearUpAllSocket();
            GetContext()->OnDisable(PROFILE_NAME_SPP, true);
            isShutdown_ = false;
        }
    }
}
REGISTER_CLASS_CREATOR(SocketService);
}  // namespace bluetooth
}  // namespace OHOS