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

#ifndef DGRAM_SOCKET_CLIENT_H
#define DGRAM_SOCKET_CLIENT_H

#include "socket_client.h"

namespace OHOS {
namespace HiviewDFX {
class DgramSocketClient : public SocketClient {
public:
    DgramSocketClient(std::string serverPath, uint32_t socketOption)
        : SocketClient(serverPath, SOCK_DGRAM), fdHandler(-1)
    {
        socketType = (socketOption & allowOption);
        SetType(SOCK_DGRAM | socketType);
    }
    ~DgramSocketClient() = default;
    int CheckSocket();
private:
    std::atomic_int fdHandler;
    uint32_t allowOption = SOCK_NONBLOCK | SOCK_CLOEXEC;
    uint32_t socketType;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* DGRAM_SOCKET_CLIENT_H */
