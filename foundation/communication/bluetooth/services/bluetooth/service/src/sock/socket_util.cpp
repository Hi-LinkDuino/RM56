/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "socket_util.h"
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
int SocketUtil::SocketSendData(int sockFd, const uint8_t *buf, int len)
{
    int sendSize = len;

    while (sendSize) {
        ssize_t size = send(sockFd, buf, sendSize, 0);
        if (size <= 0) {
            return -1;
        }
        if (size > sendSize) {
            return -1;
        }
        buf += size;
        sendSize -= size;
    }
    return len;
}

int SocketUtil::SocketSendFd(int sockFd, const uint8_t *buf, int len, int acceptFd)
{
    LOG_INFO("[sock]%{public}s", __func__);

    if ((sockFd == -1 || acceptFd == -1)) {
        return -1;
    }

    int sendSize = len;
    struct msghdr msghdr = {};
    struct iovec iovec = {};
    struct cmsghdr *cmsghdr;
    char msgbuffer[CMSG_SPACE(1)] = {0};

    (void)memset_s(&msghdr, sizeof(msghdr), 0, sizeof(msghdr));
    (void)memset_s(&iovec, sizeof(iovec), 0, sizeof(iovec));

    msghdr.msg_control = msgbuffer;
    msghdr.msg_controllen = sizeof(msgbuffer);
    cmsghdr = CMSG_FIRSTHDR(&msghdr);
    cmsghdr->cmsg_len = CMSG_LEN(sizeof(acceptFd));
    cmsghdr->cmsg_type = SCM_RIGHTS;
    cmsghdr->cmsg_level = SOL_SOCKET;
    (void)memcpy_s(CMSG_DATA(cmsghdr), sizeof(acceptFd), &acceptFd, sizeof(acceptFd));

    while (len > 0) {
        iovec.iov_base = const_cast<void *>(static_cast<const void *>(buf));
        iovec.iov_len = len;

        msghdr.msg_iov = &iovec;
        msghdr.msg_iovlen = 1;

#ifdef DARWIN_PLATFORM
        ssize_t size = sendmsg(sockFd, &msghdr, 0);
#else
        ssize_t size = sendmsg(sockFd, &msghdr, MSG_NOSIGNAL);
#endif
        LOG_INFO("[sock]%{public}s sendmsg size:%{public}zu", __func__, size);
        if (size < 0) {
            sendSize = -1;
            break;
        }

        buf += size;
        len -= size;
        (void)memset_s(&msghdr, sizeof(msghdr), 0, sizeof(msghdr));
    }
    LOG_INFO("[sock]%{public}s close accept_fd:%{public}d after sent", __func__, acceptFd);
    return sendSize;
}
}  // namespace bluetooth
}  // namespace OHOS
