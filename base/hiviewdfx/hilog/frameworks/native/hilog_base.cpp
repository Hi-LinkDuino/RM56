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

#include "hilog_base/log_base.h"

#include <hilog_common.h>
#include <vsnprintf_s_p.h>

#include <array>
#include <atomic>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>

namespace {

constexpr int SOCKET_TYPE = SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC;
constexpr int INVALID_SOCKET = -1;
constexpr sockaddr_un SOCKET_ADDR = {AF_UNIX, SOCKET_FILE_DIR INPUT_SOCKET_NAME};

struct SocketHandler {
    std::atomic_int socketFd {INVALID_SOCKET};
    std::atomic_bool isConnected {false};
    ~SocketHandler()
    {
        int currentFd = socketFd.exchange(INVALID_SOCKET);
        if (currentFd >= 0) {
            close(currentFd);
        }
    }
};

static int GenerateFD()
{
    int tmpFd = TEMP_FAILURE_RETRY(socket(AF_UNIX, SOCKET_TYPE, 0));
    int res = tmpFd;
    if (tmpFd == 0) {
        res = TEMP_FAILURE_RETRY(socket(AF_UNIX, SOCKET_TYPE, 0));
        close(tmpFd);
    }
    return res;
}

static int CheckSocket(SocketHandler& socketHandler)
{
    int currentFd = socketHandler.socketFd.load();
    if (currentFd >= 0) {
        return currentFd;
    }

    int fd = GenerateFD();
    if (fd < 0) {
        std::cerr << __FILE__ << __LINE__ << " Can't create socket! Errno: " << errno << "\n";
        return fd;
    }

    currentFd = INVALID_SOCKET;
    if (!socketHandler.socketFd.compare_exchange_strong(currentFd, fd)) {
        close(fd);
        return currentFd;
    }
    return fd;
}

static int CheckConnection(SocketHandler& socketHandler)
{
    bool isConnected = socketHandler.isConnected.load();
    if (isConnected) {
        return 0;
    }

    isConnected = socketHandler.isConnected.load();
    if (isConnected) {
        return 0;
    }

    auto result = TEMP_FAILURE_RETRY(connect(socketHandler.socketFd.load(),
        reinterpret_cast<const sockaddr*>(&SOCKET_ADDR), sizeof(SOCKET_ADDR)));
    if (result < 0) {
        std::cerr << __FILE__ << __LINE__ << " Can't connect to server. Errno: " << errno << "\n";
        return result;
    }
    socketHandler.isConnected.store(true);
    return 0;
}

static int SendMessage(HilogMsg *header, const char *tag, uint16_t tagLen, const char *fmt, uint16_t fmtLen)
{
    SocketHandler socketHandler;
    int ret = CheckSocket(socketHandler);
    if (ret < 0) {
        return ret;
    }
    ret = CheckConnection(socketHandler);
    if (ret < 0) {
        return ret;
    }

    struct timeval tv = {0};
    gettimeofday(&tv, nullptr);
    header->tv_sec = static_cast<uint32_t>(tv.tv_sec);
    header->tv_nsec = static_cast<uint32_t>(tv.tv_usec * 1000);     // 1000 : usec convert to nsec
    header->len = sizeof(HilogMsg) + tagLen + fmtLen;
    header->tag_len = tagLen;

    std::array<iovec,3> vec;
    vec[0].iov_base = header;                                             // 0 : index of hos log header
    vec[0].iov_len = sizeof(HilogMsg);                                    // 0 : index of hos log header
    vec[1].iov_base = reinterpret_cast<void*>(const_cast<char*>(tag));    // 1 : index of log tag
    vec[1].iov_len = tagLen;                                              // 1 : index of log tag
    vec[2].iov_base = reinterpret_cast<void*>(const_cast<char*>(fmt));    // 2 : index of log content
    vec[2].iov_len = fmtLen;                                              // 2 : index of log content
    ret = TEMP_FAILURE_RETRY(::writev(socketHandler.socketFd.load(), vec.data(), vec.size()));
    return ret;
}

static int HiLogBasePrintArgs(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
    const char *fmt, va_list ap)
{
    if (!HiLogBaseIsLoggable(domain, tag, level)) {
        return -1;
    }

    char buf[MAX_LOG_LEN] = {0};

    vsnprintfp_s(buf, MAX_LOG_LEN, MAX_LOG_LEN - 1, HILOG_DEFAULT_PRIVACY, fmt, ap);

    auto tagLen = strnlen(tag, MAX_TAG_LEN - 1);
    auto logLen = strnlen(buf, MAX_LOG_LEN - 1);
    HilogMsg header = {0};
    header.type = type;
    header.level = level;
#ifndef __RECV_MSG_WITH_UCRED_
    header.pid = getpid();
#endif
    header.tid = static_cast<uint32_t>(gettid());
    header.domain = domain;

    return SendMessage(&header, tag, tagLen + 1, buf, logLen + 1);
}

} // namespace

int HiLogBasePrint(LogType type, LogLevel level, unsigned int domain, const char *tag, const char *fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = HiLogBasePrintArgs(type, level, domain, tag, fmt, ap);
    va_end(ap);
    return ret;
}

bool HiLogBaseIsLoggable(unsigned int domain, const char *tag, LogLevel level)
{
    if ((level <= LOG_LEVEL_MIN) || (level >= LOG_LEVEL_MAX) || tag == nullptr) {
        return false;
    }
    return true;
}
