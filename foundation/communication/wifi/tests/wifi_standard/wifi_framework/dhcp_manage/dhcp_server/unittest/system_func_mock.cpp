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

#include "system_func_mock.h"
#include "address_utils.h"
#include "dhcp_define.h"
#include "dhcp_ipv4.h"
#include "dhcp_logger.h"
#include "dhcp_message.h"
#include "dhcp_message_sim.h"
#include "dhcp_option.h"
#include "securec.h"
#include <stdint.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>


using namespace OHOS::Wifi;

#define MAGIC_COOKIE_LENGTH 4
#define OPT_HEADER_LENGTH 2
#define TIME_SEC_TO_USEC (1000 * 1000)
#define DHCP_SEL_WAIT_TIMEOUTS 1500

#undef LOG_TAG
#define LOG_TAG "DhcpServerSystemFuncMock"

static bool g_mockTag = false;

SystemFuncMock &SystemFuncMock::GetInstance()
{
    static SystemFuncMock gSystemFuncMock;
    return gSystemFuncMock;
};

SystemFuncMock::SystemFuncMock() {}

SystemFuncMock::~SystemFuncMock() {}

void SystemFuncMock::SetMockFlag(bool flag)
{
    g_mockTag = flag;
}

bool SystemFuncMock::GetMockFlag(void)
{
    return g_mockTag;
}

extern "C" {
struct in_addr {
    uint32_t s_addr;
};

struct sockaddr_in {
    short int sin_family;
    unsigned short int sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};

int __real_socket(int __domain, int __type, int __protocol);
int __wrap_socket(int __domain, int __type, int __protocol)
{
    LOGD("==>socket.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        return SystemFuncMock::GetInstance().socket(__domain, __type, __protocol);
    }
    return __real_socket(__domain, __type, __protocol);
}

int __real_setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen);
int __wrap_setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen)
{
    LOGD("==>setsockopt.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        return SystemFuncMock::GetInstance().setsockopt(__fd, __level, __optname, __optval, __optlen);
    }
    return __real_setsockopt(__fd, __level, __optname, __optval, __optlen);
}
int __real_select(int __nfds, fd_set *__readfds, fd_set *__writefds, fd_set *__exceptfds, struct timeval *__timeout);
int __wrap_select(int __nfds, fd_set *__readfds, fd_set *__writefds, fd_set *__exceptfds, struct timeval *__timeout)
{
    const unsigned int SLEEP_TIEM = 300000;
    LOGD("==>select.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        LOGD("message queue total: %d.", DhcpMsgManager::GetInstance().SendTotal());
        if (DhcpMsgManager::GetInstance().SendTotal() > 0) {
            FD_CLR(__nfds, __readfds);
            usleep(SLEEP_TIEM);
            return 1;
        }
        int retval = SystemFuncMock::GetInstance().select(__nfds, __readfds, __writefds, __exceptfds, __timeout);
        if (retval == 0) {
            if (__timeout != nullptr) {
                usleep(DHCP_SEL_WAIT_TIMEOUTS * 1000);
                LOGD("select time out.");
            }
        }
        return retval;
    }
    return __real_select(__nfds, __readfds, __writefds, __exceptfds, __timeout);
}

int __real_bind(int __fd, struct sockaddr *__addr, socklen_t __len);
int __wrap_bind(int __fd, struct sockaddr *__addr, socklen_t __len)
{
    LOGD("==>bind.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        return SystemFuncMock::GetInstance().bind(__fd, __addr, __len);
    }
    return __real_bind(__fd, __addr, __len);
}

int __real_close(int _fileno);
int __wrap_close(int _fileno)
{
    LOGD("==>close.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        return SystemFuncMock::GetInstance().close(_fileno);
    }
    return __real_close(_fileno);
}

ssize_t recvfrom(int __fd, void *__buf, size_t __n, int __flags, struct sockaddr *__addr, socklen_t *__addr_len)
{
    LOGD("==>recvfrom.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        if (DhcpMsgManager::GetInstance().SendTotal() > 0 && __buf) {
            LOGD("== new message received.");
            DhcpMessage msg = { 0 };
            if (DhcpMsgManager::GetInstance().FrontSendMsg(&msg)) {
                (void)memcpy_s(__buf, __n, &msg, sizeof(DhcpMessage));
                DhcpMsgManager::GetInstance().PopSendMsg();
                uint32_t srcIp = DhcpMsgManager::GetInstance().GetClientIp();
                if (__addr != nullptr && srcIp != 0) {
                    struct sockaddr_in *sAddr = (struct sockaddr_in *)__addr;
                    sAddr->sin_addr.s_addr = HostToNetwork(srcIp);
                    DhcpMsgManager::GetInstance().SetClientIp(0);
                }
                return sizeof(DhcpMessage);
            }
        }
    }
    return SystemFuncMock::GetInstance().recvfrom(__fd, __buf, __n, __flags, __addr, __addr_len);
}

int ParseMockOptions(DhcpMessage *packet)
{
    if (packet == nullptr) {
        LOGD("dhcp message pointer is null.");
        return RET_FAILED;
    }
    DhcpMsgInfo reply;
    if (memset_s(&reply, sizeof(DhcpMsgInfo), 0, sizeof(DhcpMsgInfo)) != EOK) {
        LOGD("failed to reset dhcp message info.");
        return RET_FAILED;
    }
    int retval = RET_FAILED;
    if (memcpy_s(&reply.packet, sizeof(reply.packet), packet, sizeof(DhcpMessage)) != EOK) {
        LOGD("failed to fill dhcp message.");
        return RET_FAILED;
    }
    reply.length = sizeof(DhcpMessage);
    if (InitOptionList(&reply.options) != RET_SUCCESS) {
        LOGD("failed to init dhcp option list.");
        return retval;
    }
    FreeOptionList(&reply.options);
    return retval;
}
ssize_t sendto(int __fd, const void *__buf, size_t __n, int __flags, struct sockaddr *__addr, socklen_t __addr_len)
{
    LOGD("==>sendto.");
    if (g_mockTag) {
        LOGD(" ==>mock enable.");
        if (__buf == nullptr) {
            return SystemFuncMock::GetInstance().sendto(__fd, __buf, __n, __flags, __addr, __addr_len);
        }
    }
    return SystemFuncMock::GetInstance().sendto(__fd, __buf, __n, __flags, __addr, __addr_len);
}
}