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

#include "socket_listener.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "log.h"

namespace OHOS {
namespace bluetooth {
SocketThread &SocketThread::GetInstance()
{
    static SocketThread instance;
    return instance;
}

bool SocketThread::Initialize()
{
    LOG_INFO("[SocketListener]: %{public}s", __func__);
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    stopFd_ = eventfd(0, 0);
    if (epollFd_ == -1 || stopFd_ == -1) {
        LOG_ERROR("[SocketListener]: Create epoll failed!!");
        return false;
    }

    int ret;
    struct epoll_event event = {};
    event.data.ptr = nullptr;
    event.events = EPOLLIN;

    CHECK_EXCEPT_INTR(ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, stopFd_, &event));
    if (ret == -1) {
        LOG_ERROR("[SocketListener]: Epoll add stop-event failed!!");
        return false;
    }

    thread_ = std::make_unique<std::thread>(&SocketThread::OnListenEvents, this);
    count_ = 0;

    return true;
}

bool SocketThread::Uninitialize()
{
    LOG_INFO("[SocketListener]: %{public}s", __func__);
    eventfd_write(stopFd_, 1);
    if (thread_ && thread_->joinable()) {
        thread_->join();
        thread_ = nullptr;
    }

    std::lock_guard<std::mutex> lock(epollFdMutex_);
    if (epollFd_ != -1) {
        close(epollFd_);
        epollFd_ = -1;
    }
    if (stopFd_ != -1) {
        close(stopFd_);
        stopFd_ = -1;
    }
    return true;
}

bool SocketThread::AddSocket(int fd, int flag, Socket &sock)
{
    LOG_INFO("[SocketListener]: %{public}s", __func__);

    if (count_ >= MAX_EPOLL) {
        LOG_ERROR("[SocketListener]: Too many sockets..");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        fdMap_[&sock] = fd;
    }

    int ret;
    struct epoll_event event = {};
    event.data.ptr = &sock;
    if (flag == 1) {
        event.events = EPOLLRDHUP | EPOLLERR | EPOLLIN | EPOLLOUT | EPOLLONESHOT;
        ret = epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event);
    } else {
        count_++;
        event.events = EPOLLRDHUP | EPOLLERR | EPOLLIN;
        ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event);
    }

    LOG_INFO("[SocketListener]: epollFd: %{public}d, fd:%{public}d, errno:%{public}d", epollFd_, fd, errno);
    if (ret == -1) {
        LOG_ERROR("[SocketListener]: add Socket-Event failed");
        return false;
    }

    return true;
}

void SocketThread::OnListenEvents(void)
{
    struct epoll_event events[MAX_EPOLL];

    while (true) {
        int nfds;
        CHECK_EXCEPT_INTR(nfds = epoll_wait(epollFd_, events, MAX_EPOLL, -1));
        if (nfds == -1) {
            return;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.ptr == nullptr) {
                eventfd_t val;
                eventfd_read(this->stopFd_, &val);
                return;
            }

            Socket &sock = *(Socket *)events[i].data.ptr;
            if (events[i].events & EPOLLIN) {
                readCallback_(sock);
            }
            std::lock_guard<std::mutex> lock(epollFdMutex_);
            int fd = FindFd(sock);
            if (events[i].events & EPOLLOUT) {
                writeCallback_(sock);

                struct epoll_event event = {};
                event.data.ptr = &sock;
                event.events = EPOLLRDHUP | EPOLLERR | EPOLLIN;
                epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event);
                LOG_DEBUG("[SocketListener]: epollFd: %{public}d, fd:%{public}d, errno:%{public}d", epollFd_, fd, errno);
            }
            if (events[i].events & (EPOLLRDHUP | EPOLLERR)) {
                LOG_INFO("[SocketListener]: remove fd:%{public}d", fd);
                epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
                DeleteFd(sock);
                count_--;
                LOG_INFO("[SocketListener]: exceptCallback");
                exceptCallback_(sock);
            }
        }
    }
}

int SocketThread::FindFd(Socket &sock)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int fd = -1;
    if (fdMap_.find(&sock) != fdMap_.end()) {
        fd = fdMap_.at(&sock);
    } else {
        LOG_ERROR("[SocketListener]: fd does not exist");
    }
    return fd;
}

void SocketThread::DeleteFd(Socket &sock)
{
    std::lock_guard<std::mutex> lock(mutex_);
    fdMap_.erase(&sock);
}

bool SocketThread::DeleteSocket(Socket &sock)
{
    int fd = FindFd(sock);
    LOG_DEBUG("SocketThread: DeleteSocket fd:%{public}d", fd);
    std::lock_guard<std::mutex> lock(epollFdMutex_);
    if ((fd == -1) || (epollFd_ == -1)) {
        return false;
    }
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) != -1) {
        DeleteFd(sock);
        count_--;
        return true;
    }
    LOG_DEBUG("SocketThread: DeleteSocket errno:%{public}d", errno);
    return false;
}
}  // namespace bluetooth
}  // namespace OHOS