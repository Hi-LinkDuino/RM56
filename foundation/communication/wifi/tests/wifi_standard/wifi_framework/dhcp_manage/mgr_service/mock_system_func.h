/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_MOCK_SYSTEM_FUNC_H
#define OHOS_MOCK_SYSTEM_FUNC_H

#include <gmock/gmock.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>

using ::testing::_;
using ::testing::Return;

namespace OHOS {
namespace Wifi {
class MockSystemFunc {
public:
    MOCK_METHOD2(open, int(const char *__file, int __oflag));
    MOCK_METHOD1(close, int(int));
    MOCK_METHOD3(write, ssize_t(int fd, const void *buf, size_t count));
    MOCK_METHOD3(read, ssize_t(int fd, void *buf, size_t count));
    MOCK_METHOD3(socket, int(int __domain, int __type, int __protocol));
    MOCK_METHOD5(setsockopt, int(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen));
    MOCK_METHOD3(ioctl, int(int __fd, unsigned long __request, struct sockaddr *__ifreq));
    MOCK_METHOD3(bind, int(int __fd, const struct sockaddr *__addr, socklen_t __len));
    MOCK_METHOD2(listen, int(int __fd, int __n));
    MOCK_METHOD3(connect, int(int __fd, const struct sockaddr *__addr, socklen_t __len));
    MOCK_METHOD5(select, int(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout));
    MOCK_METHOD6(
        sendto, ssize_t(int fd, const void *buf, size_t count, int flags, const struct sockaddr *addr, socklen_t len));

    MOCK_METHOD0(vfork, pid_t(void));
    MOCK_METHOD2(execv, int(const char *__path, char *const *__argv));
    MOCK_METHOD1(_exit, void(int status));
    MOCK_METHOD3(waitpid, pid_t(pid_t pid, int *status, int options));
    MOCK_METHOD2(kill, int(pid_t pid, int sig));

    static MockSystemFunc &GetInstance(void);
    static void SetMockFlag(bool flag);
    static bool GetMockFlag(void);

private:
    MockSystemFunc();
    ~MockSystemFunc(){}
};
}  // namespace Wifi
}  // namespace OHOS

extern "C" {}

#endif