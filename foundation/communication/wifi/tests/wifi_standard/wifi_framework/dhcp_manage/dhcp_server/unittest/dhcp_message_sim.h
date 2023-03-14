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

#include <queue>
#include <mutex>
#include "dhcp_define.h"
#include "dhcp_message.h"

#ifndef OHOS_DHCP_MESSAGE_SIM_H
#define OHOS_DHCP_MESSAGE_SIM_H

typedef struct DhcpClientConfig DhcpClientConfig;

typedef struct DhcpClientContext DhcpClientContext;


namespace OHOS {
namespace Wifi {
class DhcpMsgManager
{
public:
    static DhcpMsgManager &GetInstance(void);

    int SendTotal();

    int RecvTotal();

    int PushSendMsg(const DhcpMessage &msg);
    int PushRecvMsg(const DhcpMessage &msg);

    void PopSendMsg();

    bool FrontSendMsg(DhcpMessage *msg);
    void PopRecvMsg();

    void SetClientIp(uint32_t ipAddr);
    uint32_t GetClientIp() const;
private:
    DhcpMsgManager(){};
    ~DhcpMsgManager(){};

    std::mutex m_recvQueueLocker;
    std::queue<DhcpMessage> m_recvMessages;
    std::mutex m_sendQueueLocker;
    std::queue<DhcpMessage> m_sendMessages;
    uint32_t m_clientIpAddress = 0;
};
}  // namespace Wifi
}  // namespace OHOS

typedef struct {
    int (*OnReceivedOffer)(DhcpMessage *msg);
    int (*OnReceivedNak)( DhcpMessage *msg);
    int (*OnReceivedAck)(DhcpMessage *msg);
} DhcpClientCallback;

struct DhcpClientConfig
{
    char ifname[IFACE_NAME_SIZE];
    uint8_t chaddr[DHCP_HWADDR_LENGTH];
};

DhcpClientContext *InitialDhcpClient(DhcpClientConfig *config);

int *StatrDhcpClient(DhcpClientContext *config);

int SendDhcpMessage(DhcpClientContext *ctx, PDhcpMsgInfo msg);

int DhcpDiscover(DhcpClientContext *ctx);

int DhcpRequest(DhcpClientContext *ctx);

int DhcpInform(DhcpClientContext *ctx);

int DhcpDecline(DhcpClientContext *ctx);

int DhcpRelease(DhcpClientContext *ctx);

int StopDhcpClient(DhcpClientContext *ctx);

int GetDhcpClinetState(DhcpClientContext *ctx);

int FreeDhcpClient(DhcpClientContext *ctx);
int InitMessage(DhcpClientContext *ctx, PDhcpMsgInfo msg, uint8_t msgType);
int FillHwAddr(uint8_t *dst, size_t dsize, uint8_t *src, size_t ssize);
int ParseDhcpOptions(PDhcpMsgInfo msg);
int ParseReceivedOptions(PDhcpMsgInfo msg);

#endif