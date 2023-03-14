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

#include "seq_packet_socket_server.h"

#include <cstring>
#include <iostream>
#include <thread>

namespace OHOS {
namespace HiviewDFX {
int SeqPacketSocketServer::StartAcceptingConnection(AcceptingHandler onAccepted)
{
    int listeningStatus = Listen(maxListenNumber);
    if (listeningStatus < 0) {
#ifdef DEBUG
        std::cerr << "Socket listen failed: ";
        HilogPrintError(listeningStatus);
#endif
        return listeningStatus;
    }

    return AcceptingLoop(onAccepted);
}

int SeqPacketSocketServer::AcceptingLoop(AcceptingHandler func)
{
    int acceptedSockedFd = 0;
    while ((acceptedSockedFd = Accept()) > 0) {
        std::unique_ptr<Socket> handler = std::make_unique<Socket>(SOCK_SEQPACKET);
        if (handler != nullptr) {
            handler->setHandler(acceptedSockedFd);
            func(std::move(handler));
        }
    }
    int acceptError = errno;
#ifdef DEBUG
    std::cerr << "Socket accept failed: ";
    HilogPrintError(acceptError);
#endif

    return acceptError;
}
} // namespace HiviewDFX
} // namespace OHOS
