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
#include "mock_custom_func.h"
#include "dhcp_ipv4.h"
#include "dhcp_options.h"
#include "dhcp_function.h"
#include "securec.h"

using namespace OHOS;

static bool g_mockTag = false;

MockCustomFunc &MockCustomFunc::GetInstance()
{
    static MockCustomFunc gMockCustomFunc;
    return gMockCustomFunc;
};

MockCustomFunc::MockCustomFunc()
{}

void MockCustomFunc::SetMockFlag(bool flag)
{
    g_mockTag = flag;
}

bool MockCustomFunc::GetMockFlag(void)
{
    return g_mockTag;
}

#ifdef __cplusplus
extern "C" {
#endif
static int AddOptDoubleValueToOpts(uint8_t *pOpts, uint8_t code, uint32_t value1, uint32_t value2)
{
    uint8_t uOption[DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + DHCP_UINT32_BYTES * 2] = {0};
    uOption[DHCP_OPT_CODE_INDEX] = code;
    uOption[DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES * 2;
    if ((memcpy_s(uOption + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &value1, DHCP_UINT32_BYTES) != EOK) ||
        (memcpy_s(uOption + DHCP_OPT_DATA_INDEX + DHCP_UINT32_BYTES,
            DHCP_UINT32_BYTES, &value2, DHCP_UINT32_BYTES) != EOK)) {
        return 0;
    }
    int nLen = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + uOption[DHCP_OPT_LEN_INDEX];
    return AddOptStrToOpts(pOpts, uOption, nLen);
}

int __real_GetDhcpRawPacket(struct DhcpPacket *getPacket, int rawFd);
int __wrap_GetDhcpRawPacket(struct DhcpPacket *getPacket, int rawFd)
{
    if (g_mockTag) {
        int nLen = MockCustomFunc::GetInstance().GetDhcpRawPacket(getPacket, rawFd);
        char cliIp[INET_ADDRSTRLEN] = "192.77.2.231";
        char serIp[INET_ADDRSTRLEN] = "192.77.2.1";
        uint32_t uCliIp = 0;
        uint32_t uSerIp = 0;
        Ip4StrConToInt(cliIp, &uCliIp, false);
        Ip4StrConToInt(serIp, &uSerIp, false);
        getPacket->xid = GetDhcpTransID();
        getPacket->yiaddr = uCliIp;
        if (nLen == 1) {
            GetPacketHeaderInfo(getPacket, DHCP_OFFER);
            AddOptValueToOpts(getPacket->options, SERVER_IDENTIFIER_OPTION, uSerIp);
        } else if (nLen == 2) {
            GetPacketHeaderInfo(getPacket, DHCP_ACK);
        } else if (nLen == 3) {
            GetPacketHeaderInfo(getPacket, DHCP_NAK);
        } else if (nLen == 4) {
            GetPacketHeaderInfo(getPacket, DHCP_ACK);
            AddOptValueToOpts(getPacket->options, SERVER_IDENTIFIER_OPTION, uSerIp);
            AddOptValueToOpts(getPacket->options, SUBNET_MASK_OPTION, 0);
            AddOptValueToOpts(getPacket->options, IP_ADDRESS_LEASE_TIME_OPTION, htonl(10));
            AddOptDoubleValueToOpts(getPacket->options, ROUTER_OPTION, uSerIp, uCliIp);
            AddOptDoubleValueToOpts(getPacket->options, DOMAIN_NAME_SERVER_OPTION, uSerIp, uCliIp);
        }
        return nLen;
    } else {
        return __real_GetDhcpRawPacket(getPacket, rawFd);
    }
}

int __real_GetDhcpKernelPacket(struct DhcpPacket *getPacket, int sockFd);
int __wrap_GetDhcpKernelPacket(struct DhcpPacket *getPacket, int sockFd)
{
    if (g_mockTag) {
        int nLen = MockCustomFunc::GetInstance().GetDhcpKernelPacket(getPacket, sockFd);
        char cliIp[INET_ADDRSTRLEN] = "192.77.3.231";
        char serIp[INET_ADDRSTRLEN] = "192.77.3.1";
        uint32_t uCliIp = 0;
        uint32_t uSerIp = 0;
        Ip4StrConToInt(cliIp, &uCliIp, false);
        Ip4StrConToInt(serIp, &uSerIp, false);
        getPacket->xid = GetDhcpTransID();
        getPacket->yiaddr = uCliIp;
        if (nLen == 1) {
            GetPacketHeaderInfo(getPacket, DHCP_OFFER);
            AddOptValueToOpts(getPacket->options, SERVER_IDENTIFIER_OPTION, uSerIp);
        } else if (nLen == 2) {
            GetPacketHeaderInfo(getPacket, DHCP_ACK);
            AddOptValueToOpts(getPacket->options, SERVER_IDENTIFIER_OPTION, uSerIp);
            AddOptValueToOpts(getPacket->options, SUBNET_MASK_OPTION, 0);
            AddOptValueToOpts(getPacket->options, IP_ADDRESS_LEASE_TIME_OPTION, htonl(4));
            AddOptDoubleValueToOpts(getPacket->options, ROUTER_OPTION, uSerIp, uCliIp);
            AddOptDoubleValueToOpts(getPacket->options, DOMAIN_NAME_SERVER_OPTION, uSerIp, uCliIp);
        } else if (nLen == 3) {
            GetPacketHeaderInfo(getPacket, DHCP_NAK);
        }
        return nLen;
    } else {
        return __real_GetDhcpKernelPacket(getPacket, sockFd);
    }
}
#ifdef __cplusplus
}
#endif
