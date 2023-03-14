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

#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include <sys/socket.h>
#include <sys/uio.h>

namespace OHOS {
namespace HiviewDFX {
class Socket {
public:
    explicit Socket(int socketType);
    ~Socket();
    bool closeHandler();
    bool setHandler(int socketHandler);
    void SetType(uint32_t socketOption);
    int GenerateFD();
    int Create();
    int Poll();
    int Write(const char *data, unsigned int len);
    int WriteAll(const char *data, unsigned int len);
    int WriteV(const iovec *vec, unsigned int len);
    int Read(char *buffer, unsigned int len);
    int Recv(void *buffer, unsigned int bufferLen, int flags = MSG_PEEK);
protected:
    int socketHandler = 0;
    uint32_t socketType;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SOCKET_H */
