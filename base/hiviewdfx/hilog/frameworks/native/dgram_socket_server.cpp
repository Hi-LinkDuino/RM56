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

#include "dgram_socket_server.h"

#include <iostream>
#include <array>

namespace OHOS {
namespace HiviewDFX {
int DgramSocketServer::RecvPacket(std::vector<char>& buffer, struct ucred *cred)
{
    uint16_t packetLen = 0;
    if (auto status = Recv(&packetLen, sizeof(packetLen)); status < 0) {
        return status;
    }
    if (packetLen > maxPacketLength) {
        Recv(&packetLen, sizeof(packetLen), 0); // skip too long packet
        return 0;
    }
    buffer.resize(packetLen + 1);

    std::array<char, CMSG_SPACE(sizeof(struct ucred))> control = {0};

    struct msghdr msgh = {0};
    int ret = 0;
    if (cred != nullptr) {
        struct iovec iov;
        iov.iov_base = buffer.data();
        iov.iov_len = packetLen;
        msgh.msg_iov = &iov;
        msgh.msg_iovlen = 1;

        msgh.msg_control = control.data();
        msgh.msg_controllen = control.size();

        msgh.msg_name = nullptr;
        msgh.msg_namelen = 0;
        msgh.msg_flags = 0;

        ret = RecvMsg(&msgh);
    } else {
        ret = Recv(buffer.data(), packetLen, 0);
    }

    if (ret <= 0) {
        return ret;
    } else if (cred != nullptr) {
        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msgh);
        struct ucred *receivedUcred = (struct ucred*)CMSG_DATA(cmsg);
        if (receivedUcred == nullptr) {
            return 0;
        }
        *cred = *receivedUcred;
    }
    buffer[ret - 1] = 0;

    return ret;
}
} // namespace HiviewDFX
} // namespace OHOS
