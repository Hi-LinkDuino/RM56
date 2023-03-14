/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "net.h"
#include "log.h"
#undef LOG_TAG
#define LOG_TAG "WifiRpcNet"

int SetNonBlock(int fd, int type)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        LOGE("get socket flags failed!");
        return -1;
    }
    if (type == 1) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    if (fcntl(fd, F_SETFL, flags) < 0) {
        LOGE("set socket O_NONBLOCK failed!");
        return -1;
    }
    return 0;
}

int MyRead(int fd, char *buf, int count)
{
    int pos = 0;
    while (count > 0) {
        int ret = read(fd, buf + pos, count);
        if (ret == 0) {
            buf[pos] = 0;
            return SOCK_CLOSE;
        } else if (ret < 0) {
            if (errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN) {
                break;
            } else {
                LOGE("read failed! error is %{public}d", errno);
                return SOCK_ERR;
            }
        } else {
            pos += ret;
            count -= ret;
        }
    }
    buf[pos] = 0;
#ifdef DEBUG
    LOGD("read: %{private}s", buf);
#endif
    return pos;
}

int MyWrite(int fd, const char *buf, int count)
{
    int pos = 0;
    while (count > 0) {
        int ret = write(fd, buf + pos, count);
        if (ret == 0) {
            return SOCK_CLOSE;
        } else if (ret < 0) {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            } else {
                LOGE("write failed! error is %{public}d", errno);
                return SOCK_ERR;
            }
        }
#ifdef DEBUG
        int tmpSize = ret + 1;
        char *szTmp = (char *)calloc(tmpSize, sizeof(char));
        if (szTmp != NULL) {
            if (strncpy_s(szTmp, tmpSize, buf + pos, ret) == EOK) {
                LOGD("write: %{private}s", szTmp);
            }
            free(szTmp);
            szTmp = NULL;
        }
#endif
        pos += ret;
        count -= ret;
    }
    return pos;
}

static int CreateSocket(int domain)
{
    int sock = socket(domain, SOCK_STREAM, 0);
    if (sock < 0) {
        LOGE("create socket failed!");
        return -1;
    }
    return sock;
}

int CreateUnixServer(const char *path, int backlog)
{
    struct sockaddr_un sockAddr;
    if (memset_s(&sockAddr, sizeof(sockAddr), 0, sizeof(sockAddr)) != EOK) {
        return -1;
    }
    sockAddr.sun_family = AF_LOCAL;
    if (strncpy_s(sockAddr.sun_path, sizeof(sockAddr.sun_path), path, strlen(path)) != EOK) {
        return -1;
    }
    int sock = CreateSocket(AF_LOCAL);
    if (sock < 0) {
        return -1;
    }
    int keepAlive = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
    int reuseaddr = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&reuseaddr, sizeof(reuseaddr));
    if (bind(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0) {
        LOGE("bind failed!");
        close(sock);
        return -1;
    }
    if (SetNonBlock(sock, 1) != 0) {
        LOGE("set socket non block failed!");
        close(sock);
        return -1;
    }
    fcntl(sock, F_SETFD, FD_CLOEXEC);
    if (listen(sock, backlog) < 0) {
        LOGE("listen failed!");
        close(sock);
        return -1;
    }
    return sock;
}

int ConnectUnixServer(const char *path)
{
    struct sockaddr_un sockAddr;
    if (memset_s(&sockAddr, sizeof(sockAddr), 0, sizeof(sockAddr)) != EOK) {
        return -1;
    }
    sockAddr.sun_family = AF_LOCAL;
    if (strncpy_s(sockAddr.sun_path, sizeof(sockAddr.sun_path), path, strlen(path)) != EOK) {
        return -1;
    }
    int sock = CreateSocket(AF_LOCAL);
    if (sock < 0) {
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0) {
        LOGE("connect failed!");
        close(sock);
        return -1;
    }
    return sock;
}

int WaitFdEvent(int fd, unsigned int mask, int milliseconds)
{
    struct pollfd pFd = {0};
    pFd.fd = fd;
    if (mask & READ_EVENT) {
        pFd.events |= POLLIN;
    }
    if (mask & WRIT_EVENT) {
        pFd.events |= POLLOUT;
    }
    int ret = poll(&pFd, 1, milliseconds);
    if (ret < 0) {
        LOGE("poll failed!");
        return -1;
    } else if (ret == 0) {
        return 0;
    } else {
        return 1;
    }
}
