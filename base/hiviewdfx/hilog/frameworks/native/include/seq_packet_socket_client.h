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

#ifndef SEQ_PACKET_SOCKET_CLIENT_H
#define SEQ_PACKET_SOCKET_CLIENT_H

#include "socket_client.h"

namespace OHOS {
namespace HiviewDFX {
enum SeqPacketSocketResult {
    CREATE_AND_CONNECTED = 0,
    CREATE_SOCKET_FAILED,
    CONNECT_FAILED,
};

class SeqPacketSocketClient : public SocketClient {
public:
    SeqPacketSocketClient(std::string serverPath, int socketOption) : SocketClient(serverPath, SOCK_SEQPACKET)
    {
        int socketType = SOCK_SEQPACKET | ((uint32_t)socketOption & (uint32_t)allowOption);
        SetType(socketType);
    }
    ~SeqPacketSocketClient() = default;
    SeqPacketSocketResult Init();
    int RecvMsg(char *buffer, unsigned int len);
private:
    int allowOption = SOCK_NONBLOCK | SOCK_CLOEXEC;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SEQ_PACKET_SOCKET_CLIENT_H */
