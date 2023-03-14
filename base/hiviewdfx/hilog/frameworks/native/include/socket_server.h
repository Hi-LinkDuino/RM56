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

#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string>
#include <chrono>

#include "hilog_common.h"
#include "socket_client.h"

namespace OHOS {
namespace HiviewDFX {
class SocketServer {
public:
    SocketServer(const std::string& _socketName, uint32_t _socketType);
    virtual ~SocketServer();
    int Init();
    int Recv(void *buffer, unsigned int bufferLen, int flags = MSG_PEEK);
    int RecvMsg(struct msghdr *hdr, int flags = 0);
    int Listen(unsigned int backlog);
    int Poll(short inEvent, short& outEvent, const std::chrono::milliseconds& timeout);
    int Accept();
private:
    int socketHandler;
    uint32_t socketType;
    sockaddr_un serverAddr;
    std::string socketName;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SOCKET_SERVER_H */
