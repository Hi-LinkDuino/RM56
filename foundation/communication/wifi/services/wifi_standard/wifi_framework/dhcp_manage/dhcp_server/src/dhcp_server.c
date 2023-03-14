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

#include "dhcp_server.h"
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include "dhcp_ipv4.h"
#include "dhcp_message.h"
#include "dhcp_option.h"
#include "dhcp_ipv4.h"
#include "securec.h"
#include "dhcp_logger.h"
#include "address_utils.h"
#include "dhcp_address_pool.h"
#include "dhcp_config.h"
#include "common_util.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServer"

#define DHCP_SEL_WAIT_TIMEOUTS 1000
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
#define RECV_BUFFER_SIZE 2048
#define ALLOW_NOBINDING_REQUEST 1
#define REUSE_ADDRESS_ENABLE 1
#define WAIT_STOPED_TIME 5

const uint8_t MAGIC_COOKIE_DATA[MAGIC_COOKIE_LENGTH] = {0x63, 0x82, 0x53, 0x63};  // Vendor Information "Magic Cookie"

enum AssignedNumbers {
    ETHERNET = 1,               // Ethernet (10Mb)
    EXPERIMENTAL_ETHERNET,      // Experimental Ethernet (3Mb)
    AMATEUR_RADIO_AX_25,        // Amateur Radio AX.25
    PROTEON_PRONET_TOKEN_RING,  // Proteon ProNET Token Ring
    CHAOS,
    IEEE802_NETWORKS,
    ARCNET,
    HYPERCHANNEL,
    LANSTAR
};

struct ServerContext {
    int broadCastFlagEnable;
    DhcpAddressPool addressPool;
    DhcpServerCallback callback;
    DhcpConfig config;
    int serverFd;
    int looperState;
    int initialized;
};

enum LooperState {
    LS_IDLE = 0,
    LS_STARING,
    LS_RUNNING,
    LS_RELOADNG,
    LS_STOPING,
    LS_STOPED
};
typedef struct sockaddr_in sockaddr_in;
int FillReply(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
static int OnReceivedDiscover(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
static int OnReceivedRequest(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
static int OnReceivedDecline(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
static int OnReceivedRelease(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
static int OnReceivedInform(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
static int SendDhcpOffer(PDhcpServerContext ctx, PDhcpMsgInfo reply);
static int SendDhcpAck(PDhcpServerContext ctx, PDhcpMsgInfo reply);
static int SendDhcpNak(PDhcpServerContext ctx, PDhcpMsgInfo reply);
static int ParseMessageOptions(PDhcpMsgInfo msg);

static int ParseReplyOptions(PDhcpMsgInfo reply);
struct sockaddr_in *BroadcastAddrIn(void);

static struct ServerContext *GetServerInstance(const DhcpServerContext *ctx)
{
    if (!ctx || !ctx->instance) {
        return NULL;
    }
    return (struct ServerContext *)ctx->instance;
}

int HasFixSocket(int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL)) == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return DHCP_FALSE;
    }
    return DHCP_TRUE;
}

typedef struct ifreq ifreq;
typedef struct sockaddr sockaddr;

int BindNetInterface(int fd, const char *ifname)
{
    if (!fd || !ifname) {
        return RET_FAILED;
    }
    ifreq iface;
    if (memset_s(&iface, sizeof(iface), 0, sizeof(iface)) != EOK) {
        return RET_FAILED;
    }
    if (ifname) {
        ssize_t ifnameSize = strlen(ifname);
        if (strncpy_s(iface.ifr_ifrn.ifrn_name, sizeof(iface.ifr_ifrn.ifrn_name), ifname, ifnameSize) != EOK) {
            return RET_FAILED;
        };
        if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&iface, sizeof(iface)) == -1) {
            LOGE("failed to bind network device interface[%s].", ifname);
            return RET_FAILED;
        }
    } else {
        LOGW("no interface specified.");
    }
    return RET_SUCCESS;
}

int InitServer(const char *ifname)
{
    sockaddr_in srvAddrIn = {0};
    int optval = 1;
    int optrval = 0;
    srvAddrIn.sin_family = AF_INET;
    srvAddrIn.sin_port = htons(DHCP_SERVER_PORT);
    srvAddrIn.sin_addr.s_addr = INADDR_ANY;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1) {
        LOGE("failed to create server socket!");
        return -1;
    }
    if (!HasFixSocket(fd)) {
        LOGD("failed to fcntl O_NONBLOCK flag!");
    }
    if (BindNetInterface(fd, ifname) != RET_SUCCESS) {
        close(fd);
        return -1;
    }
    socklen_t optlen = sizeof(optrval);
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&optrval, &optlen) == -1) {
        LOGD("failed to receive buffer size.");
    } else {
        LOGD("receive buffer size is %d", optrval);
    }
    if (REUSE_ADDRESS_ENABLE && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) == -1) {
        LOGW("failed to setsockopt 'SO_REUSEADDR' for server socket!");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
        LOGE("failed to setsockopt 'SO_BROADCAST' for server socket!");
        close(fd);
        return -1;
    }
    if (bind(fd, (sockaddr *)&srvAddrIn, sizeof(sockaddr)) == -1) {
        LOGE("failed to bind server!");
        close(fd);
        return -1;
    }
    return fd;
}

struct sockaddr_in *BroadcastAddrIn(void)
{
    static struct sockaddr_in broadcastAddrIn = {0};
    if (broadcastAddrIn.sin_port == 0) {
        broadcastAddrIn.sin_port = htons(DHCP_CLIENT_PORT);
        broadcastAddrIn.sin_family = AF_INET;
        broadcastAddrIn.sin_addr.s_addr = INADDR_BROADCAST;
    }
    return &broadcastAddrIn;
}

struct sockaddr_in *SourceAddrIn(void)
{
    static struct sockaddr_in sourceAddrIn = {0};
    sourceAddrIn.sin_port = htons(DHCP_CLIENT_PORT);
    sourceAddrIn.sin_family = AF_INET;
    sourceAddrIn.sin_addr.s_addr = INADDR_ANY;
    return &sourceAddrIn;
}

struct sockaddr_in *ResetSourceAddr(void)
{
    struct sockaddr_in *srcAddr = SourceAddrIn();
    srcAddr->sin_port = htons(DHCP_CLIENT_PORT);
    srcAddr->sin_family = AF_INET;
    srcAddr->sin_addr.s_addr = INADDR_ANY;
    return srcAddr;
}

uint32_t SourceIpAddress(void)
{
    uint32_t srcIp = SourceAddrIn()->sin_addr.s_addr;
    return srcIp;
}
struct sockaddr_in *DestinationAddrIn(void)
{
    static struct sockaddr_in destAddrIn = {0};
    if (destAddrIn.sin_port == 0) {
        destAddrIn.sin_port = htons(DHCP_CLIENT_PORT);
        destAddrIn.sin_family = AF_INET;
    }
    return &destAddrIn;
}

struct sockaddr_in *DestinationAddr(uint32_t ipAddress)
{
    struct sockaddr_in *destAddr = DestinationAddrIn();
    destAddr->sin_addr.s_addr = htonl(ipAddress);
    return destAddr;
}

int ReceiveDhcpMessage(int sock, PDhcpMsgInfo msgInfo)
{
    static uint8_t recvBuffer[RECV_BUFFER_SIZE] = {0};
    struct timeval tmt;
    fd_set recvFd;
    FD_ZERO(&recvFd);
    FD_SET(sock, &recvFd);
    time_t tms = DHCP_SEL_WAIT_TIMEOUTS;
    tmt.tv_sec = tms;
    tmt.tv_usec = 0;
    if (select(sock + 1, &recvFd, NULL, NULL, &tmt) < 0) {
        LOGE("select error, %d", errno);
        return ERR_SELECT;
    }
    if (!FD_ISSET(sock, &recvFd)) {
        LOGE("failed to select isset.");
        return RET_ERROR;
    }
    socklen_t ssize = sizeof(sockaddr_in);
    struct sockaddr_in *srcAddrIn = ResetSourceAddr();
    srcAddrIn->sin_addr.s_addr = INADDR_ANY;
    int rsize = recvfrom(sock, recvBuffer, RECV_BUFFER_SIZE, 0, (struct sockaddr *)srcAddrIn, (socklen_t *)&ssize);
    if (!rsize) {
        LOGE("receive error, %d",  errno);
        return RET_FAILED;
    }
    if (rsize > (int)sizeof(DhcpMsgInfo) || rsize < DHCP_MSG_HEADER_SIZE) {
        LOGW("message length error, received %d bytes.", rsize);
        return RET_FAILED;
    }
    msgInfo->length = rsize;
    if (memcpy_s(&msgInfo->packet, sizeof(DhcpMessage), recvBuffer, rsize) != EOK) {
        return RET_FAILED;
    };
    if (msgInfo->packet.op != BOOTREQUEST) {
        LOGW("dhcp message type error!");
        return RET_FAILED;
    }
    if (msgInfo->packet.hlen > DHCP_HWADDR_LENGTH) {
        LOGW("hlen error!");
        return RET_FAILED;
    }
    if (IsEmptyHWAddr(msgInfo->packet.chaddr)) {
        LOGW("client hardware address error!");
        return RET_FAILED;
    }
    if (IsReserved(msgInfo->packet.chaddr)) {
        LOGD("ignore client, %s", ParseLogMac(msgInfo->packet.chaddr));
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

void InitReply(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!reply) {
        LOGE("reply messsage pointer is null!");
        return;
    }
    reply->packet.op = BOOTREPLY;
    reply->packet.htype = ETHERNET;
    reply->packet.hlen = OPT_MAC_ADDR_LENGTH;
    reply->packet.secs = 0;
    reply->packet.ciaddr = 0;
    if (memset_s(reply->packet.sname, sizeof(reply->packet.sname), '\0', sizeof(reply->packet.sname)) != EOK) {
        LOGE("failed to reset message packet[sname]!");
        return;
    };
    if (memset_s(reply->packet.file, sizeof(reply->packet.file), '\0', sizeof(reply->packet.file)) != EOK) {
        LOGE("failed to reset message packet[file]!");
        return;
    }

    if (FillReply(ctx, received, reply) != RET_SUCCESS) {
        LOGW("failed to fill reply message.");
    }
}

void OnUpdateServerConfig(PDhcpServerContext ctx)
{
    LOGD("OnUpdateServerConfig ...");
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return;
    }
    if (srvIns->callback) {
        srvIns->callback(ST_RELOADNG, 0, ctx->ifname);
    }
}

static void OnServerStoping(PDhcpServerContext ctx)
{
    LOGD("server stoping ...");
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return;
    }
    if (srvIns->callback) {
        srvIns->callback(ST_STOPING, 0, ctx->ifname);
    }
}

void OnServerStoped(PDhcpServerContext ctx, int code)
{
    LOGD("OnServerStoped.");
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return;
    }
    if (srvIns->callback) {
        srvIns->callback(ST_STOPED, code, ctx->ifname);
    }
}

int SendDhcpReply(PDhcpServerContext ctx, int replyType, PDhcpMsgInfo reply)
{
    if (!reply) {
        LOGE("reply message pointer is null.");
        return RET_FAILED;
    }
    int sendRet = -1;
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    switch (replyType) {
        case REPLY_OFFER:
            LOGD("<== send reply dhcp offer.");
            sendRet = SendDhcpOffer(ctx, reply);
            break;
        case REPLY_ACK:
            LOGD("<== send reply dhcp ack.");
            sendRet = SendDhcpAck(ctx, reply);
            break;
        case REPLY_NAK:
            LOGD("<== send reply dhcp nak.");
            sendRet = SendDhcpNak(ctx, reply);
            break;
        default:
            break;
    }
    if (replyType && sendRet != RET_SUCCESS) {
        return RET_FAILED;
    }
    return  RET_SUCCESS;
}

static int MessageProcess(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    int replyType = REPLY_NONE;
    if (!received) {
        return replyType;
    }
    PDhcpOption opt = GetOption(&received->options, DHCP_MESSAGE_TYPE_OPTION);
    if (!opt) {
        LOGE("error dhcp message, missing required message type option.");
        return replyType;
    }
    uint8_t messageType = opt->data[0];
    switch (messageType) {
        case DHCPDISCOVER: {
            LOGD("==> Received DHCPDISCOVER message.");
            replyType = OnReceivedDiscover(ctx, received, reply);
            break;
        }
        case DHCPREQUEST: {
            LOGD("==> Received DHCPREQUEST message.");
            replyType = OnReceivedRequest(ctx, received, reply);
            break;
        }
        case DHCPDECLINE: {
            LOGD("==> Received DHCPDECLINE message.");
            replyType = OnReceivedDecline(ctx, received, reply);
            break;
        }
        case DHCPRELEASE: {
            LOGD("==> Received DHCPRELEASE message.");
            replyType = OnReceivedRelease(ctx, received, reply);
            break;
        }
        case DHCPINFORM: {
            LOGD("==> Received DHCPINFORM message.");
            replyType = OnReceivedInform(ctx, received, reply);
            break;
        }
        default:
            break;
    }
    return replyType;
}

int SaveLease(PDhcpServerContext ctx)
{
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    int saveRet = SaveBindingRecoders(&srvIns->addressPool, 1);
    if (saveRet == RET_FAILED) {
        LOGD("failed to save lease recoders. total: %zu", srvIns->addressPool.leaseTable.size);
    } else if (saveRet == RET_SUCCESS) {
        LOGD("lease recoders saved.");
    }
    return saveRet;
}

static int OnLooperStateChanged(PDhcpServerContext ctx)
{
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }

    if (srvIns->looperState == LS_RELOADNG) {
        OnUpdateServerConfig(ctx);
        srvIns->looperState = LS_RUNNING;
    } else if (srvIns->looperState == LS_STOPING) {
        OnServerStoping(ctx);
        return RET_BREAK;
    }
    return RET_SUCCESS;
}

static int ContinueReceive(PDhcpMsgInfo from, int recvRet)
{
    if (!from) {
        return DHCP_TRUE;
    }
    if (recvRet != RET_SUCCESS) {
        return DHCP_TRUE;
    }
    size_t recLength = from->length;
    LOGD("received, length:%zu", recLength);
    if (ParseMessageOptions(from) != 0) {
        LOGE("invalid dhcp message.");
        return DHCP_TRUE;
    }
    if (!GetOption(&from->options, DHCP_MESSAGE_TYPE_OPTION)) {
        LOGW("can't found 'message type' option.");
        return DHCP_TRUE;
    }
    return DHCP_FALSE;
}

static int BeginLooper(PDhcpServerContext ctx)
{
    DhcpMsgInfo from;
    DhcpMsgInfo reply;
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    ctx->instance->serverFd = InitServer(ctx->ifname);
    if (ctx->instance->serverFd < 0) {
        LOGE("failed to initialize server socket.");
        return RET_FAILED;
    }
    InitOptionList(&from.options);
    InitOptionList(&reply.options);
    srvIns->looperState = LS_RUNNING;
    while (srvIns->looperState) {
        if (OnLooperStateChanged(ctx) != RET_SUCCESS) {
            break;
        }
        ClearOptions(&from.options);
        ClearOptions(&reply.options);
        int recvRet = ReceiveDhcpMessage(ctx->instance->serverFd, &from);
        if (recvRet == RET_ERROR || recvRet == ERR_SELECT) {
            break;
        }
        if (ContinueReceive(&from, recvRet)) {
            continue;
        }
        InitReply(ctx, &from, &reply);
        int replyType = MessageProcess(ctx, &from, &reply);
        if (replyType && SendDhcpReply(ctx, replyType, &reply) != RET_SUCCESS) {
            LOGE("failed to send reply message.");
        }
        if (replyType == REPLY_ACK || replyType == REPLY_OFFER) {
            int saveRet = SaveBindingRecoders(&srvIns->addressPool, 0);
            if (saveRet != RET_SUCCESS && saveRet != RET_WAIT_SAVE) {
                LOGW("failed to save lease recoders.");
            };
        }
    }
    FreeOptionList(&from.options);
    FreeOptionList(&reply.options);
    LOGD("dhcp server message looper stoped.");
    close(ctx->instance->serverFd);
    ctx->instance->serverFd = -1;
    srvIns->looperState = LS_STOPED;
    return 0;
}

static int CheckAddressRange(DhcpAddressPool *pool)
{
    uint32_t serverNetwork = NetworkAddress(pool->serverId, pool->netmask);
    uint32_t firstNetwork = NetworkAddress(pool->addressRange.beginAddress, pool->netmask);
    uint32_t secondNetwork = NetworkAddress(pool->addressRange.endAddress, pool->netmask);
    if (!serverNetwork || !firstNetwork || !secondNetwork) {
        LOGE("network config error.");
        return DHCP_FALSE;
    }
    if (serverNetwork != firstNetwork || serverNetwork != secondNetwork) {
        LOGE("server network and address pool network belong to different networks.");
        return DHCP_FALSE;
    }
    return DHCP_TRUE;
}

void InitBindingRecoders(DhcpAddressPool *pool)
{
    if (!pool) {
        LOGE("address pool pointer is null.");
        return;
    }
    HashTable *table = &pool->leaseTable;
    if (!Initialized(table)) {
        LOGE("pool does not init.");
        return;
    }
    uint32_t realLeaseTotal = 0;
    for (size_t current = 0; current < table->capacity; ++current) {
        HashNode *node = table->nodes[current];
        int invalidBindig;
        while (node) {
            HashNode *next = node->next;
            AddressBinding *binding = (AddressBinding*)node->value;
            if (binding && !IsEmptyHWAddr(binding->chaddr) && binding->ipAddress) {
                AddBinding(binding);
                realLeaseTotal++;
                invalidBindig = 0;
            } else {
                LOGE("bad binding recoder.");
                invalidBindig = 1;
            }
            if (!invalidBindig && binding && pool->distribution < binding->ipAddress) {
                pool->distribution = binding->ipAddress;
            }
            node = next;
        }
    }
    LOGD("lease recoder total: %u", realLeaseTotal);
}

void InitLeaseFile(DhcpAddressPool *pool)
{
    const char *leasePath  = GetFilePath(DHCPD_LEASE_FILE);
    if (!leasePath || strlen(leasePath) == 0) {
        LOGE("failed to get lease file path.");
        return;
    }
    if (access(leasePath, 0) != 0) {
        LOGD("lease file path does not exist.");
        if (!CreatePath(leasePath)) {
            LOGE("failed to create lease file directory.");
            return;
        } else {
            LOGD("lease file directory created.");
        }
    }
    if (LoadBindingRecoders(pool) != RET_SUCCESS) {
        LOGW("failed to load lease recoders.");
    }
    InitBindingRecoders(pool);
}

static void ExitProcess(void)
{
    LOGD("dhcp server stoped.");
    sleep(1);
}

int StartDhcpServer(PDhcpServerContext ctx)
{
    LOGD("dhcp server starting ...");
    if (!ctx) {
        LOGE("server context pointer is null.");
        return RET_FAILED;
    }
    if (strlen(ctx->ifname) == 0) {
        LOGE("context interface is null or empty.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context instance pointer is null.");
        return RET_FAILED;
    }
    if (atexit(ExitProcess) != 0) {
        LOGW("failed to regiester exit process function.");
    }
    if (!srvIns->initialized) {
        LOGE("dhcp server no initialized.");
        return RET_FAILED;
    }
    LOGD("bind interface: %s", ctx->ifname);
    LOGI("begin dhcp message looper");
    if (srvIns->callback) {
        srvIns->callback(ST_STARTING, 1, ctx->ifname);
    }
    int ret = BeginLooper(ctx);
    if (ret != RET_SUCCESS) {
        LOGD("faild to start dhcp server.");
        OnServerStoped(ctx, ret);
        return RET_FAILED;
    }
    OnServerStoped(ctx, ret);
    return RET_SUCCESS;
}

int StopDhcpServer(PDhcpServerContext ctx)
{
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        return RET_FAILED;
    }
    srvIns->looperState = LS_STOPING;
    return RET_SUCCESS;
}

int GetServerStatus(PDhcpServerContext ctx)
{
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return -1;
    }
    return srvIns->looperState;
}

int FillReply(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!received || !reply) {
        return RET_ERROR;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    if (received->packet.ciaddr && received->packet.ciaddr != INADDR_BROADCAST) {
        reply->packet.ciaddr = received->packet.ciaddr;
    }
    if (received->packet.flags) {
        reply->packet.flags = received->packet.flags;
    }
    if (received->packet.xid) {
        reply->packet.xid = received->packet.xid;
    }
    if (received->packet.siaddr && received->packet.siaddr != INADDR_BROADCAST) {
        reply->packet.siaddr = received->packet.siaddr;
    } else {
        reply->packet.siaddr = srvIns->addressPool.serverId;
    }
    if (received->packet.giaddr && received->packet.giaddr != INADDR_BROADCAST) {
        reply->packet.giaddr = received->packet.giaddr;
    } else {
        if (srvIns->addressPool.gateway) {
            reply->packet.giaddr = srvIns->addressPool.gateway;
        }
    }
    if (received->packet.hlen) {
        reply->packet.hlen = received->packet.hlen;
        LOGD("fill reply - chaddr:%s", ParseLogMac(received->packet.chaddr));
        if (memset_s(reply->packet.chaddr, sizeof(reply->packet.chaddr), 0, sizeof(reply->packet.chaddr)) != EOK) {
            LOGE("failed to reset message packet[chaddr]!");
            return RET_ERROR;
        }
        if (memcpy_s(reply->packet.chaddr, sizeof(reply->packet.chaddr),
            received->packet.chaddr, sizeof(received->packet.chaddr)) != EOK) {
            LOGE("failed to copy message packet[chaddr]!");
            return RET_ERROR;
        }
    }
    if (received->packet.giaddr) {
        reply->packet.giaddr = received->packet.giaddr;
    }
    return 0;
}

int AppendReplyTimeOptions(PDhcpServerContext ctx, PDhcpOptionList options)
{
    if (!ctx || !options) {
        LOGE("server context or options poninter is null.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    uint32_t leaseTime = HostToNetwork(DHCP_LEASE_TIME);
    if (srvIns->addressPool.leaseTime) {
        leaseTime = HostToNetwork(srvIns->addressPool.leaseTime);
    }
    DhcpOption optLeaseTime = {IP_ADDRESS_LEASE_TIME_OPTION, OPT_TIME_LENGTH, {0}};
    FillU32Option(&optLeaseTime, leaseTime);
    PushBackOption(options, &optLeaseTime);

    uint32_t t1Time = HostToNetwork(DHCP_RENEWAL_TIME);
    if (srvIns->addressPool.renewalTime) {
        t1Time = HostToNetwork(srvIns->addressPool.renewalTime);
    }
    DhcpOption optRenewTime = {RENEWAL_TIME_VALUE_OPTION, OPT_TIME_LENGTH, {0}};
    FillU32Option(&optLeaseTime, t1Time);
    PushBackOption(options, &optRenewTime);

    uint32_t t2Time = HostToNetwork(DHCP_REBINDING_TIME);
    if (srvIns->addressPool.rebindingTime) {
        t2Time = HostToNetwork(srvIns->addressPool.rebindingTime);
    }
    DhcpOption optRebindTime = {REBINDING_TIME_VALUE_OPTION, OPT_TIME_LENGTH, {0}};
    FillU32Option(&optRebindTime, t2Time);
    PushBackOption(options, &optRebindTime);

    return RET_SUCCESS;
}

static int Repending(DhcpAddressPool *pool, AddressBinding *binding)
{
    if (!pool) {
        return REPLY_NONE;
    }
    uint32_t bindingIp = binding->ipAddress;
    LOGD(" binding found, bindIp:%s", ParseStrIp(bindingIp));
    binding->pendingInterval = NextPendingInterval(binding->pendingInterval);
    uint64_t tms = Tmspsec() - binding->pendingTime;
    if (tms < binding->pendingInterval) {
        binding->pendingTime = Tmspsec();
        LOGW("message interval is too short, ignore the message.");
        return REPLY_NONE;
    }
    binding->pendingTime = Tmspsec();
    binding->pendingInterval = 0;
    binding->bindingStatus = BIND_PENDING;
    uint32_t srcIp = SourceIpAddress();
    if (srcIp && srcIp != INADDR_BROADCAST && bindingIp != INADDR_BROADCAST && srcIp != bindingIp) {
        LOGW("source ip address and bound ip address inconsistency.");
        return REPLY_NAK;
    }
    if (srcIp && srcIp == bindingIp) {
        if (!ContainsKey(&pool->leaseTable, (uintptr_t)&srcIp)) {
            LOGD("can't find lease information.");
            if (Insert(&pool->leaseTable, (uintptr_t)&srcIp, (uintptr_t)binding) != HASH_INSERTED) {
                LOGE("failed to insert lease information.");
            }
        } else {
            if (Insert(&pool->leaseTable, (uintptr_t)&srcIp, (uintptr_t)binding) != HASH_UPDATED) {
                LOGE("failed to update lease information.");
            }
        }
    }
    return REPLY_OFFER;
}

static int Rebinding(DhcpAddressPool *pool, AddressBinding *binding)
{
    uint64_t pendingTime = binding->pendingTime;
    int replyType = Repending(pool, binding);
    binding->bindingStatus = BIND_ASSOCIATED;
    if (!binding->leaseTime) {
        binding->leaseTime = pool->leaseTime;
    }
    binding->bindingTime = Tmspsec();
    binding->expireIn = binding->bindingTime + binding->leaseTime;
    binding->pendingTime = pendingTime;
    if (replyType == REPLY_OFFER) {
        replyType = REPLY_ACK;
    }
    return replyType;
}

static void AddAddressOption(PDhcpMsgInfo reply, uint8_t code, int32_t address)
{
    if (!reply) {
        return;
    }
    DhcpOption optAddress = {0, 0, {0}};
    optAddress.code = code;
    if (AppendAddressOption(&optAddress, address) != RET_SUCCESS) {
        LOGE("failed to append address option.");
        return;
    };
    PushBackOption(&reply->options, &optAddress);
}

int AddReplyServerIdOption(PDhcpOptionList options, uint32_t serverId)
{
    if (!options) {
        LOGE("option list pointer is null.");
        return RET_FAILED;
    }
    if (!serverId || serverId==INADDR_BROADCAST) {
        LOGE("servier id error.");
        return RET_FAILED;
    }
    DhcpOption optSrvId = {SERVER_IDENTIFIER_OPTION, 0, {0}};
    if (AppendAddressOption(&optSrvId, serverId) != RET_SUCCESS) {
        LOGE("failed to append server id option.");
        return RET_FAILED;
    }
    if (GetOption(options, SERVER_IDENTIFIER_OPTION)) {
        LOGD("server identifier option exists.");
        return RET_SUCCESS;
    }
    PushBackOption(options, &optSrvId);
    return RET_SUCCESS;
}

static void AddReplyMessageTypeOption(PDhcpMsgInfo reply, uint8_t replyMessageType)
{
    if (!reply) {
        return;
    }
    DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, OPT_MESSAGE_TYPE_LEGTH, {replyMessageType, 0}};
    PushBackOption(&reply->options, &optMsgType);
}


AddressBinding *GetBinding(DhcpAddressPool *pool, PDhcpMsgInfo received)
{
    if (!pool) {
        return NULL;
    }
    if (!received) {
        return NULL;
    }
    AddressBinding *binding = pool->binding(received->packet.chaddr, &received->options);
    if (!binding) {
        binding = pool->newBinding(received->packet.chaddr, &received->options);
        if (binding == NULL) {
            LOGE("new binding is null");
            return NULL;
        }
        if (pool->leaseTime) {
            binding->leaseTime = pool->leaseTime;
        }
        binding->ipAddress = pool->distribue(pool, received->packet.chaddr);
        LOGD("new binding ip");
    } else {
        LOGD("rebinding ip");
    }
    return binding;
}

static int OnReceivedDiscover(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!received || !reply) {
        return REPLY_NONE;
    }
    LOGI("received 'Discover' message from: %s", ParseLogMac(received->packet.chaddr));
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        return REPLY_NONE;
    }
    uint32_t reqIp = 0;
    PDhcpOption optReqIp = GetOption(&received->options, REQUESTED_IP_ADDRESS_OPTION);
    if (optReqIp) {
        reqIp = ParseIp(optReqIp->data);
        if (reqIp) {
            LOGD(" request ip: %s", ParseStrIp(reqIp));
        }
    }
    uint32_t srcIp = SourceIpAddress();
    if (!srvIns->broadCastFlagEnable) {
        if (srcIp) {
            LOGD(" client repending:%s", ParseStrIp(srcIp));
        } else {
            srcIp = INADDR_BROADCAST;
        }
        DestinationAddr(srcIp);
    }
    AddressBinding *binding = GetBinding(&srvIns->addressPool, received);
    if (!binding) {
        return REPLY_NONE;
    }
    if (!binding->ipAddress) {
        LOGE("no ip address available.");
        return REPLY_NONE;
    }
    if (reqIp != 0 && reqIp != binding->ipAddress) {
        LOGE("error request message.");
        return REPLY_NONE;
    }
    AddressBinding *lease = GetLease(&srvIns->addressPool, binding->ipAddress);
    if (!lease) {
        LOGD("add lease recoder.");
        AddLease(&srvIns->addressPool, binding);
        lease = GetLease(&srvIns->addressPool, binding->ipAddress);
    }
    if (!lease) {
        return REPLY_NONE;
    }
    AddReplyMessageTypeOption(reply, DHCPOFFER);
    reply->packet.yiaddr = lease->ipAddress;
    return REPLY_OFFER;
}


static uint32_t GetRequestIpAddress(PDhcpMsgInfo received)
{
    uint32_t reqIp = 0;
    if (!received) {
        return reqIp;
    }
    PDhcpOption optReqIp = GetOption(&received->options, REQUESTED_IP_ADDRESS_OPTION);
    if (optReqIp) {
        reqIp = ParseIp(optReqIp->data);
    }
    return reqIp;
}

static int GetYourIpAddress(PDhcpMsgInfo received, uint32_t *yourIpAddr)
{
    uint32_t cliIp = received->packet.ciaddr;
    uint32_t srcIp = SourceIpAddress();
    uint32_t reqIp = GetRequestIpAddress(received);
    if (cliIp && srcIp && cliIp != srcIp) {
        LOGE("error dhcp request message, missing required request option.");
        return RET_FAILED;
    }
    if (reqIp && srcIp && reqIp != srcIp) {
        LOGE("error dhcp request message, request ip error.");
        return RET_FAILED;
    }
    if (cliIp && reqIp && cliIp != reqIp) {
        LOGE("error dhcp request message, client ip error.");
        return RET_FAILED;
    }

    if (srcIp && srcIp != INADDR_BROADCAST) {
        *yourIpAddr = srcIp;
    } else if (cliIp && cliIp != INADDR_BROADCAST) {
        *yourIpAddr = cliIp;
    } else if (reqIp && reqIp != INADDR_BROADCAST) {
        *yourIpAddr = reqIp;
    }

    if (srcIp && srcIp != INADDR_BROADCAST) {
        DestinationAddr(srcIp);
    } else if (srcIp == INADDR_ANY) {
        DestinationAddr(INADDR_BROADCAST);
    }
    return RET_SUCCESS;
}

static int NotBindingRequest(DhcpAddressPool *pool, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    uint32_t yourIpAddr = 0;
    if (GetYourIpAddress(received, &yourIpAddr) != RET_SUCCESS) {
        return REPLY_NONE;
    }
    AddressBinding *lease = GetLease(pool, yourIpAddr);
    if (!lease) {
        if (SourceIpAddress()) {
            return REPLY_ACK;
        }
        return REPLY_NONE;
    }
    int sameAddr = AddrEquels(lease->chaddr, received->packet.chaddr, MAC_ADDR_LENGTH);
    if (lease->bindingStatus == BIND_ASSOCIATED && !sameAddr) {
        if (!IsExpire(lease)) {
            return REPLY_NAK;
        }
        RemoveLease(pool, lease);
    }
    AddressBinding *binding = pool->newBinding(received->packet.chaddr, &received->options);
    if (binding == NULL) {
        LOGE("Not binding request binding is null.");
        return REPLY_NONE;
    }
    binding->ipAddress = yourIpAddr;
    if (pool->leaseTime) {
        binding->leaseTime = pool->leaseTime;
    }
    int replyType = Repending(pool, binding);
    if (replyType != REPLY_OFFER) {
        return replyType;
    }
    lease = GetLease(pool, yourIpAddr);
    if (!lease) {
        LOGD("add new lease recoder.");
        AddLease(pool, binding);
        lease = GetLease(pool, binding->ipAddress);
    }
    if (!lease) {
        LOGD("failed to get lease.");
        return REPLY_NONE;
    }
    lease->bindingStatus = BIND_ASSOCIATED;
    lease->bindingTime = Tmspsec();
    lease->expireIn = lease->bindingTime + binding->leaseTime;
    reply->packet.yiaddr = lease->ipAddress;
    return REPLY_ACK;
}

static int ValidateRequestMessage(const PDhcpServerContext ctx, const PDhcpMsgInfo received,
    PDhcpMsgInfo reply, uint32_t *yourIp)
{
    if (!received || !reply) {
        return REPLY_NONE;
    }
    LOGI("received 'Request' message from: %s.", ParseLogMac(received->packet.chaddr));
    uint32_t yourIpAddr = INADDR_BROADCAST;
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        return RET_FAILED;
    }
    if (GetYourIpAddress(received, &yourIpAddr) != RET_SUCCESS) {
        if (yourIpAddr && yourIpAddr != INADDR_BROADCAST) {
            return REPLY_NAK;
        }
        return REPLY_NONE;
    }
    PDhcpOption optReqSrvId = GetOption(&received->options, SERVER_IDENTIFIER_OPTION);
    if (optReqSrvId) {
        uint32_t reqSrvId = ParseIp(optReqSrvId->data);
        LOGD(" reuquest server id is:%s", ParseStrIp(reqSrvId));
        if (reqSrvId != srvIns->addressPool.serverId) {
            LOGW("other dhcp server process.");
            return REPLY_NONE;
        }
    } else {
        LOGW("request message not specified server identifier option.");
    }
    *yourIp = yourIpAddr;
    return REPLY_ACK;
}

static int HasNobindgRequest(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!received || !reply) {
        LOGE("receive or reply message pointer is null.");
        return REPLY_NONE;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return REPLY_NONE;
    }
    AddressBinding *binding = srvIns->addressPool.binding(received->packet.chaddr, &received->options);
    if (!binding && ALLOW_NOBINDING_REQUEST) {
        LOGE("client not binding.");
        uint32_t srcIp = SourceIpAddress();
        uint32_t reqIp = GetRequestIpAddress(received);
        LOGD("allow no binding request mode.");
        if (reqIp == 0 && srcIp == 0) {
            LOGE("error dhcp message.");
            return REPLY_NONE;
        }
        if (!IpInNetwork(reqIp, srvIns->addressPool.serverId, srvIns->addressPool.netmask)) {
            LOGE("error request ip.");
            return REPLY_NAK;
        }
        return NotBindingRequest(&srvIns->addressPool, received, reply);
    }
    return REPLY_ACK;
}

static int OnReceivedRequest(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    int ret;
    uint32_t yourIpAddr;
    if ((ret = ValidateRequestMessage(ctx, received, reply, &yourIpAddr)) != REPLY_ACK) {
        return ret;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    AddressBinding *binding = srvIns->addressPool.binding(received->packet.chaddr, &received->options);
    if (binding == NULL) {
        LOGE("OnReceivedRequest, binding is null");
        return REPLY_NONE;
    }
    if ((ret = HasNobindgRequest(ctx, received, reply)) != REPLY_ACK) {
        return ret;
    }
    Rebinding(&srvIns->addressPool, binding);
    AddressBinding *lease = GetLease(&srvIns->addressPool, yourIpAddr);
    if (lease) {
        int sameAddr = AddrEquels(lease->chaddr, received->packet.chaddr, MAC_ADDR_LENGTH);
        if (!sameAddr && !IsExpire(lease)) {
            LOGW("invalid request ip address.");
            return REPLY_NAK;
        }
        if (!sameAddr && IsExpire(lease)) {
            if (memcpy_s(lease->chaddr, DHCP_HWADDR_LENGTH, binding->chaddr, MAC_ADDR_LENGTH) != EOK) {
                LOGW("failed to update lease client address.");
            }
        }
        lease->bindingStatus = BIND_ASSOCIATED;
        lease->bindingTime = binding->bindingTime;
        lease->expireIn = binding->expireIn;
    } else {
        LOGW("can not found lease recoder.");
    }
    uint32_t bindingIp = binding->ipAddress;
    if (bindingIp && yourIpAddr != INADDR_BROADCAST && yourIpAddr != bindingIp) {
        LOGE("error request ip binding.");
        return REPLY_NAK;
    }
    AddAddressOption(reply, SUBNET_MASK_OPTION, srvIns->addressPool.netmask);
    if (srvIns->addressPool.gateway) {
        AddAddressOption(reply, ROUTER_OPTION, srvIns->addressPool.gateway);
    }
    reply->packet.yiaddr = bindingIp;
    return REPLY_ACK;
}

static int OnReceivedDecline(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!received || !reply) {
        return REPLY_NONE;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        return REPLY_NONE;
    }
    LOGI("received 'Decline' message from: %s.", ParseLogMac(received->packet.chaddr));
    uint32_t reqIp = 0;
    PDhcpOption optReqIp = GetOption(&received->options, REQUESTED_IP_ADDRESS_OPTION);
    if (optReqIp) {
        reqIp = ParseIp(optReqIp->data);
    }
    if (!reqIp) {
        LOGD("invalid request ip address.");
        return REPLY_NONE;
    }
    AddressBinding* binding = srvIns->addressPool.binding(received->packet.chaddr, &received->options);
    if (!binding) {
        LOGD("client not binding.");
        return REPLY_NONE;
    }
    if (binding->ipAddress != reqIp) {
        LOGD("invalid request ip address.");
        return REPLY_NONE;
    }
    if (ContainsKey(&srvIns->addressPool.leaseTable, (uintptr_t)&binding->ipAddress)) {
        AddressBinding *lease = GetBindingByIp(&srvIns->addressPool.leaseTable, binding->ipAddress);
        if (lease) {
            lease->bindingStatus = BIND_MODE_RESERVED;
            lease->expireIn = Tmspsec() + lease->leaseTime;
        } else {
            LOGE("failed to get lease info.");
        }
    }
    RemoveBinding(received->packet.chaddr);
    return REPLY_NONE;
}

static int OnReceivedRelease(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!received || !reply) {
        return REPLY_NONE;
    }
    LOGI("received 'Release' message from: %s", ParseLogMac(received->packet.chaddr));
    if (!ctx || !ctx->instance) {
        return RET_FAILED;
    }
    PDhcpOption optReqIp = GetOption(&received->options, REQUESTED_IP_ADDRESS_OPTION);
    if (!optReqIp) {
        LOGW("missing required request option.");
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    AddressBinding *binding = srvIns->addressPool.binding(received->packet.chaddr, &received->options);
    if (!binding) {
        LOGD("client not binding.");
        return REPLY_NONE;
    }
    uint32_t bindIp = binding->ipAddress;
    uint32_t reqIp = 0;
    if (optReqIp) {
        reqIp = ParseIp(optReqIp->data);
    }
    uint32_t srcIp = SourceIpAddress();
    if (srcIp != 0 && reqIp != 0 && reqIp != srcIp) {
        LOGE("error release message, invalid request ip address.");
        return REPLY_NONE;
    }
    if (bindIp != 0 && reqIp != 0 && reqIp != bindIp) {
        LOGE("error release message, invalid request ip address.");
        return REPLY_NONE;
    }
    AddressBinding *lease = GetLease(&srvIns->addressPool, bindIp);
    if (lease) {
        RemoveLease(&srvIns->addressPool, lease);
        LOGD("lease recoder has been removed.");
    } else {
        LOGW("can't found lease recoder.");
    }

    if (ReleaseBinding(received->packet.chaddr) != RET_SUCCESS) {
        LOGW("failed to release client[%s] bind.", ParseLogMac(received->packet.chaddr));
    }
    LOGD("client released.");
    return REPLY_NONE;
}

static int OnReceivedInform(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply)
{
    if (!received || !reply) {
        return REPLY_NONE;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    LOGI("received 'Inform' message from: %s", ParseLogMac(received->packet.chaddr));
    if (IsEmptyHWAddr(received->packet.chaddr)) {
        LOGD("error dhcp 'Inform' message.");
    }
    return REPLY_ACK;
}

static int AppendFixedOptions(PDhcpServerContext ctx, PDhcpMsgInfo reply)
{
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        return RET_FAILED;
    }
    if (!reply) {
        return RET_FAILED;
    }
    if (srvIns->addressPool.fixedOptions.size > 0) {
        DhcpOptionNode *pNode = srvIns->addressPool.fixedOptions.first->next;
        for (size_t i = 0; pNode != NULL && i < srvIns->addressPool.fixedOptions.size; i++) {
            PDhcpOption opt = NULL;
            if (pNode->option.code) {
                opt = GetOption(&reply->options, pNode->option.code);
            }
            if (opt == NULL) {
                PushBackOption(&reply->options, &pNode->option);
            }
            pNode = pNode->next;
        }
    }
    return RET_SUCCESS;
}
int AppendReplyTypeOption(PDhcpMsgInfo reply, int replyType)
{
    if (!reply) {
        return RET_FAILED;
    }
    if (!replyType) {
        return RET_FAILED;
    }
    uint8_t msgType = 0;
    switch (replyType) {
        case REPLY_OFFER:
            msgType = DHCPOFFER;
            break;
        case REPLY_ACK:
            msgType = DHCPACK;
            break;
        case REPLY_NAK:
            msgType = DHCPNAK;
            break;
        default:
            break;
    }
    PDhcpOption pOptMsgType = GetOption(&reply->options, DHCP_MESSAGE_TYPE_OPTION);
    if (!pOptMsgType) {
        LOGD("append message type option for reply message, type:%hhu", msgType);
        DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, OPT_MESSAGE_TYPE_LEGTH, {msgType, 0}};
        PushFrontOption(&reply->options, &optMsgType);
    } else {
        if (pOptMsgType->data[0] != msgType) {
            LOGD("error dhcp nak message type.");
            return RET_FAILED;
        }
    }
    return RET_SUCCESS;
}

static int SendDhcpOffer(PDhcpServerContext ctx, PDhcpMsgInfo reply)
{
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        return RET_FAILED;
    }
    if (AppendReplyTypeOption(reply, REPLY_OFFER) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (AppendReplyTimeOptions(ctx, &reply->options) != RET_SUCCESS ||
        AddReplyServerIdOption(&reply->options, srvIns->addressPool.serverId) != RET_SUCCESS) {
            return RET_FAILED;
    }
    if (AppendFixedOptions(ctx, reply) != RET_SUCCESS) {
        LOGW("failed to append fixed reply options.");
    }
    if (ParseReplyOptions(reply) != RET_SUCCESS) {
        LOGE("failed to parse reply options.");
        return RET_FAILED;
    }
    int ret;
    struct sockaddr_in *bcastAddrIn = BroadcastAddrIn();
    struct sockaddr_in *destAddrIn = DestinationAddrIn();
    if (srvIns->broadCastFlagEnable == 1 && destAddrIn) {
        int broadCastFlag = 1;
        if (reply->packet.flags && (reply->packet.flags >> (DHCP_MESSAGE_FLAG_LENGTH - 1)) == 0) {
            broadCastFlag = 0;
        }
        if (!broadCastFlag && destAddrIn->sin_addr.s_addr) {
            ret = sendto(srvIns->serverFd, &reply->packet, reply->length, 0, (struct sockaddr *)destAddrIn,
                sizeof(*destAddrIn));
        } else {
            ret = sendto(srvIns->serverFd, &reply->packet, reply->length, 0, (struct sockaddr *)bcastAddrIn,
                sizeof(*bcastAddrIn));
        }
    } else {
        ret = sendto(
            srvIns->serverFd, &reply->packet, reply->length, 0, (struct sockaddr *)bcastAddrIn, sizeof(*bcastAddrIn));
    }
    if (!ret) {
        LOGD("failed to send dhcp offer message.");
        return RET_FAILED;
    }
    LOGI(" send reply offer, length:%d", reply->length);
    return RET_SUCCESS;
}

static int SendDhcpAck(PDhcpServerContext ctx, PDhcpMsgInfo reply)
{
    if (AppendReplyTypeOption(reply, REPLY_ACK) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (AppendFixedOptions(ctx, reply) != RET_SUCCESS) {
        LOGW("failed to append fixed reply options.");
    }
    if (!ctx || !ctx->instance) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);

    if (AppendReplyTimeOptions(ctx, &reply->options) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (AddReplyServerIdOption(&reply->options, srvIns->addressPool.serverId) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (ParseReplyOptions(reply) != RET_SUCCESS) {
        return RET_FAILED;
    }
    sockaddr_in *destAddrIn = DestinationAddrIn();
    int ret = sendto(srvIns->serverFd, &reply->packet, reply->length, 0, (struct sockaddr *)destAddrIn,
        sizeof(*destAddrIn));
    if (!ret) {
        LOGD("failed to send dhcp ack message.");
        return RET_FAILED;
    }
    LOGI(" send reply ack, size:%d", reply->length);
    return RET_SUCCESS;
}

static int SendDhcpNak(PDhcpServerContext ctx, PDhcpMsgInfo reply)
{
    if (AppendReplyTypeOption(reply, REPLY_NAK) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (!ctx || !ctx->instance) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (ParseReplyOptions(reply) != RET_SUCCESS) {
        return RET_FAILED;
    }

    struct sockaddr_in *destAddrIn = DestinationAddrIn();
    int ret = sendto(srvIns->serverFd, &reply->packet, reply->length, 0, (struct sockaddr *)destAddrIn,
        sizeof(*destAddrIn));
    if (!ret) {
        LOGD("failed to send dhcp ack message.");
        return RET_FAILED;
    }
    LOGI(" send reply nak, size:%d", reply->length);
    return RET_SUCCESS;
}

static int ParseMessageOptions(PDhcpMsgInfo msg)
{
    if (msg->length < (DHCP_MSG_HEADER_SIZE + MAGIC_COOKIE_LENGTH)) {
        return RET_FAILED;
    }
    DhcpOption *current, *end;
    current = (DhcpOption *)msg->packet.options;
    end = (DhcpOption *)(((uint8_t *)msg->packet.options) + (msg->length - DHCP_MSG_HEADER_SIZE));

    if (memcmp(current, MAGIC_COOKIE_DATA, sizeof(MAGIC_COOKIE_DATA)) != 0) {
        LOGD("bad magic cookie.");
        return RET_FAILED;
    }
    current = (DhcpOption *)(((uint8_t *)current) + MAGIC_COOKIE_LENGTH);
    uint8_t *pos = (((uint8_t *)current) + MAGIC_COOKIE_LENGTH);
    uint8_t *maxPos = (((uint8_t *)current) + (DHCP_OPTION_SIZE - MAGIC_COOKIE_LENGTH - OPT_HEADER_LENGTH -1));
    int optTotal = 0;
    while (current < end && current->code != END_OPTION) {
        pos += (OPT_HEADER_LENGTH + current->length);
        if (pos >= maxPos) {
            LOGD("out of option max pos.");
            return RET_FAILED;
        }
        if (PushBackOption(&msg->options, current) != RET_SUCCESS) {
            LOGD("faild to PushOption.");
        }
        current = (DhcpOption *)(((uint8_t *)current) + OPT_HEADER_LENGTH + current->length);
        optTotal++;
    }
    if (current < end && current->code == END_OPTION) {
        LOGD("option list size:%zu xid:%u", msg->options.size, msg->packet.xid);
        return RET_SUCCESS;
    }

    LOGD("option list parse failed.");
    return RET_FAILED;
}

static int ResetMessageOptions(PDhcpMsgInfo reply)
{
    if (!reply || reply->options.size == 0) {
        LOGE("message pointer is null.");
        return RET_ERROR;
    }
    if (memset_s(reply->packet.options, DHCP_OPTIONS_SIZE, 0, DHCP_OPTIONS_SIZE) != EOK) {
        LOGE("failed to reset message options!");
        return RET_ERROR;
    }
    return RET_SUCCESS;
}

static int ValidateReplyOptions(PDhcpMsgInfo reply)
{
    if (!reply) {
        LOGE("reply message pointer is null.");
        return RET_FAILED;
    }
    int ret = RET_FAILED;
    if ((ret = ResetMessageOptions(reply)) != RET_SUCCESS) {
        return ret;
    }
    reply->length = DHCP_MSG_HEADER_SIZE;
    PDhcpOptionNode pNode = reply->options.first;
    if (!pNode) {
        return RET_ERROR;
    }
    pNode = pNode->next;
    PDhcpOption pOptMsgType = GetOption(&reply->options, DHCP_MESSAGE_TYPE_OPTION);
    if (!pOptMsgType) {
        LOGE("unkown reply message type.");
        return ret;
    }
    return RET_SUCCESS;
}

static int ParseReplyOptions(PDhcpMsgInfo reply)
{
    int ret = RET_FAILED;
    if ((ret = ValidateReplyOptions(reply)) != RET_SUCCESS) {
        return ret;
    }
    PDhcpOptionNode pNode = reply->options.first->next;
    DhcpOption endOpt = {END_OPTION, 0, {0}};
    PushBackOption(&reply->options, &endOpt);
    int replyOptsLength = 0;
    uint8_t *current = reply->packet.options, olen = MAGIC_COOKIE_LENGTH;
    size_t remainingSize = sizeof(reply->packet.options);
    uint32_t cookie = htonl(DHCP_MAGIC_COOKIE);
    if (memcpy_s(current, remainingSize, &cookie, olen) != EOK) {
        LOGE("memcpy cookie out of options buffer!");
        return RET_FAILED;
    }
    replyOptsLength += olen;
    remainingSize -= olen;
    current += olen;
    ret = RET_SUCCESS;
    while (pNode && (uint32_t)pNode->option.length < DHCP_OPTION_SIZE) {
        if ((uint32_t)pNode->option.code == END_OPTION) {
            olen = OPT_HEADER_LENGTH + 1;
        } else {
            olen = OPT_HEADER_LENGTH + pNode->option.length;
        }
        if (memcpy_s(current, remainingSize, &pNode->option, olen) != EOK) {
            LOGE("memcpy current option out of options buffer!");
            ret = RET_FAILED;
            break;
        }
        remainingSize -= olen;
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
    reply->length += replyOptsLength;
    return ret;
}

void RegisterDhcpCallback(PDhcpServerContext ctx, DhcpServerCallback callback)
{
    LOGI("register server callback.");
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return;
    }
    srvIns->callback = callback;
}

static int InitServerContext(DhcpConfig *config, DhcpServerContext *ctx)
{
    if (!config) {
        LOGE("server configure pointer is null.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    if (InitAddressPool(&srvIns->addressPool, config->ifname, NULL) != RET_SUCCESS) {
        LOGD("failed to init address pool.");
        return RET_FAILED;
    }
    if (memcpy_s(ctx->ifname, sizeof(ctx->ifname), config->ifname, strlen(config->ifname)) != EOK) {
        LOGD("failed to set interface name.");
        return RET_FAILED;
    }
    srvIns->serverFd = 0;
    srvIns->callback = 0;
    srvIns->looperState = LS_IDLE;
    srvIns->broadCastFlagEnable = config->broadcast;
    srvIns->addressPool.serverId = config->serverId;
    srvIns->addressPool.netmask = config->netmask;
    srvIns->addressPool.gateway = config->gateway;
    if (config->pool.beginAddress && config->pool.endAddress) {
        srvIns->addressPool.addressRange.beginAddress = config->pool.beginAddress;
        srvIns->addressPool.addressRange.endAddress = config->pool.endAddress;
    } else {
        srvIns->addressPool.addressRange.beginAddress = FirstIpAddress(config->serverId, config->netmask);
        srvIns->addressPool.addressRange.endAddress = LastIpAddress(config->serverId, config->netmask);
    }
    if (memcpy_s(srvIns->addressPool.ifname, sizeof(srvIns->addressPool.ifname),
        config->ifname, strlen(config->ifname)) != EOK) {
        LOGD("failed to set interface name.");
        return RET_FAILED;
    }
    if (!CheckAddressRange(&srvIns->addressPool)) {
        LOGE("failed to validate address range.");
        return RET_FAILED;
    }
    InitLeaseFile(&srvIns->addressPool);
    return RET_SUCCESS;
}

int InitServerFixedOptions(DhcpConfig *config, DhcpServerContext *ctx)
{
    if (!config) {
        LOGE("server configure pointer is null.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }

    if (!HasInitialized(&config->options)) {
        LOGE("dhcp configure has not been initialized.");
        return RET_FAILED;
    }
    if (InitOptionList(&srvIns->addressPool.fixedOptions) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (config->options.first != NULL && config->options.size > 0) {
        DhcpOptionNode *pNode = config->options.first->next;
        for (size_t i = 0; pNode != NULL && i < config->options.size; i++) {
            PushBackOption(&srvIns->addressPool.fixedOptions, &pNode->option);
            LOGD("append fixed option ==> %hhu,%d", pNode->option.code,
                pNode->option.length);
            pNode = pNode->next;
        }
    }
    return RET_SUCCESS;
}

PDhcpServerContext InitializeServer(DhcpConfig *config)
{
    DhcpServerContext *context = NULL;
    if (!config) {
        LOGE("dhcp server config pointer is null.");
        return NULL;
    }
    if (strlen(config->ifname) == 0) {
        LOGE("can't found interface name config.");
        return NULL;
    }
    if (!config->serverId || !config->netmask) {
        LOGE("missing required parameter or config item: \"serverId\", \"netmask\"");
        return NULL;
    }
    if ((context = (DhcpServerContext *)calloc(1, sizeof(DhcpServerContext))) == NULL) {
        LOGE("failed to calloc server context.");
        return NULL;
    }
    if ((context->instance = calloc(1, sizeof(ServerContext))) == NULL) {
        LOGE("failed to calloc server instance.");
        FreeServerContext(context);
        return NULL;
    }
    if (InitServerContext(config, context) != RET_SUCCESS) {
        LOGE("failed initialize dhcp server context.");
        FreeServerContext(context);
        return NULL;
    }
    if (InitServerFixedOptions(config, context) != RET_SUCCESS) {
        LOGE("failed initialize dhcp server fixed options.");
        FreeServerContext(context);
        return NULL;
    }
    LOGD("server id: %s", ParseStrIp(config->serverId));
    LOGD("netmask: %s", ParseStrIp(config->netmask));
    if (config->gateway) {
        LOGD("gateway: %s", ParseStrIp(config->gateway));
    }
    LOGD("address range begin of: %s", ParseStrIp(config->pool.beginAddress));
    LOGD("address range end of: %s", ParseStrIp(config->pool.endAddress));
    context->instance->initialized = 1;
    return context;
}

int FreeServerContext(PDhcpServerContext ctx)
{
    if (!ctx) {
        LOGE("dhcp server context pointer is null.");
        return RET_FAILED;
    }
    ServerContext *srvIns = GetServerInstance(ctx);
    if (!srvIns) {
        LOGE("dhcp server instance pointer is null.");
        return RET_FAILED;
    }
    FreeAddressPool(&srvIns->addressPool);
    if (ctx->instance != NULL) {
        free(ctx->instance);
        ctx->instance = NULL;
        free(ctx);
        ctx = NULL;
    }
    return RET_SUCCESS;
}