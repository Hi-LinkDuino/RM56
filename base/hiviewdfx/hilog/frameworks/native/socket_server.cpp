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

#include "socket_server.h"

#include <cerrno>
#include <iostream>
#include <securec.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>

#include "socket_server_adapter.h"

extern "C" {
#include "init_socket.h"
}
namespace OHOS {
namespace HiviewDFX {
SocketServer::SocketServer(const std::string& _socketName, uint32_t socketType)
    : socketHandler(0), socketType(socketType), socketName(_socketName)
{
    serverAddr.sun_family = AF_UNIX;

    std::string sockPath(SOCKET_FILE_DIR);
    sockPath += socketName;
    if (strcpy_s(serverAddr.sun_path, sizeof(serverAddr.sun_path), sockPath.c_str()) != EOK) {
        return;
    }
}

int SocketServer::Init()
{
    if (socketName.length()) {
        socketHandler = GetControlSocket(socketName.c_str());
        if (socketHandler >= 0) {
            return socketHandler;
        }
    }

    socketHandler = TEMP_FAILURE_RETRY(socket(AF_UNIX, socketType, 0));
    if (socketHandler < 0) {
        return socketHandler;
    }

    (void)unlink(serverAddr.sun_path);

    int optval = 1;
    int ret = setsockopt(socketHandler, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval));
    if (ret < 0) {
        return ret;
    }

    return TEMP_FAILURE_RETRY(bind(socketHandler, (struct sockaddr *)&serverAddr, sizeof(sockaddr_un)));
}

int SocketServer::Recv(void *buffer, unsigned int bufferLen, int flags)
{
    return TEMP_FAILURE_RETRY(recv(socketHandler, buffer, bufferLen, flags));
}

int SocketServer::RecvMsg(struct msghdr *hdr, int flags)
{
    return recvmsg(socketHandler, hdr, flags);
}

int SocketServer::Listen(unsigned int backlog)
{
    return listen(socketHandler, backlog);
}

int SocketServer::Poll(short inEvent, short& outEvent, const std::chrono::milliseconds& timeout)
{
    pollfd info {socketHandler, inEvent, outEvent};
    int result = poll(&info, 1, timeout.count());
    outEvent = info.revents;
    return result;
}

int SocketServer::Accept()
{
    socklen_t addressSize = static_cast<socklen_t>(sizeof(serverAddr));
    return TEMP_FAILURE_RETRY(accept(socketHandler, (struct sockaddr*)&serverAddr, &addressSize));
}

SocketServer::~SocketServer()
{
    close(socketHandler);
    socketHandler = -1;
}
} // namespace HiviewDFX
} // namespace OHOS
