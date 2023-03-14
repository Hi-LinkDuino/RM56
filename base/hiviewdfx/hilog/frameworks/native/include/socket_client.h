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

#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <sys/un.h>
#include <sys/socket.h>
#include <string>
#include <atomic>
#include <memory>

#include "socket.h"

namespace OHOS {
namespace HiviewDFX {
class SocketClient : public Socket {
public:
    SocketClient(std::string serverPath, uint32_t socketType);
    ~SocketClient() = default;
    int Connect();
private:
    sockaddr_un serverAddr;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SOCKET_CLIENT_H */
