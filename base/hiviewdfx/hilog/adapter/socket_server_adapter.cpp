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

#ifdef USING_EXISTING_SOCKET
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <vector>

#define OHOS_PROC_FD_DIR "/proc/self/fd"
#define OHOS_SOCKET_DIR "/dev/socket/"
#define OHOS_SOCKET_FILE_PREFIX "socket"

static constexpr int DECIMAL = 10;

static std::string GetFileNameByFd(const int fd)
{
    if (fd <= 0) {
        return std::string();
    }

    std::string fdPath = "/proc/self/fd/" + std::to_string(fd);
    char fileName[PATH_MAX + 1] = {0};

    int ret = readlink(fdPath.c_str(), fileName, PATH_MAX);
    if (ret < 0 || ret > PATH_MAX) {
        return std::string();
    }
    fileName[ret] = '\0';
    return std::string(fileName);
}

static void GetSocketFds(std::vector<int> &fds)
{
    struct dirent *dp = nullptr;
    DIR *dir = opendir(OHOS_PROC_FD_DIR);
    if (!dir) {
        return;
    }

    while ((dp = readdir(dir)) != nullptr) {
        int fd = -1;
        if (dp->d_name[0] == '.') {
            continue;
        }
        fd = strtol(dp->d_name, nullptr, DECIMAL);
        auto name = GetFileNameByFd(fd);
        if (strncmp(name.c_str(), OHOS_SOCKET_FILE_PREFIX, strlen(OHOS_SOCKET_FILE_PREFIX))) {
            continue;
        }
        if (TEMP_FAILURE_RETRY(fcntl(fd, F_GETFD)) >= 0) {
            fds.push_back(fd);
        }
    }
    closedir(dir);
}

static bool CheckSocketType(int fd, int type)
{
    int soType = -1;
    socklen_t optlen = static_cast<socklen_t>(sizeof(soType));
    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &soType, &optlen)) {
        return false;
    }

    return (type == soType);
}

int GetExistingSocketServer(const char *name, int type)
{
    std::vector<int> fds;
    if (name == nullptr) {
        return -1;
    }

    GetSocketFds(fds);
    std::string socketPath(OHOS_SOCKET_DIR);
    socketPath += name;

    for (auto fd : fds) {
        struct sockaddr_un addr;
        socklen_t addrLen = static_cast<socklen_t>(sizeof(addr));

        if (!CheckSocketType(fd, type)) {
            continue;
        }

        int ret = getsockname(fd, (struct sockaddr*)&addr, &addrLen);
        if (ret < 0) {
            return -1;
        }
        if (socketPath.compare(addr.sun_path) == 0) {
            return fd;
        }
    }

    return -1;
}
#else
int GetExistingSocketServer(const char *, int)
{
    return -1;
}
#endif
