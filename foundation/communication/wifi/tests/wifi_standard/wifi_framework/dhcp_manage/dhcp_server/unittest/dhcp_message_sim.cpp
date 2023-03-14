/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "dhcp_message_sim.h"
#include <stdio.h>
#include <stdint.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include "dhcp_logger.h"
#include "dhcp_option.h"
#include "dhcp_ipv4.h"
#include "address_utils.h"
#include "securec.h"
#include "common_util.h"

#define OPT_MESSAGE_TYPE_LEGTH 1
#define OPT_HEADER_LENGTH 2
#define OPT_TIME_LENGTH 4
#define OPT_TYPE_FIELD_LENGTH 1
#define OPT_MAC_ADDR_LENGTH 6
#define MAGIC_COOKIE_LENGTH 4
#define OPT_BROADCAST_FLAG_ENABLE 0

#define OFFER_MIN_INTERVAL_TIME 5

#define PENDING_DEFAULT_TIMEOUT 1200
#define PENDING_DEFAULT_INTERVAL 1
#define PENDING_INTERVAL_CHECKING_ENABLE 1
#define DHCP_MAGIC_COOKIE 0x63825363
#define ALLOW_NOBINDING_REQUEST 1
#define REUSE_ADDRESS_ENABLE 1
#define WAIT_STOPED_TIME 5

#undef LOG_TAG
#define LOG_TAG "DhcpMessageSimulator"


using namespace OHOS::Wifi;

DhcpMsgManager& DhcpMsgManager::GetInstance()
{
    static DhcpMsgManager gMessageManager;
    return gMessageManager;
}

int DhcpMsgManager::SendTotal()
{
    int total = 0;
    m_sendQueueLocker.lock();
    total = m_sendMessages.size();
    m_sendQueueLocker.unlock();
    return total;
}

int DhcpMsgManager::RecvTotal()
{
    int total = 0;
    m_recvQueueLocker.lock();
    total = m_recvMessages.size();
    m_recvQueueLocker.unlock();
    return total;
}

bool DhcpMsgManager::FrontSendMsg(DhcpMessage *msg)
{
    int retval = false;
    if (!msg) {
        return retval;
    }
    m_sendQueueLocker.lock();
    if (!m_sendMessages.empty()) {
        DhcpMessage fmsg = m_sendMessages.front();
        if (memcpy_s(msg, sizeof(DhcpMessage), &fmsg, sizeof(DhcpMessage)) == EOK) {
            retval = true;
        }
    }
    m_sendQueueLocker.unlock();
    return retval;
}

void DhcpMsgManager::PopSendMsg()
{
    m_sendQueueLocker.lock();
    if (m_sendMessages.size() > 0) {
        m_sendMessages.pop();
    }
    m_sendQueueLocker.unlock();
}

void DhcpMsgManager::PopRecvMsg()
{
    m_recvQueueLocker.lock();
    if (!m_recvMessages.empty()) {
        m_recvMessages.pop();
    }
    m_recvQueueLocker.unlock();
}

int DhcpMsgManager::PushSendMsg(const DhcpMessage &msg)
{
    m_sendQueueLocker.lock();
    m_sendMessages.push(msg);
    m_sendQueueLocker.unlock();
    return 1;
}

int DhcpMsgManager::PushRecvMsg(const DhcpMessage &msg)
{
    m_recvQueueLocker.lock();
    m_recvMessages.push(msg);
    m_recvQueueLocker.unlock();
    return 1;
}

void DhcpMsgManager::SetClientIp(uint32_t ipAddr)
{
    m_clientIpAddress = ipAddr;
}
uint32_t DhcpMsgManager::GetClientIp() const
{
    return m_clientIpAddress;
}

struct DhcpClientContext
{
    int clientFd;
    int state;
    DhcpClientConfig config;
};

struct sockaddr_in *BroadcastAddrIn(void)
{
    static struct sockaddr_in broadcastAddrIn = {0};
    if (broadcastAddrIn.sin_port == 0) {
        broadcastAddrIn.sin_port = htons(DHCP_SERVER_PORT);
        broadcastAddrIn.sin_family = AF_INET;
        broadcastAddrIn.sin_addr.s_addr = INADDR_BROADCAST;
    }
    return &broadcastAddrIn;
}
struct sockaddr_in *DestinationAddr(void)
{
    static struct sockaddr_in destAddrIn = {0};
    if (destAddrIn.sin_port == 0) {
        destAddrIn.sin_port = htons(DHCP_SERVER_PORT);
        destAddrIn.sin_family = AF_INET;
        destAddrIn.sin_addr.s_addr = INADDR_BROADCAST;
    }
    return &destAddrIn;
}

int FillHwAddr(uint8_t *dst, size_t dsize, uint8_t *src, size_t ssize)
{
    if (!dst || !src) {
        return DHCP_FALSE;
    }
    if (ssize > dsize){
        return DHCP_FALSE;
    }
    if (memset_s(dst, dsize, 0, dsize) != EOK) {
        return DHCP_FALSE;
    }
    if (memcpy_s(dst, dsize, src, ssize) != EOK) {
        return DHCP_FALSE;
    }
    return DHCP_TRUE;
}

struct sockaddr_in *SetDestinationAddr(uint32_t ipAddress)
{
    struct sockaddr_in *destAddr = DestinationAddr();
    if (destAddr != nullptr) {
        destAddr->sin_addr.s_addr = htons(ipAddress);
    }
    return destAddr;
}

DhcpClientContext *InitialDhcpClient(DhcpClientConfig *config)
{
    LOGD("init dhcp client.");
    if (!config) {
        return NULL;
    }
    DhcpClientContext *context = (DhcpClientContext *)calloc(1, sizeof(DhcpClientContext));
    if (context == NULL) {
        LOGE("failed to calloc client context.");
        return NULL;
    }
    if (memset_s(context, sizeof(DhcpClientContext), 0, sizeof(DhcpClientContext)) != EOK) {
        LOGE("failed to reset client context.");
        free(context);
        return NULL;
    }
    if (memset_s(context->config.ifname, IFACE_NAME_SIZE, '\0', IFACE_NAME_SIZE) != EOK) {
        LOGE("failed to reset interface name.");
        free(context);
        return NULL;
    }
    if (strncpy_s(context->config.ifname, IFACE_NAME_SIZE, config->ifname, strlen(config->ifname)) != EOK) {
        LOGE("failed to set interface name.");
        free(context);
        return NULL;
    }
    if (!FillHwAddr(context->config.chaddr, DHCP_HWADDR_LENGTH, config->chaddr, MAC_ADDR_LENGTH)) {
        LOGE("failed to set chaddr.");
        free(context);
        return NULL;
    }
    context->clientFd = 1;
    return context;
}

int ParseDhcpOptions(PDhcpMsgInfo msg)
{
    int ret;
    PDhcpOptionNode pNode = msg->options.first->next;
    DhcpOption endOpt = {END_OPTION, 0, {0}};
    PushBackOption(&msg->options, &endOpt);
    int replyOptsLength = 0;
    uint8_t *current = msg->packet.options, olen = MAGIC_COOKIE_LENGTH;
    uint32_t cookie = htonl(DHCP_MAGIC_COOKIE);
    if (memcpy_s(current, olen, &cookie, olen) != EOK) {
        LOGE("memcpy cookie out of options buffer!");
        return RET_FAILED;
    }
    replyOptsLength += olen;
    current += olen;
    ret = RET_SUCCESS;
    while (pNode && (uint32_t)pNode->option.length < DHCP_OPTION_SIZE) {
        if ((uint32_t)pNode->option.code == END_OPTION) {
            olen = OPT_HEADER_LENGTH + 1;
        } else {
            olen = OPT_HEADER_LENGTH + pNode->option.length;
        }
        if (memcpy_s(current, olen, &pNode->option, olen) != EOK) {
            LOGE("memcpy current option out of options buffer!");
            ret = RET_FAILED;
            break;
        }
        current += olen;
        replyOptsLength += olen;
        if ((uint32_t)pNode->option.code == END_OPTION) {
            break;
        }
        pNode = pNode->next;
        if (replyOptsLength >= DHCP_OPTIONS_SIZE) {
            LOGE("current option out of options buffer!");
            ret = RET_FAILED;
            break;
        }
    }
    msg->length += replyOptsLength;
    return ret;
}

int SendDhcpMessage(DhcpClientContext *ctx, PDhcpMsgInfo msg)
{
    if (!ctx || !msg) {
        LOGE("client context or message pointer is null.");
        return RET_FAILED;
    }
    if (ParseDhcpOptions(msg) != RET_SUCCESS) {
        LOGE("failed to parse dhcp message info.");
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().PushSendMsg(msg->packet);
    return RET_SUCCESS;
}

static uint32_t GetXid(int update)
{
    static uint32_t currXid = Tmspsec();
    if (update) {
        currXid = Tmspsec();
    }
    return currXid;
}

int InitMessage(DhcpClientContext *ctx, PDhcpMsgInfo msg, uint8_t msgType)
{
    LOGD("init dhcp message...");
    if (!ctx) {
        LOGD("client context pointer is null.");
        return DHCP_FALSE;
    }
    if (!msg) {
        LOGD("dhcp message pointer is null.");
        return DHCP_FALSE;
    }
    if (memset_s(msg, sizeof(DhcpMsgInfo), 0, sizeof(DhcpMsgInfo)) != EOK) {
        LOGD("message info pointer is null.");
        return DHCP_FALSE;
    }
    if (InitOptionList(&msg->options) != RET_SUCCESS) {
        LOGD("failed to initialize dhcp client options.");
        return DHCP_FALSE;
    }
    if (!FillHwAddr(msg->packet.chaddr, DHCP_HWADDR_LENGTH, ctx->config.chaddr, MAC_ADDR_LENGTH)) {
        return DHCP_FALSE;
    }
    msg->packet.op = BOOTREQUEST;
    msg->packet.htype = 0x01;
    msg->packet.hlen = MAC_ADDR_LENGTH;
    if (msgType == DHCPDISCOVER) {
        msg->packet.xid = GetXid(DHCP_TRUE);
    } else {
        msg->packet.xid = GetXid(DHCP_FALSE);
    }

    if (DhcpMsgManager::GetInstance().GetClientIp() != 0) {
        DhcpOption optReqIp = {REQUESTED_IP_ADDRESS_OPTION, 0, {0}};
        AppendAddressOption(&optReqIp, DhcpMsgManager::GetInstance().GetClientIp());
        PushFrontOption(&msg->options, &optReqIp);
    }

    DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, OPT_MESSAGE_TYPE_LEGTH, {msgType, 0}};
    PushFrontOption(&msg->options, &optMsgType);

    PDhcpOption pEndOpt = GetOption(&msg->options, END_OPTION);
    if (pEndOpt == NULL) {
        DhcpOption endOpt = {END_OPTION, 0, {0}};
        PushBackOption(&msg->options, &endOpt);
    }
    return DHCP_TRUE;
}

int DhcpDiscover(DhcpClientContext *ctx)
{
    if (!ctx) {
        return RET_FAILED;
    }
    DhcpMsgInfo msgInfo;
    if (!InitMessage(ctx, &msgInfo, DHCPDISCOVER)) {
        LOGD("failed to init dhcp message.");
        return RET_FAILED;
    }
    if (SendDhcpMessage(ctx, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        return RET_FAILED;
    }
    LOGD("send dhcp discover...");
    return RET_SUCCESS;
}

int DhcpRequest(DhcpClientContext *ctx)
{
    if (!ctx) {
        return RET_FAILED;
    }
    DhcpMsgInfo msgInfo;
    if (!InitMessage(ctx, &msgInfo, DHCPREQUEST)) {
        LOGD("failed to init dhcp message.");
        return RET_FAILED;
    }
    if (SendDhcpMessage(ctx, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        return RET_FAILED;
    }
    LOGD("send dhcp request...");
    return RET_SUCCESS;
}

int DhcpInform(DhcpClientContext *ctx)
{
    if (!ctx) {
        return RET_FAILED;
    }
    DhcpMsgInfo msgInfo;
    if (!InitMessage(ctx, &msgInfo, DHCPINFORM)) {
        LOGD("failed to init dhcp message.");
        return RET_FAILED;
    }
    if (SendDhcpMessage(ctx, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        return RET_FAILED;
    }
    LOGD("send dhcp inform...");
    return RET_SUCCESS;
}

int DhcpDecline(DhcpClientContext *ctx)
{
    if (!ctx) {
        return RET_FAILED;
    }
    DhcpMsgInfo msgInfo;
    if (!InitMessage(ctx, &msgInfo, DHCPDECLINE)) {
        LOGD("failed to init dhcp message.");
        return RET_FAILED;
    }
    if (SendDhcpMessage(ctx, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        return RET_FAILED;
    }
    LOGD("send dhcp decline...");
    return RET_SUCCESS;
}

int DhcpRelease(DhcpClientContext *ctx)
{
    if (!ctx) {
        return RET_FAILED;
    }
    DhcpMsgInfo msgInfo;
    if (!InitMessage(ctx, &msgInfo, DHCPRELEASE)) {
        LOGD("failed to init dhcp message.");
        return RET_FAILED;
    }
    if (SendDhcpMessage(ctx, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        return RET_FAILED;
    }
    LOGD("send dhcp release...");
    return RET_SUCCESS;
}