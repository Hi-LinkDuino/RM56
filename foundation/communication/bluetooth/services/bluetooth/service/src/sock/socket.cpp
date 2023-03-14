/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "socket.h"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "adapter_config.h"
#include "log.h"
#include "packet.h"
#include "power_manager.h"
#include "profile_service_manager.h"
#include "securec.h"

#include "socket_def.h"
#include "socket_listener.h"
#include "socket_service.h"
#include "socket_util.h"

namespace OHOS {
namespace bluetooth {
static int g_arrayServiceId[SOCK_MAX_SERVICE_ID] = {0};
std::vector<Socket *> Socket::g_allServerSockets;
std::recursive_mutex Socket::g_socketMutex;

struct Socket::impl {
    class DataTransportObserverImplement;
    std::unique_ptr<DataTransportObserver> transportObserver_ {};
    void OnConnectIncomingNative(Socket &socket, RawAddress addr, uint8_t port);
    void OnConnectedNative(Socket &socket, DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU);
    void OnDisconnectedNative(Socket &socket, DataTransport *transport);
    void OnDisconnectSuccessNative(Socket &socket, DataTransport *transport);
    void OnDataAvailableNative(Socket &socket, DataTransport *transport);
    void OnTransportErrorNative(Socket &socket, DataTransport *transport, int errType);
    void SockRfcConnectFail(Socket &socket, DataTransport *transport);
    void SockRfcDisconnectFail(Socket &socket, DataTransport *transport);
    void SockRfcFcOn(Socket &socket, DataTransport *transport);
    static int GetMaxConnectionDevicesNum();
};

class Socket::impl::DataTransportObserverImplement : public DataTransportObserver {
public:
    void OnConnectIncoming(const RawAddress &addr, uint16_t port) override
    {
        SocketService *socketService =
            static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
        if (socketService != nullptr) {
            socketService->GetDispatcher()->PostTask(
                std::bind(&impl::OnConnectIncomingNative, socket_.pimpl.get(), std::ref(socket_), addr, port));
        }
    }

    void OnIncomingDisconnected(const RawAddress &addr) override
    {
        LOG_INFO("[sock]%{public}s", __func__);
    }

    void OnConnected(DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU) override
    {
        LOG_INFO("[sock]%{public}s", __func__);

        SocketService *socketService =
            static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
        if (socketService != nullptr) {
            socketService->GetDispatcher()->PostTask(std::bind(&impl::OnConnectedNative, socket_.pimpl.get(),
                                                               std::ref(socket_), transport, sendMTU, recvMTU));
        }
    }

    void OnDisconnected(DataTransport *transport) override
    {
        LOG_INFO("[sock]%{public}s", __func__);

        SocketService *socketService =
            static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
        if (socketService != nullptr) {
            socketService->GetDispatcher()->PostTask(
                std::bind(&impl::OnDisconnectedNative, socket_.pimpl.get(), std::ref(socket_), transport));
        }
    }

    void OnDisconnectSuccess(DataTransport *transport) override
    {
        LOG_INFO("[sock]%{public}s", __func__);

        SocketService *socketService =
            static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
        if (socketService != nullptr) {
            socketService->GetDispatcher()->PostTask(
                std::bind(&impl::OnDisconnectSuccessNative, socket_.pimpl.get(), std::ref(socket_), transport));
        }
    }

    void OnDataAvailable(DataTransport *transport) override
    {
        LOG_INFO("[sock]%{public}s", __func__);

        SocketService *socketService =
            static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
        if (socketService != nullptr) {
            socketService->GetDispatcher()->PostTask(
                std::bind(&impl::OnDataAvailableNative, socket_.pimpl.get(), std::ref(socket_), transport));
        }
    }

    void OnDataAvailable(DataTransport *transport, Packet *pkt) override
    {
        LOG_INFO("[sock]%{public}s", __func__);
    }

    void OnTransportError(DataTransport *transport, int errType) override
    {
        LOG_INFO("[sock]%{public}s", __func__);

        SocketService *socketService =
            static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
        if (socketService != nullptr) {
            socketService->GetDispatcher()->PostTask(
                std::bind(&impl::OnTransportErrorNative, socket_.pimpl.get(), std::ref(socket_), transport, errType));
        }
    }

    DataTransportObserverImplement(Socket &socket) : socket_(socket)
    {}

    ~DataTransportObserverImplement() = default;

private:
    Socket &socket_;
};

void Socket::impl::OnConnectIncomingNative(Socket &socket, RawAddress addr, uint8_t port)
{
    LOG_INFO("[sock]%{public}s", __func__);

    socket.maxConnectedNum_ = GetMaxConnectionDevicesNum();
    addr.ConvertToUint8(socket.remoteAddr_.addr);
    socket.remoteAddr_.type = BT_PUBLIC_DEVICE_ADDRESS;
    if (socket.clientNumber_ < socket.maxConnectedNum_) {
        socket.sockTransport_->AcceptConnection(addr, port);
    } else {
        socket.sockTransport_->RejectConnection(addr, port);
    }
}

void Socket::impl::OnConnectedNative(Socket &socket, DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU)
{
    LOG_INFO("[sock]%{public}s", __func__);
    IPowerManager::GetInstance().StatusUpdate(
        RequestStatus::CONNECT_ON, PROFILE_NAME_SPP, RawAddress::ConvertToString(socket.remoteAddr_.addr));
    if (socket.IsServer()) {
        socket.clientNumber_++;
        int newFd = socket.AddSocketInternal(socket.remoteAddr_, transport, sendMTU, recvMTU);
        Socket::SendAppConnectInfo(socket.transportFd_, socket.remoteAddr_, true, newFd);
    } else {
        socket.state_ = CONNECTED;
        socket.sendMTU_ = sendMTU;
        socket.recvMTU_ = recvMTU;
        Socket::SendAppConnectInfo(socket.transportFd_, socket.remoteAddr_, true, -1);
        LOG_INFO("[sock]%{public}s app fd:%{public}d client connect successfully", __func__, socket.upperlayerFd_);
        std::lock_guard<std::recursive_mutex> lck(Socket::g_socketMutex);
        g_allServerSockets.push_back(&socket);
    }
}

void Socket::impl::OnDisconnectedNative(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    socket.ProcessDisconnection(socket, transport);
}

void Socket::impl::OnDisconnectSuccessNative(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    socket.ProcessDisconnection(socket, transport);
}

void Socket::impl::OnDataAvailableNative(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    Packet *pkt = nullptr;
    uint8_t *pData = nullptr;
    Buffer *buf = nullptr;

    Socket *socketTmp = nullptr;
    if (socket.IsServer()) {
        if (socket.socketMap_.find(transport) != socket.socketMap_.end()) {
            socketTmp = socket.socketMap_.at(transport).get();
        } else {
            LOG_ERROR("[sock]%{public}s socket does not exist", __func__);
            return;
        }
    } else {
        socketTmp = &socket;
    }

    if (!socketTmp->isCanRead_) {
        LOG_DEBUG("[sock]%{public}s app can not receive data", __func__);
        return;
    }

    if (socketTmp->isNewSocket_) {
        if (socketTmp->newSockTransport_ != nullptr) {
            socketTmp->newSockTransport_->Read(&pkt);
        } else {
            LOG_DEBUG("[sock]%{public}s newSockTransport is null", __func__);
            return;
        }
    } else {
        socketTmp->sockTransport_->Read(&pkt);
    }

    if (pkt == nullptr) {
        LOG_ERROR("[sock]%{public}s pkt is null", __func__);
        return;
    }

    size_t len = PacketPayloadSize(pkt);
    buf = PacketContinuousPayload(pkt);
    if (buf == nullptr) {
        LOG_ERROR("[sock]%{public}s pkt buf is null", __func__);
        return;
    }
    pData = (uint8_t *)BufferPtr(buf);

    socketTmp->WriteDataToAPP(pData, len);

    if (pkt != nullptr) {
        PacketFree(pkt);
    }
}

void Socket::impl::OnTransportErrorNative(Socket &socket, DataTransport *transport, int errType)
{
    LOG_INFO("[sock]%{public}s errType:%{public}d", __func__, errType);

    switch (errType) {
        case RFCOMM_CONNECT_FAIL:
            SockRfcConnectFail(socket, transport);
            break;
        case RFCOMM_DISCONNECT_FAIL:
            SockRfcDisconnectFail(socket, transport);
            break;
        case RFCOMM_EV_FC_ON:
            SockRfcFcOn(socket, transport);
            break;
        default:
            break;
    }
}

void Socket::impl::SockRfcConnectFail(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    if (socket.IsServer()) {
        if (socket.socketMap_.find(transport) != socket.socketMap_.end()) {
            Socket *serverSocket = nullptr;
            serverSocket = socket.socketMap_.at(transport).get();
            Socket::SendAppConnectInfo(serverSocket->transportFd_, socket.remoteAddr_, false, -1);
        }
    } else {
        Socket::SendAppConnectInfo(socket.transportFd_, socket.remoteAddr_, false, -1);
    }
    socket.ProcessDisconnection(socket, transport);
}

void Socket::impl::SockRfcDisconnectFail(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    if (socket.IsServer()) {
        if (socket.socketMap_.find(transport) != socket.socketMap_.end()) {
            LOG_DEBUG("SockRfcDisconnectFail closefd : fd:%{public}d", socket.socketMap_.at(transport).get()->transportFd_);
            close(socket.socketMap_.at(transport).get()->transportFd_);
        } else {
            LOG_ERROR("[sock]socket does not exist");
        }
    } else {
        LOG_DEBUG("SockRfcDisconnectFail closefd : fd:%{public}d", socket.transportFd_);
        close(socket.transportFd_);
    }
}

void Socket::impl::SockRfcFcOn(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    Socket *socketTmp = nullptr;
    if (socket.IsServer()) {
        if (socket.socketMap_.find(transport) != socket.socketMap_.end()) {
            socketTmp = socket.socketMap_.at(transport).get();
        } else {
            LOG_ERROR("socket does not exist");
        }
    } else {
        socketTmp = &socket;
    }
    if (socketTmp == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lk(socketTmp->writeMutex_);
    if (socketTmp->sendBufLen_ > 0) {
        Packet *wPkt = PacketMalloc(0, 0, socketTmp->sendBufLen_);
        Buffer *wPayloadBuf = PacketContinuousPayload(wPkt);
        void *buffer = BufferPtr(wPayloadBuf);
        (void)memcpy_s(buffer, socketTmp->sendBufLen_, socketTmp->sendDataBuf_, socketTmp->sendBufLen_);
        if (wPayloadBuf == nullptr) {
            if (wPkt != nullptr) {
                PacketFree(wPkt);
            }
            return;
        }
        int ret = socketTmp->TransportWrite(wPkt);
        if (ret < 0) {
            LOG_ERROR("%{public}s stack write failed", __func__);
        } else {
            (void)memset_s(socketTmp->sendDataBuf_, socketTmp->sendBufLen_, 0x00, socketTmp->sendBufLen_);
            socketTmp->sendBufLen_ = 0;
            socketTmp->isCanWrite_ = true;
            socketTmp->WriteData();
        }
        if (wPkt != nullptr) {
            PacketFree(wPkt);
        }
    } else {
        socketTmp->isCanWrite_ = true;
        socketTmp->WriteData();
    }
}

int Socket::impl::GetMaxConnectionDevicesNum()
{
    int number = SOCK_MAX_CLIENT;
    if (!AdapterConfig::GetInstance()->GetValue(SECTION_SOCKET_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, number)) {
        LOG_DEBUG("[sock]%{public}s: It's failed to get the max connection number", __FUNCTION__);
    }
    return number;
}

Socket::Socket() : pimpl(nullptr)
{
    state_ = SocketState::INIT;
    pimpl = std::make_unique<Socket::impl>();
    this->pimpl->transportObserver_ = std::make_unique<Socket::impl::DataTransportObserverImplement>(*this);
}

Socket::~Socket()
{}

int Socket::Connect(const std::string &addr, const Uuid &uuid, int securityFlag, int &sockfd)
{
    LOG_INFO("[sock]%{public}s", __func__);

    sockfd = SOCK_INVALID_FD;
    int socketPair[2] = {SOCK_INVALID_FD, SOCK_INVALID_FD};

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socketPair) == -1) {
        LOG_ERROR("[sock]%{public}s: create rfcomm socket pair failed", __FUNCTION__);
        return -1;
    }

    SetRemoteAddr(addr);

    sdpClient_ = std::make_unique<SocketSdpClient>();
    int ret = sdpClient_->StartDiscovery(addr, uuid, this);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s: Discovery SPP Service Fail!", __FUNCTION__);
    }

    upperlayerFd_ = socketPair[0];
    transportFd_ = socketPair[1];

    LOG_INFO("[sock]%{public}s appFd:%{public}d fd:%{public}d", __func__, upperlayerFd_, transportFd_);

    sockfd = upperlayerFd_;
    upperlayerFd_ = SOCK_INVALID_FD;
    securityFlag_ = securityFlag;
    SocketThread::GetInstance().AddSocket(transportFd_, 0, *this);
    return ret;
}

int Socket::Listen(const std::string &name, const Uuid &uuid, int securityFlag, int &sockfd)
{
    LOG_INFO("[sock]%{public}s", __func__);

    isServer_ = true;
    sockfd = SOCK_INVALID_FD;
    int socketPair[2] = {SOCK_INVALID_FD, SOCK_INVALID_FD};
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socketPair) == -1) {
        LOG_ERROR("[sock]%{public}s: create listen socket failed", __FUNCTION__);
        return -1;
    }
    upperlayerFd_ = socketPair[0];
    transportFd_ = socketPair[1];
    LOG_INFO("[sock]%{public}s appFd:%{public}d fd:%{public}d", __func__, upperlayerFd_, transportFd_);
    sockfd = upperlayerFd_;
    upperlayerFd_ = SOCK_INVALID_FD;
    securityFlag_ = securityFlag;
    state_ = LISTEN;

    SocketThread::GetInstance().AddSocket(transportFd_, 0, *this);

    scn_ = RFCOMM_AssignServerNum();

    sdpServer_ = std::make_unique<SocketSdpServer>();
    int ret = sdpServer_->RegisterSdpService(name, uuid, scn_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s: Discovery SPP Service Fail!", __FUNCTION__);
    }

    serviceId_ = AssignServiceId();
    LOG_INFO("[sock]%{public}s securityFlag:%{public}d serviceId_:%{public}d", __func__, securityFlag_, serviceId_);
    socketGapServer_ = std::make_unique<SocketGapServer>();
    socketGapServer_->RegisterServiceSecurity(scn_, securityFlag_, serviceId_);

    sockTransport_ = std::move(transportFactory_->CreateRfcommTransport(
        nullptr, scn_, SOCK_DEF_RFC_MTU, *this->pimpl->transportObserver_.get(), *GetDispatchter()));
    sockTransport_->RegisterServer();

    std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
    g_allServerSockets.push_back(this);
    return ret;
}

int Socket::ReceiveSdpResult(uint8_t scn)
{
    LOG_INFO("[sock]%{public}s", __func__);

    serviceId_ = AssignServiceId();
    scn_ = scn;
    LOG_INFO("[sock]%{public}s securityFlag:%{public}d serviceId_:%{public}d scn:%hhu", __func__, securityFlag_, serviceId_, scn_);
    if (scn_ > SOCK_MAX_SERVER) {
        LOG_INFO("[sock]%{public}s scn invalid", __func__);
        return -1;
    }

    socketGapClient_ = std::make_unique<SocketGapClient>();
    socketGapClient_->RegisterServiceSecurity(remoteAddr_, scn_, securityFlag_, serviceId_);
    RawAddress rawAddr = RawAddress::ConvertToString(remoteAddr_.addr);
    sockTransport_ = std::move(transportFactory_->CreateRfcommTransport(
        &rawAddr, scn_, SOCK_DEF_RFC_MTU, *this->pimpl->transportObserver_.get(), *GetDispatchter()));

    switch (state_) {
        case INIT:
            if (sockTransport_->Connect() < 0) {
                LOG_ERROR("[sock]create rfcomm channel failed");
                SocketThread::GetInstance().DeleteSocket(*this);
                CloseSocketFd();
                return -1;
            }
            state_ = CONNECTING;
            break;
        default:
            LOG_ERROR("[sock]create rfcomm channel failed");
            break;
    }
    return 0;
}

int Socket::AddSocketInternal(BtAddr addr, DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU)
{
    LOG_INFO("[sock]%{public}s", __func__);

    std::unique_ptr<Socket> acceptSocket = std::make_unique<Socket>();
    int socketPair[2] = {SOCK_INVALID_FD, SOCK_INVALID_FD};
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socketPair) == -1) {
        LOG_ERROR("[sock]create accept socket failed");
    }
    LOG_DEBUG("AddSocketInternal : fd:%{public}d, fd:%{public}d", socketPair[0], socketPair[1]);
    acceptSocket->upperlayerFd_ = socketPair[0];
    acceptSocket->transportFd_ = socketPair[1];
    acceptSocket->remoteAddr_ = addr;
    acceptSocket->isNewSocket_ = true;
    acceptSocket->isServer_ = true;
    acceptSocket->state_ = SocketState::CONNECTED;
    acceptSocket->sendMTU_ = sendMTU;
    acceptSocket->recvMTU_ = recvMTU;
    acceptSocket->newSockTransport_ = transport;
    mutex_.lock();
    auto it = socketMap_.emplace(transport, std::move(acceptSocket));
    mutex_.unlock();

    SocketThread::GetInstance().AddSocket(
        it.first->second.get()->transportFd_, 0, *(it.first->second.get()));

    utility::Message msg(SOCKET_ACCEPT_NEW);
    msg.arg1_ = socketPair[0];
    msg.arg2_ = it.first->second.get();
    SocketService *socketService =
        static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
    if (socketService != nullptr) {
            socketService->ProcessMessage(msg);
    }

    std::lock_guard<std::recursive_mutex> lck(Socket::g_socketMutex);
    g_allServerSockets.push_back(it.first->second.get());

    if (socketPair[0] < 0) {
        LOG_ERROR("[sock]create accept socket failed");
    }
    return socketPair[0];
}

bool Socket::SendAppConnectInfo(int fd, BtAddr addr, bool status, int acceptFd)
{
    LOG_INFO("[sock]%{public}s", __func__);

    SocketConnectInfo connectInfo;
    (void)memset_s(&connectInfo, sizeof(connectInfo), 0, sizeof(connectInfo));
    (void)memcpy_s(connectInfo.addr, sizeof(addr.addr), addr.addr, sizeof(addr.addr));
    connectInfo.status = status;
    LOG_INFO("[sock]%{public}s size:%{public}zu", __func__, sizeof(connectInfo));
    if (acceptFd == -1) {
        return SocketUtil::SocketSendData(fd, reinterpret_cast<const uint8_t *>(&connectInfo), sizeof(connectInfo));
    } else {
        return SocketUtil::SocketSendFd(fd, reinterpret_cast<const uint8_t *>(&connectInfo),
                                        sizeof(connectInfo), acceptFd);
    }
}

void Socket::ProcessDisconnection(Socket &socket, DataTransport *transport)
{
    LOG_INFO("[sock]%{public}s", __func__);

    IPowerManager::GetInstance().StatusUpdate(
        RequestStatus::CONNECT_OFF, PROFILE_NAME_SPP, RawAddress::ConvertToString(socket.remoteAddr_.addr));

    if (socket.IsServer()) {
        if (socket.socketMap_.find(transport) != socket.socketMap_.end()) {
            socket.clientNumber_--;
            Socket *serverSocket = nullptr;
            serverSocket = socket.socketMap_.at(transport).get();
            serverSocket->state_ = DISCONNECTED;
            serverSocket->newSockTransport_ = nullptr;
            SocketThread::GetInstance().DeleteSocket(*serverSocket);
            serverSocket->CloseSocketFd();
            std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
            Socket::EraseSocket(*serverSocket);
            socket.socketMap_.erase(transport);
            socket.NotifyServiceDeleteSocket(*serverSocket);
            if (transport != nullptr) {
                delete transport;
            }
        } else {
            LOG_ERROR("[sock]socket does not exist");
        }
    } else {
        socket.state_ = DISCONNECTED;
        socketGapClient_->UnregisterSecurity(remoteAddr_, scn_, serviceId_);
        FreeServiceId(serviceId_);
        SocketThread::GetInstance().DeleteSocket(*this);
        socket.CloseSocketFd();
        std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
        Socket::EraseSocket(socket);
        Socket::NotifyServiceDeleteSocket(socket);
    }
}

void Socket::SetRemoteAddr(std::string addr)
{
    LOG_INFO("[sock]%{public}s", __func__);

    RawAddress rawAddr(addr);
    rawAddr.ConvertToUint8(remoteAddr_.addr);
    remoteAddr_.type = BT_PUBLIC_DEVICE_ADDRESS;
}

void Socket::CloseSocket(bool isDisable)
{
    LOG_INFO("[sock]%{public}s", __func__);

    CloseSocketFd();

    if (isServer_ && (!isNewSocket_)) {
        RFCOMM_FreeServerNum(scn_);
        sdpServer_->UnregisterSdpService();
        socketGapServer_->UnregisterSecurity(remoteAddr_, scn_, serviceId_);
        FreeServiceId(serviceId_);
        if (isDisable) {
            sockTransport_->RemoveServer(true);
        } else {
            sockTransport_->RemoveServer(false);
        }
        state_ = CLOSED;
        NotifyServiceDeleteSocket(*this);
        std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
        EraseSocket(*this);
        return;
    }

    if (state_ == CONNECTED || state_ == CONNECTING) {
        LOG_INFO("[sock]%{public}s close connection", __func__);
        if (isServer_) {
            socketGapServer_->UnregisterSecurity(remoteAddr_, scn_, serviceId_);
        } else {
            socketGapClient_->UnregisterSecurity(remoteAddr_, scn_, serviceId_);
        }
        FreeServiceId(serviceId_);

        if (isServer_) {
            if (newSockTransport_ != nullptr) {
                newSockTransport_->Disconnect();
            } else {
                LOG_ERROR("[sock]%{public}s newSockTransport is null", __func__);
            }
        } else {
            if (sockTransport_ != nullptr) {
                sockTransport_->Disconnect();
            } else {
                LOG_ERROR("[sock]%{public}s client sockTransport is null", __func__);
            }
        }
    } else if (state_ == INIT || state_ == DISCONNECTED) {
        LOG_INFO("[sock]%{public}s close no connection", __func__);
        NotifyServiceDeleteSocket(*this);
    }
}

void Socket::OnSocketReadReady(Socket &sock)
{
    std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
    std::vector<Socket *>::iterator it = find(g_allServerSockets.begin(), g_allServerSockets.end(), &sock);
    if (it == g_allServerSockets.end()) {
        LOG_DEBUG("[sock]%{public}s socket does not exist", __func__);
        return;
    }

    std::lock_guard<std::recursive_mutex> lck(sock.writeMutex_);
    if (sock.isCanWrite_) {
        LOG_INFO("[sock]%{public}s socket write data", __func__);
        sock.WriteData();
    }
}

void Socket::OnSocketWriteReady(Socket &sock)
{
    LOG_INFO("[sock]%{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
    std::vector<Socket *>::iterator it = find(g_allServerSockets.begin(), g_allServerSockets.end(), &sock);
    if (it == g_allServerSockets.end()) {
        LOG_DEBUG("[sock]%{public}s socket does not exist", __func__);
        return;
    }

    SocketService *socketService =
        static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
    if (socketService != nullptr) {
        socketService->GetDispatcher()->PostTask(std::bind(&Socket::OnSocketWriteReadyNative, &sock, std::ref(sock)));
    }
}

void Socket::OnSocketWriteReadyNative(Socket &sock)
{
    LOG_INFO("[sock]%{public}s", __func__);

    sock.isCanRead_ = true;

    if (sock.recvBufLen_ == 0) {
        LOG_INFO("[sock]%{public}s recvbuf has been send", __func__);
        sock.ReadData();
        return;
    }

    SocketSendRet sendRet = SendDataToApp(sock.transportFd_, sock.recvDataBuf_, sock.recvBufLen_);
    switch (sendRet) {
        case SOCKET_SEND_NONE:
        case SOCKET_SEND_PARTIAL:
            sock.isCanRead_ = false;
            SocketThread::GetInstance().AddSocket(
                sock.transportFd_, 1, sock);
            break;
        case SOCKET_SEND_ERROR:
            sock.isCanRead_ = false;
            LOG_INFO("[sock]%{public}s close socket", __func__);
            SocketThread::GetInstance().DeleteSocket(sock);
            sock.CloseSocket(false);
            break;
        case SOCKET_SEND_ALL:
            sock.isCanRead_ = true;
            (void)memset_s(sock.recvDataBuf_, SOCK_DEF_RFC_MTU, 0, SOCK_DEF_RFC_MTU);
            sock.ReadData();
            LOG_INFO("[sock]%{public}s send data success", __func__);
            break;
        default:
            break;
    }
}

void Socket::ReadData()
{
    Packet *pkt = nullptr;
    uint8_t *pData = nullptr;
    Buffer *buf = nullptr;

    while (true) {
        if (!this->isCanRead_) {
            LOG_DEBUG("[sock]%{public}s can not read.", __func__);
            return;
        }

        if (this->isNewSocket_) {
            if (this->newSockTransport_ == nullptr) {
                LOG_DEBUG("[sock]%{public}s newSockTransport is null", __func__);
                return;
            }
            if (this->newSockTransport_->Read(&pkt) != 0) {
                break;
            }
        } else {
            if (this->sockTransport_->Read(&pkt) != 0) {
                break;
            }
        }

        if (pkt == nullptr) {
            LOG_ERROR("[sock]%{public}s pkt is null", __func__);
            return;
        }

        size_t len = PacketPayloadSize(pkt);
        if (len == 0) {
            break;
        }
        buf = PacketContinuousPayload(pkt);
        if (buf != nullptr) {
            pData = (uint8_t *)BufferPtr(buf);
        }
        if (pData == nullptr) {
            return;
        }

        this->WriteDataToAPP(pData, len);

        if (pkt != nullptr) {
            PacketFree(pkt);
            pkt = nullptr;
        }
    }
}

void Socket::WriteDataToAPP(const uint8_t *buffer, size_t len)
{
    LOG_INFO("[sock]%{public}s", __func__);

    SocketSendRet sendRet = SendDataToApp(this->transportFd_, buffer, len);
    switch (sendRet) {
        case SOCKET_SEND_NONE:
        case SOCKET_SEND_PARTIAL:
            LOG_INFO("[sock]%{public}s SOCKET_SEND_PARTIAL", __func__);
            this->isCanRead_ = false;
            (void)memcpy_s(this->recvDataBuf_, SOCK_DEF_RFC_MTU, buffer, len);
            this->recvBufLen_ = len;
            SocketThread::GetInstance().AddSocket(this->transportFd_, 1, *this);
            break;
        case SOCKET_SEND_ERROR:
            this->isCanRead_ = false;
            LOG_INFO("[sock]%{public}s send data error", __func__);
            SocketThread::GetInstance().DeleteSocket(*this);
            this->CloseSocket(false);
            break;
        case SOCKET_SEND_ALL:
            this->isCanRead_ = true;
            LOG_INFO("[sock]%{public}s send data success", __func__);
            break;
        default:
            break;
    }
}

void Socket::WriteData()
{
    LOG_INFO("[sock]%{public}s", __func__);

    int totalSize = 0;

    {
        std::lock_guard<std::mutex> lock(fdMutex_);
        if (ioctl(this->transportFd_, FIONREAD, &totalSize) != 0) {
            LOG_ERROR("[sock]%{public}s ioctl read fd error", __func__);
            return;
        }
    }

    if (totalSize == 0) {
        LOG_DEBUG("[sock]%{public}s recv buffer has no data", __func__);
        return;
    }

    LOG_INFO("[sock]%{public}s totalSize:%{public}d", __func__, totalSize);

    while (totalSize > 0) {
        if (this->isCanWrite_) {
            int mallocSize = (totalSize > this->sendMTU_) ? this->sendMTU_ : totalSize;

            Packet *wPkt = PacketMalloc(0, 0, mallocSize);
            if (wPkt == nullptr) {
                LOG_INFO("[sock]pkt is null");
                return;
            }
            Buffer *wPayloadBuf = PacketContinuousPayload(wPkt);
            void *buffer = BufferPtr(wPayloadBuf);

            int wbytes = read(this->transportFd_, buffer, mallocSize);
            LOG_INFO("[sock]%{public}s wbytes:%{public}d", __func__, wbytes);
            if (wbytes <= 0) {
                LOG_DEBUG("[sock]%{public}s socket fd exception", __func__);
                PacketFree(wPkt);
                return;
            }
            int ret = TransportWrite(wPkt);
            if (ret < 0) {
                LOG_DEBUG("[sock]%{public}s stack write failed", __func__);
                (void)memcpy_s(this->sendDataBuf_, wbytes, buffer, wbytes);
                this->sendBufLen_ = wbytes;
                this->isCanWrite_ = false;
                PacketFree(wPkt);
                return;
            }
            totalSize -= wbytes;
            PacketFree(wPkt);
        } else {
            return;
        }
    }
}

int Socket::TransportWrite(Packet *subPkt)
{
    LOG_INFO("[sock]%{public}s", __func__);

    RawAddress rawAddr = RawAddress::ConvertToString(this->remoteAddr_.addr);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_SPP, rawAddr);

    int ret = 0;
    if (this->isNewSocket_) {
        if (this->newSockTransport_ == nullptr) {
            LOG_DEBUG("[sock]%{public}s newSockTransport is nullptr", __func__);
        } else {
            ret = this->newSockTransport_->Write(subPkt);
        }
    } else {
        ret = this->sockTransport_->Write(subPkt);
    }
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_SPP, rawAddr);
    return ret;
}

void Socket::OnSocketException(Socket &sock)
{
    LOG_INFO("[sock]%{public}s", __func__);

    SocketService *socketService =
        static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
    if (socketService != nullptr) {
        socketService->GetDispatcher()->PostTask(std::bind(&Socket::OnSocketExceptionNative, &sock, std::ref(sock)));
    }
}

void Socket::OnSocketExceptionNative(Socket &sock)
{
    LOG_INFO("[sock]%{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(Socket::g_socketMutex);
    std::vector<Socket *>::iterator it;

    LOG_INFO("[sock]%{public}s size:%{public}zu", __func__, g_allServerSockets.size());

    for (it = g_allServerSockets.begin(); it != g_allServerSockets.end(); ++it) {
        if (*it == &sock) {
            sock.CloseSocket(false);
            break;
        }
    }
}

GAP_Service Socket::AssignServiceId()
{
    int serviceId = 0;
    for (int i = 0; i < SOCK_MAX_SERVICE_ID; i++) {
        if (g_arrayServiceId[i] == 0) {
            g_arrayServiceId[i] = SPP_ID_START + i;
            serviceId = g_arrayServiceId[i];
            break;
        }
    }
    return (GAP_Service)serviceId;
}

void Socket::FreeServiceId(GAP_Service serviceId)
{
    if (serviceId >= SPP_ID_START) {
        g_arrayServiceId[serviceId - SPP_ID_START] = 0;
    }
}

SocketSendRet Socket::SendDataToApp(int fd, const uint8_t *buf, size_t len)
{
    LOG_INFO("[sock]%{public}s", __func__);

#ifdef DARWIN_PLATFORM
    auto sendRet = send(fd, buf, len, MSG_DONTWAIT);
#else
    auto sendRet = send(fd, buf, len, MSG_NOSIGNAL);
#endif
    if (sendRet < 0) {
        if ((errno == EAGAIN || errno == EWOULDBLOCK)) {
            return SOCKET_SEND_NONE;
        }
        return SOCKET_SEND_ERROR;
    }

    if (sendRet == 0) {
        return SOCKET_SEND_ERROR;
    }

    if (sendRet == ssize_t(len)) {
        return SOCKET_SEND_ALL;
    }

    return SOCKET_SEND_PARTIAL;
}

void Socket::NotifyServiceDeleteSocket(Socket &sock)
{
    LOG_INFO("[sock]%{public}s", __func__);

    utility::Message msg(SOCKET_CLOSE);
    msg.arg2_ = &sock;
    SocketService *socketService =
        static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
    if (socketService != nullptr) {
        socketService->ProcessMessage(msg);
    }
}

void Socket::EraseSocket(Socket &socket)
{
    LOG_INFO("[sock]%{public}s", __func__);

    std::vector<Socket *>::iterator it;
    LOG_INFO("[sock]%{public}s size:%{public}zu", __func__, g_allServerSockets.size());

    for (it = g_allServerSockets.begin(); it != g_allServerSockets.end(); ++it) {
        if (*it == &socket) {
            g_allServerSockets.erase(it);
            break;
        }
    }
}

void Socket::RemoveServerSocket()
{
    LOG_INFO("[sock]%{public}s", __func__);

    CloseSocketFd();
    EraseSocket(*this);
    sockTransport_->RemoveServer(true);
}

void Socket::CloseSocketFd()
{
    LOG_INFO("[sock]%{public}s", __func__);
    if (this->transportFd_ != SOCK_INVALID_FD) {
        LOG_DEBUG("closefd : transportFd_:%{public}d", this->transportFd_);
        shutdown(this->transportFd_, SHUT_RDWR);
        close(this->transportFd_);
        std::lock_guard<std::mutex> lock(this->fdMutex_);
        this->transportFd_ = SOCK_INVALID_FD;
    }

    if (this->upperlayerFd_ != SOCK_INVALID_FD) {
        LOG_DEBUG("closefd : upperlayerFd_:%{public}d", this->upperlayerFd_);
        shutdown(this->upperlayerFd_, SHUT_RDWR);
        close(this->upperlayerFd_);
        this->upperlayerFd_ = SOCK_INVALID_FD;
    }
}

utility::Dispatcher *Socket::GetDispatchter()
{
    LOG_INFO("[sock]%{public}s", __func__);
    SocketService *socketService =
        static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
    if (socketService == nullptr) {
        return nullptr;
    }
    return socketService->GetDispatcher();
}

void Socket::ClearUpAllSocket()
{
    LOG_INFO("[sock]%{public}s", __func__);

    LOG_INFO("[sock]%{public}s size:%{public}zu", __func__, g_allServerSockets.size());
    if (g_allServerSockets.size() > 0) {
        g_allServerSockets.clear();
    }
}
}  // namespace bluetooth
}  // namespace OHOS
