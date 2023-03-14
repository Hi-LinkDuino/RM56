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

#include <sys/socket.h>
#include <string>
#include <unistd.h>
#include "bluetooth_log.h"
#include "bluetooth_host.h"
#include "bluetooth_host_proxy.h"
#include "bluetooth_socket_proxy.h"
#include "bluetooth_socket.h"
#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
const int LENGTH = 18;
struct SppClientSocket::impl {
    impl(const BluetoothRemoteDevice &addr, UUID uuid, SppSocketType type, bool auth);
    impl(int fd, std::string address);
    ~impl()
    {
        if (!proxy_) {
            HILOGI("failed: no proxy_");
            return;
        }

        proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);

        if (fd_ > 0) {
            shutdown(fd_, SHUT_RD);
            shutdown(fd_, SHUT_WR);
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "SPP_CONNECT_STATE",
                HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", "close", "ID", fd_, "ADDRESS", "empty",
                "PID", IPCSkeleton::GetCallingPid(), "UID", IPCSkeleton::GetCallingUid());
            HILOGI("fd closed, fd_: %{pubilc}d", fd_);
            close(fd_);
            fd_ = -1;
        }
    }

    void Close()
    {
        HILOGI("starts");
        if (socketStatus_ == SOCKET_CLOSED) {
            HILOGW("The socketStatus_ is already SOCKET_CLOSED");
            return;
        } else {
            socketStatus_ = SOCKET_CLOSED;
            if (fd_ > 0) {
                shutdown(fd_, SHUT_RD);
                shutdown(fd_, SHUT_WR);
                HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "SPP_CONNECT_STATE",
                    HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", "close", "ID", fd_, "ADDRESS", "empty",
                    "PID", IPCSkeleton::GetCallingPid(), "UID", IPCSkeleton::GetCallingUid());
                HILOGI("fd closed, fd_: %{pubilc}d", fd_);
                close(fd_);
                fd_ = -1;
            } else {
                HILOGE("socket not created");
                return;
            }
        }
    }

    bool RecvSocketSignal()
    {
        uint8_t recvStateBuf[1];
#ifdef DARWIN_PLATFORM
        int recvBufSize = recv(fd_, recvStateBuf, sizeof(recvStateBuf), 0);
#else
        int recvBufSize = recv(fd_, recvStateBuf, sizeof(recvStateBuf), MSG_WAITALL);
#endif
        if (recvBufSize <= 0) {
            HILOGE("service closed");
            return false;
        }
        bool state = recvStateBuf[0];

        uint8_t buf[6] = {0}; // addr buffer len
#ifdef DARWIN_PLATFORM
        int recvAddrSize = recv(fd_, buf, sizeof(buf), 0);
#else
        int recvAddrSize = recv(fd_, buf, sizeof(buf), MSG_WAITALL);
#endif
        if (recvAddrSize <= 0) {
            HILOGE("service closed");
            return false;
        }
        char token[LENGTH] = {0};
        (void)sprintf_s(token,
            sizeof(token), "%02X:%02X:%02X:%02X:%02X:%02X", buf[0x05], buf[0x04], buf[0x03], buf[0x02], buf[0x01], buf[0x00]);
        BluetoothRawAddress rawAddr {
            token
        };
        std::string address = rawAddr.GetAddress().c_str();
        return state;
    }

    int getSecurityFlags()
    {
        int flags = 0;
        if (auth_) {
            flags |= FLAG_AUTH;
            flags |= FLAG_ENCRYPT;
        }
        return flags;
    }

    InputStream &GetInputStream()
    {
        HILOGI("starts");
        if (inputStream_ == nullptr) {
            HILOGE("inputStream is NULL, failed. please Connect");
        }
        return *inputStream_;
    }

    OutputStream &GetOutputStream()
    {
        HILOGI("starts");
        if (outputStream_ == nullptr) {
            HILOGE("outputStream is NULL, failed. please Connect");
        }
        return *outputStream_;
    }

    BluetoothRemoteDevice &GetRemoteDevice()
    {
        HILOGI("starts");
        return remoteDevice_;
    }

    bool IsConnected()
    {
        HILOGI("starts");
        return socketStatus_ == SOCKET_CONNECTED;
    }

    class BluetoothClientSocketDeathRecipient;
    sptr<BluetoothClientSocketDeathRecipient> deathRecipient_;

    sptr<IBluetoothSocket> proxy_;
    std::unique_ptr<InputStream> inputStream_ {
        nullptr
    };
    std::unique_ptr<OutputStream> outputStream_ {
        nullptr
    };
    BluetoothRemoteDevice remoteDevice_;
    UUID uuid_;
    SppSocketType type_;
    std::string address_;
    int fd_;
    bool auth_;
    int socketStatus_;
};

class SppClientSocket::impl::BluetoothClientSocketDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    BluetoothClientSocketDeathRecipient(SppClientSocket::impl &host) : host_(host) {};
    ~BluetoothClientSocketDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothClientSocketDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        host_.proxy_->AsObject()->RemoveDeathRecipient(host_.deathRecipient_);
        host_.proxy_ = nullptr;
    }

private:
    SppClientSocket::impl &host_;
};

SppClientSocket::impl::impl(const BluetoothRemoteDevice &addr, UUID uuid, SppSocketType type, bool auth)
    : inputStream_(nullptr),
      outputStream_(nullptr),
      remoteDevice_(addr),
      uuid_(uuid),
      type_(type),
      fd_(-1),
      auth_(auth),
      socketStatus_(SOCKET_INIT)
{
    HILOGI("starts");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGI("failed: no hostRemote");
        return;
    }

    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_SPP);

    if (!remote) {
        HILOGI("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothSocket>(remote);

    deathRecipient_ = new BluetoothClientSocketDeathRecipient(*this);

    if (!proxy_) {
        HILOGE("proxy_ is nullptr");
        return;
    }

    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
}

SppClientSocket::impl::impl(int fd, std::string address)
    : inputStream_(std::make_unique<InputStream>(fd)),
      outputStream_(std::make_unique<OutputStream>(fd)),
      remoteDevice_(BluetoothRemoteDevice(address, 0)),
      type_(TYPE_RFCOMM),
      address_(address),
      fd_(fd),
      auth_(false),
      socketStatus_(SOCKET_CONNECTED)
{
    HILOGI("starts");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGI("failed: no hostRemote");
        return;
    }

    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_SPP);

    if (!remote) {
        HILOGI("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothSocket>(remote);

    deathRecipient_ = new BluetoothClientSocketDeathRecipient(*this);

    if (!proxy_) {
        HILOGE("proxy_ is nullptr");
        return;
    }

    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
}

SppClientSocket::SppClientSocket(const BluetoothRemoteDevice &bda, UUID uuid, SppSocketType type, bool auth)
    : pimpl(new SppClientSocket::impl(bda, uuid, type, auth))
{}

SppClientSocket::SppClientSocket(int fd, std::string address) : pimpl(new SppClientSocket::impl(fd, address))
{}

SppClientSocket::~SppClientSocket()
{}

int SppClientSocket::Connect()
{
    HILOGI("starts");
    if (!IS_BT_ENABLED()) {
        HILOGI("BR is not TURN_ON");
        return BtStatus::BT_FAILURE;
    }
    pimpl->address_ = pimpl->remoteDevice_.GetDeviceAddr();

    std::string tempAddress = pimpl->address_;
    if (!tempAddress.size()) {
        return BtStatus::BT_FAILURE;
    }
    if (pimpl->socketStatus_ == SOCKET_CLOSED) {
        HILOGE("socket closed");
        return BtStatus::BT_FAILURE;
    }

    bluetooth::Uuid serverUuid = bluetooth::Uuid::ConvertFrom128Bits(pimpl->uuid_.ConvertTo128Bits());

    if (!pimpl->proxy_) {
        HILOGE("proxy_ is nullptr");
        return BtStatus::BT_FAILURE;
    }

    pimpl->fd_ =
        pimpl->proxy_->Connect(tempAddress, serverUuid, (int32_t)pimpl->getSecurityFlags(), (int32_t)pimpl->type_);

    HILOGI("fd_: %{public}d", pimpl->fd_);
    if (pimpl->fd_ == -1) {
        HILOGE("connect failed!");
        return BtStatus::BT_FAILURE;
    }

    bool recvret = pimpl->RecvSocketSignal();
    HILOGI("recvret: %{public}d", recvret);
    pimpl->inputStream_ = std::make_unique<InputStream>(pimpl->fd_);
    pimpl->outputStream_ = std::make_unique<OutputStream>(pimpl->fd_);

    if (!recvret) {
        HILOGE("connect failed!");
        return BtStatus::BT_FAILURE;
    }
    pimpl->socketStatus_ = SOCKET_CONNECTED;
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "SPP_CONNECT_STATE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", "connect", "ID", pimpl->fd_, "ADDRESS", tempAddress,
        "PID", IPCSkeleton::GetCallingPid(), "UID", IPCSkeleton::GetCallingUid());
    return BtStatus::BT_SUCCESS;
}

void SppClientSocket::Close()
{
    HILOGI("starts");
    return pimpl->Close();
}

InputStream &SppClientSocket::GetInputStream()
{
    HILOGI("starts");
    return pimpl->GetInputStream();
}

OutputStream &SppClientSocket::GetOutputStream()
{
    HILOGI("starts");
    return pimpl->GetOutputStream();
}

BluetoothRemoteDevice &SppClientSocket::GetRemoteDevice()
{
    HILOGI("starts");
    return pimpl->GetRemoteDevice();
}

bool SppClientSocket::IsConnected() const
{
    HILOGI("starts");
    return pimpl->IsConnected();
}

struct SppServerSocket::impl {
    impl(const std::string &name, UUID uuid, SppSocketType type, bool encrypt);
    ~impl()
    {
        if (!proxy_) {
            HILOGE("proxy_ is nullptr");
            return;
        }

        proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);

        if (fd_ > 0) {
            shutdown(fd_, SHUT_RD);
            shutdown(fd_, SHUT_WR);
            close(fd_);
            HILOGI("fd closed, fd_: %{public}d", fd_);
            fd_ = -1;
        }
    }

    int Listen()
    {
        HILOGI("starts");
        if (!IS_BT_ENABLED()) {
            HILOGI("BR is not TURN_ON");
            return BtStatus::BT_FAILURE;
        }
        if (socketStatus_ == SOCKET_CLOSED) {
            HILOGE("failed, socketStatus_ is SOCKET_CLOSED");
            return BtStatus::BT_FAILURE;
        }

        bluetooth::Uuid serverUuid = bluetooth::Uuid::ConvertFrom128Bits(uuid_.ConvertTo128Bits());

        if (!proxy_) {
            HILOGE("failed, proxy_ is nullptr");
            socketStatus_ = SOCKET_CLOSED;
            return BtStatus::BT_FAILURE;
        }

        fd_ = proxy_->Listen(name_, serverUuid, (int32_t)getSecurityFlags(), (int32_t)type_);
        if (fd_ == -1) {
            HILOGE("failed, fd_ is -1");
            socketStatus_ = SOCKET_CLOSED;
            return BtStatus::BT_FAILURE;
        }

        if (socketStatus_ == SOCKET_INIT) {
            socketStatus_ = SOCKET_LISTENING;
        } else {
            HILOGE("failed, socketStatus_: %{public}d is not SOCKET_INIT", socketStatus_);
            close(fd_);
            socketStatus_ = SOCKET_CLOSED;
            return BtStatus::BT_FAILURE;
        }

        return BtStatus::BT_SUCCESS;
    }

    int getSecurityFlags()
    {
        int flags = 0;
        if (encrypt_) {
            flags |= FLAG_AUTH;
            flags |= FLAG_ENCRYPT;
        }
        return flags;
    }

    std::unique_ptr<SppClientSocket> Accept(int timeout)
    {
        HILOGI("starts");
        if (socketStatus_ != SOCKET_LISTENING) {
            HILOGE("socket is not in listen state");
            return nullptr;
        }
        struct timeval time = {timeout, 0};
        setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time, sizeof(time));
        setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time, sizeof(time));

        acceptFd_ = RecvSocketFd();
        HILOGE("RecvSocketFd acceptFd: %{public}d", acceptFd_);
        if (acceptFd_ <= 0) {
            return nullptr;
        }
        if (timeout > 0) {
            time = {0, 0};
            setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time, sizeof(time));
            setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time, sizeof(time));
        }

        std::unique_ptr<SppClientSocket> clientSocket = std::make_unique<SppClientSocket>(acceptFd_, acceptAddress_);

        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "SPP_CONNECT_STATE",
            HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", "connect", "ID", acceptFd_, "ADDRESS",
            acceptAddress_, "PID", IPCSkeleton::GetCallingPid(), "UID", IPCSkeleton::GetCallingUid());

        return clientSocket;
    }

    int RecvSocketFd()
    {
        HILOGI("starts");
        int rv = 0;
        int cfd = -1;
        int clientFd = -1;
        char ccmsg[CMSG_SPACE(sizeof(cfd))];
        char buffer[10];
        struct iovec io = {.iov_base = buffer, .iov_len = sizeof(buffer)};
        struct msghdr msg;
        (void)memset_s(&msg, sizeof(msg), 0, sizeof(msg));
        msg.msg_control = ccmsg;
        msg.msg_controllen = sizeof(ccmsg);
        msg.msg_iov = &io;
        msg.msg_iovlen = 1;

#ifdef DARWIN_PLATFORM
        rv = recvmsg(fd_, &msg, 0);
#else
        rv = recvmsg(fd_, &msg, MSG_NOSIGNAL);
#endif
        if (rv == -1) {
            HILOGE("[sock] recvmsg error  %{public}d, fd: %{public}d", errno, fd_);
            return BtStatus::BT_FAILURE;
        }
        struct cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);
        if ((cmptr != NULL) && (cmptr->cmsg_len == CMSG_LEN(sizeof(int)))) {
            if (cmptr->cmsg_level != SOL_SOCKET || cmptr->cmsg_type != SCM_RIGHTS) {
                HILOGE("[sock] control level: %{public}d", cmptr->cmsg_level);
                HILOGE("[sock] control type: %{public}d", cmptr->cmsg_type);
                return BtStatus::BT_FAILURE;
            }
            clientFd = *((int *)CMSG_DATA(cmptr));
        } else {
            return BtStatus::BT_FAILURE;
        }
        uint8_t recvBuf[rv];
        (void)memset_s(&recvBuf, sizeof(recvBuf), 0, sizeof(recvBuf));
        if (memcpy_s(recvBuf, sizeof(recvBuf), (uint8_t *)msg.msg_iov[0].iov_base, rv) != EOK) {
            HILOGE("[sock] RecvSocketFd, recvBuf memcpy_s fail");
            return BtStatus::BT_FAILURE;
        }

        uint8_t buf[6] = {0};
        if (memcpy_s(buf, sizeof(buf), &recvBuf[1], sizeof(buf)) != EOK) {
            HILOGE("[sock] RecvSocketFd, buf memcpy_s fail");
            return BtStatus::BT_FAILURE;
        }

        char token[LENGTH] = {0};
        (void)sprintf_s(token,
            sizeof(token), "%02X:%02X:%02X:%02X:%02X:%02X", buf[0x05], buf[0x04], buf[0x03], buf[0x02], buf[0x01], buf[0x00]);
        BluetoothRawAddress rawAddr {token};
        acceptAddress_ = rawAddr.GetAddress().c_str();
        return clientFd;
    }

    void Close()
    {
        HILOGI("starts");
        if (socketStatus_ == SOCKET_CLOSED) {
            HILOGW("The socketStatus_ is already SOCKET_CLOSED");
            return;
        } else {
            socketStatus_ = SOCKET_CLOSED;
            if (fd_ > 0) {
                shutdown(fd_, SHUT_RD);
                shutdown(fd_, SHUT_WR);
                close(fd_);
                HILOGI("fd closed, fd_: %{public}d", fd_);
                fd_ = -1;
                return;
            } else {
                HILOGE("socket not created");
                return;
            }
        }
    }

    const std::string &GetStringTag()
    {
        HILOGI("starts");
        if (socketStatus_ == SOCKET_CLOSED) {
            HILOGE("socketStatus_ is SOCKET_CLOSED");
            socketServiceType_ = "";
        } else {
            socketServiceType_ = "ServerSocket: Type: TYPE_RFCOMM";
            socketServiceType_.append(" ServerName: ").append(name_);
        }
        return socketServiceType_;
    }

    class BluetoothServerSocketDeathRecipient;
    sptr<BluetoothServerSocketDeathRecipient> deathRecipient_;

    sptr<IBluetoothSocket> proxy_;
    UUID uuid_;
    SppSocketType type_;
    bool encrypt_;
    int fd_;
    int socketStatus_;
    std::string name_ {
        ""
        };
    int acceptFd_;
    std::string acceptAddress_;
    std::string socketServiceType_ {
        ""
    };
};

class SppServerSocket::impl::BluetoothServerSocketDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    BluetoothServerSocketDeathRecipient(SppServerSocket::impl &host) : host_(host) {};
    ~BluetoothServerSocketDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothServerSocketDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        host_.proxy_ = nullptr;
    }

private:
    SppServerSocket::impl &host_;
};

SppServerSocket::impl::impl(const std::string &name, UUID uuid, SppSocketType type, bool encrypt)
    : uuid_(uuid), type_(type), encrypt_(encrypt), fd_(-1), socketStatus_(SOCKET_INIT), name_(name)
{
    HILOGI("(4 parameters) starts");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGI("failed: no hostRemote");
        return;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_SPP);

    if (!remote) {
        HILOGE("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothSocket>(remote);
    if (proxy_ == nullptr) {
        return;
    }
    deathRecipient_ = new BluetoothServerSocketDeathRecipient(*this);
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
}

SppServerSocket::SppServerSocket(const std::string &name, UUID uuid, SppSocketType type, bool encrypt)
    : pimpl(new SppServerSocket::impl(name, uuid, type, encrypt))
{
    HILOGI("(4 parameters) starts");
    int ret = pimpl->Listen();
    if (ret < 0) {
        HILOGE("bind listen failed!");
    }
}

SppServerSocket::~SppServerSocket()
{}

std::unique_ptr<SppClientSocket> SppServerSocket::Accept(int timeout)
{
    HILOGI("starts");
    return pimpl->Accept(timeout);
}

void SppServerSocket::Close()
{
    HILOGI("starts");
    return pimpl->Close();
}

const std::string &SppServerSocket::GetStringTag()
{
    HILOGI("starts");
    return pimpl->GetStringTag();
}

SppClientSocket *SocketFactory::BuildInsecureRfcommDataSocketByServiceRecord(
    const BluetoothRemoteDevice &device, const UUID &uuid)
{
    HILOGI("starts");
    if (device.IsValidBluetoothRemoteDevice()) {
        return new SppClientSocket(device, uuid, TYPE_RFCOMM, false);
    } else {
        HILOGE("[sock] Device is not valid.");
        return nullptr;
    }
}

SppClientSocket *SocketFactory::BuildRfcommDataSocketByServiceRecord(
    const BluetoothRemoteDevice &device, const UUID &uuid)
{
    HILOGI("starts");
    if (device.IsValidBluetoothRemoteDevice()) {
        return new SppClientSocket(device, uuid, TYPE_RFCOMM, true);
    } else {
        HILOGE("[sock] Device is not valid.");
        return nullptr;
    }
}

SppServerSocket *SocketFactory::DataListenInsecureRfcommByServiceRecord(const std::string &name, const UUID &uuid)
{
    HILOGI("starts");
    return new SppServerSocket(name, uuid, TYPE_RFCOMM, false);
}

SppServerSocket *SocketFactory::DataListenRfcommByServiceRecord(const std::string &name, const UUID &uuid)
{
    HILOGI("starts");
    return new SppServerSocket(name, uuid, TYPE_RFCOMM, true);
}
}  // namespace Bluetooth
}  // namespace OHOS