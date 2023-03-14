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


#ifndef OHOS_DHCP_SYSTEM_FUNC_MOCK_H
#define OHOS_DHCP_SYSTEM_FUNC_MOCK_H

#include <gmock/gmock.h>
#include <stdint.h>
#include <dlfcn.h>
#include <unistd.h>

#ifdef __OHOS__
typedef int socklen_t;
#endif // __OHOS__

using ::testing::_;
using ::testing::Return;

namespace OHOS {
namespace Wifi {
class SystemFuncMock {
public:
    MOCK_METHOD3(socket, int(int __domain, int __type, int __protocol));
    MOCK_METHOD5(setsockopt, int(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen));
    MOCK_METHOD5(select, int(int __nfds, fd_set *__readfds, fd_set *__writefds, fd_set *__exceptfds,
        struct timeval *__timeout));
    MOCK_METHOD3(bind, int(int __fd, struct sockaddr *__addr, socklen_t __len));

    MOCK_METHOD6(sendto, ssize_t(int __fd, const void *__buf, size_t __n, int __flags, struct sockaddr *__addr,
        socklen_t __addr_len));
    MOCK_METHOD6(recvfrom, ssize_t(int __fd, void *__buf, size_t __n, int __flags, struct sockaddr *__addr,
        socklen_t *__addr_len));
    MOCK_METHOD1(close, int(int _fileno));
    static SystemFuncMock &GetInstance(void);
    static void SetMockFlag(bool flag);
    static bool GetMockFlag(void);
private:
    SystemFuncMock();
    ~SystemFuncMock();
};
}  // namespace Wifi
}  // namespace OHOS
#endif