/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "pan_network.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_tun.h>

#include "interface_adapter_manager.h"
#include "interface_adapter_classic.h"
#include "pan_service.h"

namespace OHOS {
namespace bluetooth {
PanNetwork::PanNetwork()
{
    pollThreadId_ = static_cast<pthread_t>(-1);
    fd_ = -1;
    keepPolling_ = false;
}

PanNetwork::~PanNetwork()
{
    Close();
}

int PanNetwork::Open()
{
    pthread_t threadId;
    int inetSocket = -1;
    int ret = PAN_FAILURE;

    if (fd_ < 0) {
        LOG_DEBUG("[Pan Network]%{public}s():fd is null,creat fd", __FUNCTION__);
        fd_ = open(PAN_DEVICE_PATH, O_RDWR);
        if (fd_ < 0) {
            LOG_ERROR("[Pan Network]%{public}s():open %{public}s failed, fd = %{public}d error = %{public}d",
                __FUNCTION__, PAN_DEVICE_PATH, fd_, errno);
            return PAN_FAILURE;
        } else {
            LOG_DEBUG("[Pan Network]%{public}s(): fd = %{public}d", __FUNCTION__, fd_);
        }
        ret = TunSetIff();
        if (ret == PAN_SUCCESS) {
            inetSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        if ((ret == PAN_SUCCESS) && (inetSocket >= 0)) {
            ret = SetMacAddress(inetSocket);
        }

        if ((ret == PAN_SUCCESS) && (inetSocket >= 0)) {
            ret = SetIffUp(inetSocket);
        }

        if ((ret == PAN_SUCCESS) && (inetSocket >= 0)) {
            ret = SetIpAddress(inetSocket);
        }

        if ((ret == PAN_SUCCESS) && (inetSocket >= 0)) {
            ret = SetIffUp(inetSocket);
        }
        if (inetSocket >= 0) {
            close(inetSocket);
        }

        if (ret == PAN_SUCCESS) {
            LOG_DEBUG("[Pan Network]%{public}s():create thread", __FUNCTION__);
            threadId = CreateThread(PollEventThread, this);
            if (threadId == static_cast<pthread_t>(-1)) {
                LOG_DEBUG("[Pan Network]%{public}s():create thread failed", __FUNCTION__);
                ret = PAN_FAILURE;
            } else {
                pollThreadId_ = threadId;
            }
        }
    }
    return ret;
}

int PanNetwork::TunSetIff()
{
    LOG_DEBUG("[Pan Network]%{public}s():", __FUNCTION__);
    struct ifreq ifr;
    int err;

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        LOG_ERROR("[Pan Network] %{public}s:memset error.", __func__);
        return PAN_FAILURE;
    }
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, PAN_NETWORK_NAME, strlen(PAN_NETWORK_NAME)) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
    }
    err = ioctl(fd_, TUNSETIFF, &ifr);
    if (err < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): tun set iff error:%{public}d", __FUNCTION__, err);
        return PAN_FAILURE;
    }
    return PAN_SUCCESS;
}

int PanNetwork::SetMacAddress(int inetSocket)
{
    LOG_DEBUG("[Pan Network]%{public}s():", __FUNCTION__);
    struct ifreq ifr;
    int err;

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        LOG_ERROR("[Pan Network] %{public}s:memset error.", __func__);
        return PAN_FAILURE;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, PAN_NETWORK_NAME, strlen(PAN_NETWORK_NAME)) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
        return PAN_FAILURE;
    }
    err = ioctl(inetSocket, SIOCGIFHWADDR, &ifr);
    if (err < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): get network hardware error:%{public}d", __FUNCTION__, err);
        return PAN_FAILURE;
    }
    BtAddr btAddr;
    RawAddress(PanService::GetLocalAddress()).ConvertToUint8(btAddr.addr);
    for (int i = 0; i < BT_ADDRESS_LENGTH; i++) {
        ifr.ifr_hwaddr.sa_data[i] = btAddr.addr[BT_ADDRESS_LENGTH - i -1];
    }
    if (ifr.ifr_hwaddr.sa_data[0] & 0x01) {
        LOG_DEBUG("[Pan Network]%{public}s():use unicast MAC address", __FUNCTION__);
        ifr.ifr_hwaddr.sa_data[0] &= ~0x01;
    }
    err = ioctl(inetSocket, SIOCSIFHWADDR, &ifr);
    if (err < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): set network hardware error:%{public}d", __FUNCTION__, err);
        return PAN_FAILURE;
    }
    return PAN_SUCCESS;
}

int PanNetwork::SetIpAddress(int inetSocket)
{
    LOG_DEBUG("[Pan Network]%{public}s():", __FUNCTION__);
    struct ifreq ifr;
    struct in_addr ipv4Addr = {INADDR_ANY};
    struct in_addr mask4Addr = {INADDR_ANY};
    size_t mask = 0xffffffff;

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        LOG_ERROR("[Pan Network] %{public}s:memset error.", __func__);
        return PAN_FAILURE;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, PAN_NETWORK_NAME, strlen(PAN_NETWORK_NAME)) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
        return PAN_FAILURE;
    }

    if (inet_aton(PAN_NETWORK_IPV4_ADDRESS, &ipv4Addr) == 0) {
        LOG_ERROR("[Pan Network] %{public}s:inet_aton error.", __func__);
        return PAN_FAILURE;
    }
    mask = mask >> (MAX_MASK_LENGTH - PAN_NETWORK_IPV4_PREFIX_LENGTH);
    mask4Addr.s_addr = mask;

    struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    sin->sin_addr = ipv4Addr;
    if (ioctl(inetSocket, SIOCSIFADDR, &ifr) < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): ioctl set ip address failed", __FUNCTION__);
        return PAN_FAILURE;
    }
    sin->sin_addr = mask4Addr;
    if (ioctl(inetSocket, SIOCSIFNETMASK, &ifr) < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): ioctl set mask address failed", __FUNCTION__);
        return PAN_FAILURE;
    }

    if (ioctl(inetSocket, SIOCGIFADDR, &ifr) < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): ioctl get ip address failed", __FUNCTION__);
        return PAN_SUCCESS;
    }
    ipv4Addr.s_addr = (reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr))->sin_addr.s_addr;
    std::string ipv4AddrStr = std::string(inet_ntoa(ipv4Addr));
    LOG_DEBUG("[Pan Network]%{public}s(): ip address is  ipv4AddrStr=%{public}s",
        __FUNCTION__, ipv4AddrStr.c_str());
    return PAN_SUCCESS;
}

int PanNetwork::SetIffUp(int inetSocket)
{
    LOG_DEBUG("[Pan Network]%{public}s():", __FUNCTION__);
    struct ifreq ifr;
    int err;

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        LOG_ERROR("[Pan Network] %{public}s:memset error.", __func__);
        return PAN_FAILURE;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, PAN_NETWORK_NAME, strlen(PAN_NETWORK_NAME)) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
        return PAN_FAILURE;
    }
    ifr.ifr_flags |= IFF_UP;
    ifr.ifr_flags |= IFF_MULTICAST;

    err = ioctl(inetSocket, SIOCSIFFLAGS, &ifr);
    if (err < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): up network error:%{public}d", __FUNCTION__, err);
        return PAN_FAILURE;
    }
    return PAN_SUCCESS;
}

int PanNetwork::SetIffdown(int inetSocket)
{
    LOG_DEBUG("[Pan Network]%{public}s():", __FUNCTION__);
    struct ifreq ifr;
    int err;

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        LOG_ERROR("[Pan Network] %{public}s:memset error.", __func__);
        return PAN_FAILURE;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, PAN_NETWORK_NAME, strlen(PAN_NETWORK_NAME)) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
        return PAN_FAILURE;
    }
    ifr.ifr_flags &= ~IFF_UP;

    err = ioctl(inetSocket, SIOCSIFFLAGS, &ifr);
    if (err < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): down network error:%{public}d", __FUNCTION__, err);
        return PAN_FAILURE;
    }
    return PAN_SUCCESS;
}

int PanNetwork::Close()
{
    LOG_DEBUG("[Pan Network]%{public}s():", __FUNCTION__);
    int ret = PAN_FAILURE;
    int inetSocket;

    if (isRemoteDeviceBusy_) {
        isRemoteDeviceBusy_ = false;
        std::lock_guard<std::mutex> lock(mutexBusyChanged_);
        cvWaitBusyChanged_.notify_all();
    }

    if (keepPolling_) {
        keepPolling_ = false;
        pthread_join(pollThreadId_, nullptr);
    }
    pollThreadId_ = static_cast<pthread_t>(-1);

    if (fd_ >= 0) {
        LOG_DEBUG("[Pan Network]%{public}s(): Closing fd=%{public}d", __FUNCTION__, fd_);
        inetSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (inetSocket < 0) {
            LOG_ERROR("[Pan Network]%{public}s(): get AF_INET socket inetSocket:%{public}d", __FUNCTION__, inetSocket);
            ret = PAN_FAILURE;
        } else {
            SetIffdown(inetSocket);
            close(inetSocket);
        }

        close(fd_);
        fd_ = -1;
    }

    return PAN_SUCCESS;
}

int PanNetwork::ReceiveRemoteBusy(bool isBusy)
{
    isRemoteDeviceBusy_ = isBusy;
    if (!isBusy) {
        std::lock_guard<std::mutex> lock(mutexBusyChanged_);
        cvWaitBusyChanged_.notify_all();
    }
    return PAN_SUCCESS;
}

int PanNetwork::WriteData(EthernetHeader head, uint8_t *data, int len)
{
    LOG_DEBUG("[Pan Network]%{public}s(): len=%{public}d", __FUNCTION__, len);
    if (fd_ < 0) {
        LOG_ERROR("[Pan Network]%{public}s(): fd is not open", __FUNCTION__);
        return PAN_FAILURE;
    }
    ssize_t ret;
    uint8_t packet[PAN_MAX_NETWORK_PACKET_SIZE] = {0};
    head.protocol = htons(head.protocol);
    if (memcpy_s(packet, PAN_MAX_NETWORK_PACKET_SIZE, &head, sizeof(head)) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
        return PAN_FAILURE;
    }
    if (len > (PAN_MAX_NETWORK_PACKET_SIZE - sizeof(head))) {
        LOG_ERROR("[Pan Network]%{public}s(): data length is exceeded limit", __FUNCTION__);
        return PAN_FAILURE;
    }
    if (memcpy_s(packet + sizeof(head), PAN_MAX_NETWORK_PACKET_SIZE - sizeof(head), data, len) != EOK) {
        LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
        return PAN_FAILURE;
    }
    do {
    } while ((ret = write(fd_, packet, len + sizeof(head))) == -1 && errno == EINTR);
    if (ret < 0) {
        int rtn = errno;
        LOG_ERROR("[Pan Network]%{public}s(): Cannot write to :%{public}s", __FUNCTION__, strerror(errno));
        return rtn;
    }
    return PAN_SUCCESS;
}

pthread_t PanNetwork::CreateThread(void* (*startRoutine)(void*), void* arg)
{
    LOG_DEBUG("[Pan Network]%{public}s():create_thread: entered", __FUNCTION__);
    pthread_attr_t threadAttr;

    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_t threadId = static_cast<pthread_t>(-1);
    if (pthread_create(&threadId, &threadAttr, startRoutine, arg) != 0) {
        LOG_ERROR("[Pan Network]%{public}s():pthread_create : %{public}s", __FUNCTION__, strerror(errno));
        return static_cast<pthread_t>(-1);
    }
    LOG_DEBUG("[Pan Network]%{public}s():%{public}lu: threadcreated successfully", __FUNCTION__, threadId);
    return threadId;
}

void* PanNetwork::PollEventThread(void* arg)
{
    PanNetwork *network = (PanNetwork*)arg;
    if (network != nullptr) {
        network->PollEventThread_();
    } else {
        LOG_ERROR("[Pan Network]%{public}s():Thread creat fail, is null", __FUNCTION__);
    }
    return nullptr;
}

void PanNetwork::PollEventThread_()
{
    HILOGI("[Pan Network] Thread: %{public}d, fd: %{public}d execute", static_cast<int>(pollThreadId_), fd_);
    struct pollfd pfds[1];
    keepPolling_ = true;
    pfds[0].fd = fd_;
    pfds[0].events = POLLIN;

    SetPanNetworkNonBlocking(fd_);

    while (keepPolling_) {
        int ret;
        std::unique_lock<std::mutex> lock(mutexBusyChanged_);
        if (isRemoteDeviceBusy_) {
            cvWaitBusyChanged_.wait(lock, [this] { return !isRemoteDeviceBusy_; });
        }
        do {
        } while ((ret = poll(pfds, 1, POLL_TIMEOUT)) == -1 && errno == EINTR);
        if (ret < 0) {
            LOG_ERROR("[Pan Network]%{public}s(): Cannot poll for fds: %{public}s", __FUNCTION__,
                strerror(errno));
            break;
        }
        if (pfds[0].revents & POLLIN) {
            LOG_DEBUG("[Pan Network]%{public}s(): POLLIN", __FUNCTION__);
            ReadPanNetworkEvent();
        }
    }
    LOG_DEBUG("[Pan Network]%{public}s(): exit", __FUNCTION__);
    keepPolling_ = false;
    pollThreadId_ = static_cast<pthread_t>(-1);
}

void PanNetwork::SetPanNetworkNonBlocking(int fd)
{
    int opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        LOG_DEBUG("[Pan Network]%{public}s(): Getting flags failed (%{public}s)", __FUNCTION__,
            strerror(errno));
        return;
    }

    opts |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, opts) < 0) {
        LOG_DEBUG("[Pan Network]%{public}s(): Setting non-blocking flag failed (%{public}s)", __FUNCTION__,
            strerror(errno));
    }
}

int PanNetwork::ReadPanNetworkEvent()
{
    uint8_t packet[PAN_MAX_NETWORK_PACKET_SIZE] = {0};
    ssize_t packetSize;

    do {
    } while ((packetSize = read(fd_, &packet, sizeof(packet))) == -1 && errno == EINTR);

    if (packetSize <= 0) {
        LOG_ERROR("[Pan Network]%{public}s():read err", __FUNCTION__);
        return PAN_FAILURE;
    }
    if (packetSize > PAN_MAX_NETWORK_PACKET_SIZE) {
        LOG_ERROR("[Pan Network]%{public}s():packet size is too large packetSize=%{public}zu",
            __FUNCTION__, packetSize);
        return PAN_FAILURE;
    }
    if (packetSize > sizeof(EthernetHeader)) {
        EthernetHeader head;
        if (memcpy_s(&head, sizeof(head), &packet, sizeof(head)) != EOK) {
            LOG_ERROR("[Pan Network]%{public}s(): memcpy error", __FUNCTION__);
            return PAN_FAILURE;
        }
        uint16_t protocol = ntohs(head.protocol);
        if ((protocol != ETH_P_IP) && (protocol != ETH_P_ARP) && (protocol != ETH_P_IPV6)) {
            LOG_ERROR("[Pan Network]%{public}s(): unknown protocol 0x%{public}x", __FUNCTION__, protocol);
            return PAN_FAILURE;
        }
        head.protocol = protocol;

        PanService::GetService()->PanSendData(head, packet + sizeof(head), packetSize - sizeof(head));
    }
    return PAN_SUCCESS;
}
}  // namespace bluetooth
}  // namespace OHOS
