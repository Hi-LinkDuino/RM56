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

#include "event_server.h"

#include <memory>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <securec.h>

extern "C" {
#include "init_socket.h"
}

#include "device_node.h"
#include "logger.h"
#include "socket_util.h"

#define SOCKET_FILE_DIR "/dev/unix/socket/hisysevent"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-EventServer");
namespace {
constexpr int BUFFER_SIZE = 384 * 1024;
#ifndef KERNEL_DEVICE_BUFFER
constexpr int EVENT_READ_BUFFER = 2048;
#else
constexpr int EVENT_READ_BUFFER = KERNEL_DEVICE_BUFFER;
#endif

void InitRecvBuffer(int socketId)
{
    int oldN = 0;
    socklen_t oldOutSize = static_cast<socklen_t>(sizeof(int));
    if (getsockopt(socketId, SOL_SOCKET, SO_RCVBUF, static_cast<void *>(&oldN), &oldOutSize) < 0) {
        HIVIEW_LOGE("get socket buffer error=%{public}d, msg=%{public}s", errno, strerror(errno));
    }

    int sendBuffSize = BUFFER_SIZE;
    if (setsockopt(socketId, SOL_SOCKET, SO_RCVBUF, static_cast<void *>(&sendBuffSize), sizeof(int)) < 0) {
        HIVIEW_LOGE("set socket buffer error=%{public}d, msg=%{public}s", errno, strerror(errno));
    }

    int newN = 0;
    socklen_t newOutSize = static_cast<socklen_t>(sizeof(int));
    if (getsockopt(socketId, SOL_SOCKET, SO_RCVBUF, static_cast<void *>(&newN), &newOutSize) < 0) {
        HIVIEW_LOGE("get new socket buffer error=%{public}d, msg=%{public}s", errno, strerror(errno));
    }
    HIVIEW_LOGI("reset recv buffer size old=%{public}d, new=%{public}d", oldN, newN);
}
}

void SocketDevice::InitSocket(int &socketId)
{
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    if (strcpy_s(serverAddr.sun_path, sizeof(serverAddr.sun_path), SOCKET_FILE_DIR) != EOK) {
        socketId = -1;
        HIVIEW_LOGE("copy hisysevent dev path fail %{public}d, msg=%{public}s", errno, strerror(errno));
        return;
    }
    serverAddr.sun_path[sizeof(serverAddr.sun_path) - 1] = '\0';
    socketId = TEMP_FAILURE_RETRY(socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0));
    if (socketId < 0) {
        HIVIEW_LOGE("create hisysevent socket fail %{public}d, msg=%{public}s", errno, strerror(errno));
        return;
    }
    InitRecvBuffer(socketId_);
    unlink(serverAddr.sun_path);
    if (TEMP_FAILURE_RETRY(bind(socketId, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr))) < 0) {
        close(socketId);
        socketId = -1;
        HIVIEW_LOGE("bind hisysevent socket fail %{public}d, msg=%{public}s", errno, strerror(errno));
        return;
    }
}

int SocketDevice::Open()
{
    socketId_ = GetControlSocket("hisysevent");
    if (socketId_ < 0) {
        HIVIEW_LOGI("create hisysevent socket");
        InitSocket(socketId_);
    } else {
        InitRecvBuffer(socketId_);
        HIVIEW_LOGI("use hisysevent exist socket");
    }

    if (socketId_ < 0) {
        HIVIEW_LOGE("hisysevent create socket fail");
        return -1;
    }
    return socketId_;
}

int SocketDevice::Close()
{
    if (socketId_ > 0) {
        close(socketId_);
        socketId_ = -1;
    }
    return 0;
}

uint32_t SocketDevice::GetEvents()
{
    return EPOLLIN | EPOLLET;
}

std::string SocketDevice::GetName()
{
    return "SysEventSocket";
}

int SocketDevice::ReceiveMsg(std::vector<std::shared_ptr<EventReceiver>> &receivers)
{
    while (true) {
        struct sockaddr_un clientAddr;
        socklen_t clientLen = static_cast<socklen_t>(sizeof(clientAddr));
        char* recvbuf = new char[BUFFER_SIZE];
        int n = recvfrom(socketId_, recvbuf, sizeof(char) * BUFFER_SIZE, 0,
            reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (n <= 0) {
            delete[] recvbuf;
            break;
        }
        recvbuf[BUFFER_SIZE - 1] = 0;
        HIVIEW_LOGD("receive data from client %s", recvbuf);
        for (auto receiver = receivers.begin(); receiver != receivers.end(); receiver++) {
            (*receiver)->HandlerEvent(std::string(recvbuf));
        }
        delete[] recvbuf;
    }
    return 0;
}

int BBoxDevice::Close()
{
    if (fd_ > 0) {
        close(fd_);
        fd_ = -1;
    }
    return 0;
}

int BBoxDevice::Open()
{
    fd_ = open("/dev/bbox", O_RDONLY, 0);
    if (fd_ < 0) {
        HIVIEW_LOGE("open bbox fail error=%{public}d, msg=%{public}s", errno, strerror(errno));
        return -1;
    }
    return fd_;
}

uint32_t BBoxDevice::GetEvents()
{
    return EPOLLIN;
}

std::string BBoxDevice::GetName()
{
    return "BBox";
}

int BBoxDevice::ReceiveMsg(std::vector<std::shared_ptr<EventReceiver>> &receivers)
{
    char buffer[EVENT_READ_BUFFER];
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    int ret = read(fd_, buffer, EVENT_READ_BUFFER);
    if (ret <= 0) {
        return -1;
    }
    buffer[EVENT_READ_BUFFER - 1] = '\0';
    HIVIEW_LOGD("receive from kernal %{public}s", buffer);
    for (auto receiver = receivers.begin(); receiver != receivers.end(); receiver++) {
        (*receiver)->HandlerEvent(std::string(buffer));
    }
    return 0;
}

void EventServer::AddDev(std::shared_ptr<DeviceNode> dev)
{
    int fd = dev->Open();
    if (fd < 0) {
        HIVIEW_LOGI("open device %{public}s failed", dev->GetName().c_str());
        return;
    }
    devs_[fd] = dev;
}

int EventServer::OpenDevs()
{
    AddDev(std::make_shared<SocketDevice>());
    AddDev(std::make_shared<BBoxDevice>());
    if (devs_.empty()) {
        HIVIEW_LOGE("can not open any device");
        return -1;
    }
    HIVIEW_LOGI("has open %{public}u devices", devs_.size());
    return 0;
}

int EventServer::AddToMonitor(int pollFd, struct epoll_event pollEvents[])
{
    int index = 0;
    auto it = devs_.begin();
    while (it != devs_.end()) {
        HIVIEW_LOGI("add to poll device %{public}s, fd=%{public}d", it->second->GetName().c_str(), it->first);
        pollEvents[index].data.fd = it->first;
        pollEvents[index].events = it->second->GetEvents();
        int ret = epoll_ctl(pollFd, EPOLL_CTL_ADD, it->first, &pollEvents[index]);
        if (ret < 0) {
            HIVIEW_LOGE("add to poll fail device %{public}s error=%{public}d, msg=%{public}s",
                it->second->GetName().c_str(), errno, strerror(errno));
            it->second->Close();
            it = devs_.erase(it);
        } else {
            it++;
        }
        index++;
    }

    if (devs_.empty()) {
        HIVIEW_LOGE("can not monitor any device");
        return -1;
    }
    HIVIEW_LOGI("monitor devices %{public}u", devs_.size());
    return 0;
}

void EventServer::Start()
{
    HIVIEW_LOGD("start event server");
    if (OpenDevs() < 0) {
        return;
    }

    int pollFd = epoll_create1(EPOLL_CLOEXEC);
    if (pollFd < 0) {
        HIVIEW_LOGE("create poll fail error=%{public}d, msg=%{public}s", errno, strerror(errno));
        return;
    }

    struct epoll_event pollEvents[devs_.size()];
    if (AddToMonitor(pollFd, pollEvents) < 0) {
        return;
    }

    HIVIEW_LOGI("go into event loop");
    isStart_ = true;
    while (isStart_) {
        struct epoll_event chkPollEvents[devs_.size()];
        int eventCount = epoll_wait(pollFd, chkPollEvents, devs_.size(), 10000); // 10000: 10 seconds
        if (eventCount <= 0) {
            HIVIEW_LOGD("read event timeout");
            continue;
        }
        for (int ii = 0; ii < eventCount; ii++) {
            auto it = devs_.find(chkPollEvents[ii].data.fd);
            it->second->ReceiveMsg(receivers_);
        }
    }
    CloseDevs();
}

void EventServer::CloseDevs()
{
    for (auto devItem : devs_) {
        devItem.second->Close();
    }
}

void EventServer::Stop()
{
    isStart_ = false;
}

void EventServer::AddReceiver(std::shared_ptr<EventReceiver> receiver)
{
    receivers_.emplace_back(receiver);
}
} // namespace HiviewDFX
} // namespace OHOS
