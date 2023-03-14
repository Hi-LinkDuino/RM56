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

#include "socket.h"

#include <cerrno>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

namespace OHOS {
namespace HiviewDFX {
Socket::Socket(int socketType) : socketType(socketType) {}

void Socket::SetType(uint32_t socketOption)
{
    socketType = socketOption;
}

int Socket::GenerateFD()
{
    int tmpFd = TEMP_FAILURE_RETRY(socket(AF_UNIX, socketType, 0));
    int res = tmpFd;
    if (tmpFd == 0) {
        res = TEMP_FAILURE_RETRY(socket(AF_UNIX, socketType, 0));
        close(tmpFd);
    }
    return res;
}

int Socket::Create()
{
    if (socketHandler != 0) {
        return socketHandler;
    }

    int fd = TEMP_FAILURE_RETRY(socket(AF_UNIX, socketType, 0));
    if (fd < 0) {
#ifdef DEBUG
        std::cout << "Create socket failed: " <<  fd << std::endl;
#endif
        return fd;
    }

    socketHandler = fd;
    return socketHandler;
}

int Socket::Poll()
{
    return -1;
}

int Socket::Write(const char *data, unsigned int len)
{
    if (data == nullptr) {
        return -1;
    }

    return write(socketHandler, data, len);
}

int Socket::WriteAll(const char *data, unsigned int len)
{
    const char *ptr = data;
    int sizeLeft = static_cast<int>(len);
    int midRes = 0;

    if (data == nullptr) {
        return -1;
    }

    while (sizeLeft > 0) {
        midRes = Write(ptr, sizeLeft);
        if (midRes < 0) {
            break;
        }
        sizeLeft -= midRes;
        ptr += midRes;
    }

    return (midRes < 0) ? midRes : len;
}


int Socket::WriteV(const iovec *vec, unsigned int len)
{
    return TEMP_FAILURE_RETRY(::writev(socketHandler, vec, len));
}

int Socket::Read(char *buffer, unsigned int len)
{
    return TEMP_FAILURE_RETRY(read(socketHandler, buffer, len));
}

int Socket::Recv(void *buffer, unsigned int bufferLen, int flags)
{
    return TEMP_FAILURE_RETRY(recv(socketHandler, buffer, bufferLen, flags));
}

bool Socket::setHandler(int handler)
{
    if (socketHandler > 0) {
        return false;
    }
    socketHandler = handler;
    return true;
}

bool Socket::closeHandler()
{
    if (socketHandler > 0) {
        close(socketHandler);
        socketHandler = -1;
        return true;
    }
    return false;
}

Socket::~Socket()
{
    close(socketHandler);
    socketHandler = -1;
}
} // namespace HiviewDFX
} // namespace OHOS
