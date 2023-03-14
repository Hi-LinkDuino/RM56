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
#include "dhcp_ipv4.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>

#include "securec.h"
#include "dhcp_client.h"
#include "dhcp_options.h"
#include "dhcp_socket.h"
#include "dhcp_function.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpIpv4"

struct ArpPacket {
    struct ethhdr ethh;                 /* Ethernet header */
    struct arphdr arph;                 /* Arp header */
    u_char  sHaddr[6];                  /* sender's hardware address */
    u_char  sInaddr[4];                 /* sender's IP address */
    u_char  tHaddr[6];                  /* target's hardware address */
    u_char  tInaddr[4];                 /* target's IP address */
    u_char  pad[18];                    /* pad for min. Ethernet payload (60 bytes) */
};

/* static defined */
static int g_dhcp4State = DHCP_STATE_INIT;
static int g_sockFd = -1;
static int g_sigSockFds[NUMBER_TWO];
static uint32_t g_sentPacketNum = 0;
static uint32_t g_timeoutTimestamp = 0;
static uint32_t g_renewalTimestamp = 0;
static uint32_t g_leaseTime = 0;
static uint32_t g_renewalSec = 0;
static uint32_t g_rebindSec = 0;
static uint32_t g_requestedIp4 = 0;
static uint32_t g_serverIp4 = 0;
static uint32_t g_socketMode = SOCKET_MODE_INVALID;
static uint32_t g_transID = 0;

static struct DhcpClientCfg *g_cltCnf;

int GetPacketReadSockFd(void)
{
    return g_sockFd;
}

int GetSigReadSockFd(void)
{
    return g_sigSockFds[0];
}

uint32_t GetDhcpTransID(void)
{
    return g_transID;
}

/* Send signals. */
static void SignalHandler(int signum)
{
    switch (signum) {
        case SIGTERM:
            /* Send signal SIGTERM. */
        case SIGUSR1:
            /* Send signal SIGUSR1. */
        case SIGUSR2:
            /* Send signal SIGUSR2. */
            send(g_sigSockFds[1], &signum, sizeof(signum), MSG_DONTWAIT);
            break;
        default:
            break;
    }
}

/* Set the socket mode. */
void SetSocketMode(uint32_t mode)
{
    close(g_sockFd);
    g_sockFd = -1;
    g_socketMode = mode;
    LOGI("SetSocketMode() the socket mode %{public}s.", (mode == SOCKET_MODE_RAW) ? "raw"
        : ((mode == SOCKET_MODE_KERNEL) ? "kernel" : "not valid"));
}

/* Execution dhcp release. */
int ExecDhcpRelease(void)
{
    /* Ensure that we've received dhcp ack packet completely. */
    if ((g_dhcp4State == DHCP_STATE_BOUND) || (g_dhcp4State == DHCP_STATE_RENEWING) ||
        (g_dhcp4State == DHCP_STATE_REBINDING)) {
        /* Unicast dhcp release packet. */
        DhcpRelease(g_requestedIp4, g_serverIp4);
    }

    g_dhcp4State = DHCP_STATE_RELEASED;
    SetSocketMode(SOCKET_MODE_INVALID);

    /* Ensure that the function select() is always blocked and don't need to receive ip from dhcp server. */
    g_timeoutTimestamp = SIGNED_INTEGER_MAX;

    LOGI("ExecDhcpRelease() enter released state...");
    return DHCP_OPT_SUCCESS;
}

/* Execution dhcp renew. */
int ExecDhcpRenew(void)
{
    /* Set socket mode and dhcp ipv4 state, make sure dhcp packets can be sent normally. */
    switch (g_dhcp4State) {
        case DHCP_STATE_INIT:
        case DHCP_STATE_SELECTING:
            LOGI("ExecDhcpRenew() dhcp ipv4 old state:%{public}d, no need change state.", g_dhcp4State);
            break;
        case DHCP_STATE_REQUESTING:
        case DHCP_STATE_RELEASED:
        case DHCP_STATE_RENEWED:
            LOGI("ExecDhcpRenew() dhcp ipv4 old state:%{public}d, init state:INIT.", g_dhcp4State);
            /* Init socket mode and dhcp ipv4 state. */
            g_dhcp4State = DHCP_STATE_INIT;
            SetSocketMode(SOCKET_MODE_RAW);
            break;
        case DHCP_STATE_BOUND:
            /* Set socket mode, send unicast packet. */
            SetSocketMode(SOCKET_MODE_KERNEL);
        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
            LOGI("ExecDhcpRenew() dhcp ipv4 old state:%{public}d, set state:RENEWED.", g_dhcp4State);
            /* Set dhcp ipv4 state, send request packet. */
            g_dhcp4State = DHCP_STATE_RENEWED;
            break;
        default:
            break;
    }

    /* Start record again, go back to init state. */
    g_sentPacketNum = 0;
    g_timeoutTimestamp = 0;

    LOGI("ExecDhcpRenew() a dhcp renew is executed...");
    return DHCP_OPT_SUCCESS;
}

/* Add dhcp option paramater request list. */
static void AddParamaterRequestList(struct DhcpPacket *packet)
{
    int end = GetEndOptionIndex(packet->options);
    int i;
    int len = 0;
    uint8_t arrReqCode[DHCP_REQ_CODE_NUM] = {SUBNET_MASK_OPTION,
        ROUTER_OPTION,
        DOMAIN_NAME_SERVER_OPTION,
        HOST_NAME_OPTION,
        DOMAIN_NAME_OPTION,
        BROADCAST_ADDRESS_OPTION};

    packet->options[end + DHCP_OPT_CODE_INDEX] = PARAMETER_REQUEST_LIST_OPTION;
    for (i = 0; i < DHCP_REQ_CODE_NUM; i++) {
        if ((arrReqCode[i] > PAD_OPTION) && (arrReqCode[i] < END_OPTION)) {
            packet->options[end + DHCP_OPT_DATA_INDEX + len++] = arrReqCode[i];
        }
    }
    packet->options[end + DHCP_OPT_LEN_INDEX] = len;
    packet->options[end + DHCP_OPT_DATA_INDEX + len] = END_OPTION;
}

/* Init the socket fd. */
static void InitSocketFd(void)
{
    if (g_sockFd < 0) {
        if (g_socketMode == SOCKET_MODE_INVALID) {
            return;
        }

        bool bInitSuccess = true;
        if (g_socketMode == SOCKET_MODE_RAW) {
            if ((CreateRawSocket(&g_sockFd) != SOCKET_OPT_SUCCESS) ||
                (BindRawSocket(g_sockFd, g_cltCnf->ifaceIndex, NULL) != SOCKET_OPT_SUCCESS)) {
                LOGE("InitSocketFd() fd:%{public}d,index:%{public}d failed!", g_sockFd, g_cltCnf->ifaceIndex);
                bInitSuccess = false;
            }
        } else {
            if ((CreateKernelSocket(&g_sockFd) != SOCKET_OPT_SUCCESS) ||
                (BindKernelSocket(g_sockFd, g_cltCnf->ifaceName, INADDR_ANY, BOOTP_CLIENT, true) !=
                    SOCKET_OPT_SUCCESS)) {
                LOGE("InitSocketFd() fd:%{public}d,ifname:%{public}s failed!", g_sockFd, g_cltCnf->ifaceName);
                bInitSuccess = false;
            }
        }
        if (!bInitSuccess || (g_sockFd < 0)) {
            LOGE("InitSocketFd() %{public}d err:%{public}d, couldn't listen on socket!", g_sockFd, errno);
            unlink(g_cltCnf->pidFile);
            unlink(g_cltCnf->resultFile);
            exit(EXIT_SUCCESS);
        }
    }
}

/* Obtains a random number as the trans id. */
static uint32_t GetTransId(void)
{
    static bool bSranded = false;
    if (!bSranded) {
        unsigned int uSeed = 0;
        int nFd = -1;
        if ((nFd = open("/dev/urandom", 0)) == -1) {
            LOGE("GetTransId() open /dev/urandom failed, error:%{public}d!", errno);
            uSeed = time(NULL);
        } else {
            if (read(nFd, &uSeed, sizeof(uSeed)) == -1) {
                LOGE("GetTransId() read /dev/urandom failed, error:%{public}d!", errno);
                uSeed = time(NULL);
            }
            LOGI("GetTransId() read /dev/urandom uSeed:%{public}u.", uSeed);
            close(nFd);
        }
        srandom(uSeed);
        bSranded = true;
    }
    return random();
}

static void InitSelecting(time_t timestamp)
{
    if (g_sentPacketNum > TIMEOUT_TIMES_MAX) {
        /* Send packet timed out, now exit process. */
        LOGW("InitSelecting() send packet timed out %{public}u times, now exit process!", g_sentPacketNum);
        g_timeoutTimestamp = timestamp + TIMEOUT_MORE_WAIT_SEC;
        g_sentPacketNum = 0;
        g_cltCnf->timeoutExit = true;
        return;
    }

    if (g_sentPacketNum == 0) {
        g_transID = GetTransId();
    }

    /* Broadcast dhcp discover packet. */
    DhcpDiscover(g_transID, g_requestedIp4);
    if (g_dhcp4State != DHCP_STATE_SELECTING) {
        g_dhcp4State = DHCP_STATE_SELECTING;
    }

    uint32_t uTimeoutSec = TIMEOUT_WAIT_SEC << g_sentPacketNum;
    g_timeoutTimestamp = timestamp + uTimeoutSec;
    LOGI("InitSelecting() DhcpDiscover g_sentPacketNum:%{public}u,timeoutSec:%{public}u,timestamp:%{public}u.",
        g_sentPacketNum,
        uTimeoutSec,
        g_timeoutTimestamp);

    g_sentPacketNum++;
}

static struct DhcpPacket *ReadLease(void)
{
    if (g_cltCnf->leaseFile[0] == '\0') {
        return NULL;
    }

    int fd;
    struct DhcpPacket *dhcp = NULL;
    ssize_t rBytes;

    fd = open(g_cltCnf->leaseFile, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }

    dhcp = calloc(1, sizeof(*dhcp));
    if (dhcp == NULL) {
        close(fd);
        return NULL;
    }

    rBytes = read(fd, (uint8_t *)dhcp, sizeof(*dhcp));
    close(fd);
    if (rBytes <= 0 || (size_t)rBytes < offsetof(struct DhcpPacket, options)) {
        free(dhcp);
        dhcp = NULL;
        return NULL;
    }

    return dhcp;
}

static ssize_t WriteLease(struct DhcpPacket *pkt)
{
    if (pkt == NULL) {
        return -1;
    }

    if (g_cltCnf->leaseFile[0] == '\0') {
        return -1;
    }

    int fd;
    ssize_t wBytes;

    fd = open(g_cltCnf->leaseFile, O_WRONLY | O_CREAT | O_TRUNC, RWMODE);
    if (fd < 0) {
        return -1;
    }

    wBytes = write(fd, (uint8_t *)pkt, sizeof(*pkt));
    close(fd);
    return wBytes;
}

static void AddParamaterRebootList(struct DhcpPacket *packet)
{
    int end = GetEndOptionIndex(packet->options);
    int i;
    int len = 0;
    uint8_t arrReqCode[DHCP_REQ_CODE_NUM] = {SUBNET_MASK_OPTION,
        STATIC_ROUTE_OPTION,
        ROUTER_OPTION,
        BROADCAST_ADDRESS_OPTION,
        IP_ADDRESS_LEASE_TIME_OPTION,
        RENEWAL_TIME_VALUE_OPTION,
        REBINDING_TIME_VALUE_OPTION};

    packet->options[end + DHCP_OPT_CODE_INDEX] = PARAMETER_REQUEST_LIST_OPTION;
    for (i = 0; i < DHCP_REQ_CODE_NUM; i++) {
        if ((arrReqCode[i] > PAD_OPTION) && (arrReqCode[i] < END_OPTION)) {
            packet->options[end + DHCP_OPT_DATA_INDEX + len++] = arrReqCode[i];
        }
    }
    packet->options[end + DHCP_OPT_LEN_INDEX] = len;
    packet->options[end + DHCP_OPT_DATA_INDEX + len] = END_OPTION;
}

static int DhcpReboot(uint32_t transid, uint32_t reqip)
{
    LOGI("DhcpReboot() enter");
    struct DhcpPacket packet;
    if (memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) != EOK) {
        return -1;
    }

    /* Get packet header and common info. */
    if (GetPacketHeaderInfo(&packet, DHCP_REQUEST) != DHCP_OPT_SUCCESS) {
        return -1;
    }

    if (memcpy_s(packet.chaddr, sizeof(packet.chaddr), g_cltCnf->ifaceMac, MAC_ADDR_LEN) != EOK) {
        LOGE("DhcpReboot() failed, memcpy_s error!");
        return -1;
    }

    /* Get packet not common info. */
    packet.xid = transid;
    AddOptValueToOpts(packet.options, REQUESTED_IP_ADDRESS_OPTION, reqip);
    AddOptValueToOpts(packet.options, MAXIMUM_DHCP_MESSAGE_SIZE_OPTION, MAX_MSG_SIZE);
    AddOptValueToOpts(packet.options, FORCERENEW_NONCE_OPTION, 1);
    AddParamaterRebootList(&packet);

    /* Begin broadcast dhcp request packet. */
    char *pReqIp = Ip4IntConToStr(reqip, false);
    if (pReqIp != NULL) {
        LOGI("DhcpReboot() broadcast req packet, reqip: host %{private}u->%{private}s.", ntohl(reqip), pReqIp);
        free(pReqIp);
        pReqIp = NULL;
    }
    return SendToDhcpPacket(&packet, INADDR_ANY, INADDR_BROADCAST, g_cltCnf->ifaceIndex, (uint8_t *)MAC_BCAST_ADDR);
}

void SendReboot(struct DhcpPacket *p, time_t timestamp)
{
    if (p == NULL) {
        return;
    }

    g_requestedIp4 = p->yiaddr;

    free(p);
    p = NULL;

    g_transID = GetTransId();
    g_dhcp4State = DHCP_STATE_INITREBOOT;
    g_sentPacketNum = 0;
    g_timeoutTimestamp = timestamp + g_renewalSec;
    DhcpReboot(g_transID, g_requestedIp4);
}

static void Reboot(time_t timestamp)
{
    struct DhcpPacket *pkt = ReadLease();
    if (pkt == NULL) {
        return;
    }

    uint32_t leaseTime;
    uint32_t renewalTime;
    uint32_t rebindTime;
    uint32_t interval;
    struct stat st;
    if (!GetDhcpOptionUint32(pkt, IP_ADDRESS_LEASE_TIME_OPTION, &leaseTime)) {
        leaseTime = ~0U;
    }

    if (leaseTime != ~0U && stat(g_cltCnf->leaseFile, &st) == 0) {
        if (timestamp == (time_t)-1 || timestamp < st.st_mtime || (time_t)leaseTime < timestamp - st.st_mtime) {
            LOGI("Reboot read lease file leaseTime expire");
            free(pkt);
            pkt = NULL;
            return;
        } else {
            interval = timestamp - st.st_mtime;
            leaseTime -= interval;
            renewalTime = leaseTime * RENEWAL_SEC_MULTIPLE;
            rebindTime = leaseTime * REBIND_SEC_MULTIPLE;
            LOGI("Reboot read lease file leaseTime:%{public}u, renewalTime:%{public}u, rebindTime:%{public}u",
                leaseTime, renewalTime, rebindTime);
            free(pkt);
            pkt = NULL;
            return;
        }
    } else {
        LOGI("Reboot read lease file leaseTime option not found");
        free(pkt);
        pkt = NULL;
        return;
    }

    g_leaseTime = leaseTime;
    g_renewalSec = renewalTime;
    g_rebindSec = rebindTime;

    SendReboot(pkt, timestamp);
}

static void Requesting(time_t timestamp)
{
    if (g_sentPacketNum > TIMEOUT_TIMES_MAX) {
        /* Send packet timed out, now enter init state. */
        g_dhcp4State = DHCP_STATE_INIT;
        SetSocketMode(SOCKET_MODE_RAW);
        g_sentPacketNum = 0;
        g_timeoutTimestamp = timestamp;
        return;
    }

    if (g_dhcp4State == DHCP_STATE_RENEWED) {
        /* Unicast dhcp request packet in the renew state. */
        DhcpRenew(g_transID, g_requestedIp4, g_serverIp4);
    } else {
        /* Broadcast dhcp request packet in the requesting state. */
        DhcpRequest(g_transID, g_requestedIp4, g_serverIp4);
    }

    uint32_t uTimeoutSec = TIMEOUT_WAIT_SEC << g_sentPacketNum;
    g_timeoutTimestamp = timestamp + uTimeoutSec;
    LOGI("Requesting() DhcpRequest g_sentPacketNum:%{public}u,timeoutSec:%{public}u,g_timeoutTimestamp:%{public}u.",
        g_sentPacketNum,
        uTimeoutSec,
        g_timeoutTimestamp);

    g_sentPacketNum++;
}

static void Renewing(time_t timestamp)
{
    if ((g_renewalSec + TIME_INTERVAL_MAX) < g_rebindSec) {
        /* Cur time is between renewal and rebind time, unicast dhcp request packet in the renew state. */
        DhcpRenew(g_transID, g_requestedIp4, g_serverIp4);

        /* Set a new renewal time. */
        g_renewalSec += (g_rebindSec - g_renewalSec) / NUMBER_TWO;
        g_timeoutTimestamp = g_renewalTimestamp + g_renewalSec;
        LOGI("Renewing() DhcpRenew unicast renewalTime:%{public}u,renewal:%{public}u,timeoutTime:%{public}u, "
                  "rebind:%{public}u.",
            g_renewalTimestamp,
            g_renewalSec,
            g_timeoutTimestamp,
            g_rebindSec);
    } else {
        /* Cur time reaches rebind time, now enter rebinding state. */
        g_dhcp4State = DHCP_STATE_REBINDING;
        LOGI("Renewing() cur time reaches rebind time, now enter rebinding state...");
        g_timeoutTimestamp = timestamp + (g_rebindSec - g_renewalSec);
        LOGI("Renewing() timestamp:%{public}d,rebind:%{public}u,renewal:%{public}u, timeoutTime:%{public}u.",
            (int)timestamp, g_rebindSec, g_renewalSec, g_timeoutTimestamp);
    }
}

static void Rebinding(time_t timestamp)
{
    if ((g_rebindSec + TIME_INTERVAL_MAX) < g_leaseTime) {
        /* Cur time is between rebind and lease time, broadcast dhcp request packet in the rebind state. */
        DhcpRenew(g_transID, g_requestedIp4, 0);

        /* Set a new rebind time. */
        g_rebindSec += (g_leaseTime - g_rebindSec) / NUMBER_TWO;
        g_timeoutTimestamp = g_renewalTimestamp + g_rebindSec;
        LOGI("Rebinding() DhcpRenew broadcast renewalTime:%{public}u,rebind:%{public}u,timeoutTime:%{public}u, "
                  "lease:%{public}u.",
            g_renewalTimestamp,
            g_rebindSec,
            g_timeoutTimestamp,
            g_leaseTime);
    } else {
        /* Cur time reaches lease time, send packet timed out, now enter init state. */
        LOGI("Rebinding() 555 cur time reaches lease time, now enter init state...");
        g_dhcp4State = DHCP_STATE_INIT;
        SetSocketMode(SOCKET_MODE_RAW);
        g_sentPacketNum = 0;
        g_timeoutTimestamp = timestamp;
        return;
    }
}

static void DhcpRequestHandle(time_t timestamp)
{
    switch (g_dhcp4State) {
        case DHCP_STATE_INIT:
        case DHCP_STATE_SELECTING:
            InitSelecting(timestamp);
            break;
        case DHCP_STATE_REQUESTING:
        case DHCP_STATE_RENEWED:
            Requesting(timestamp);
            break;
        case DHCP_STATE_BOUND:
            /* Now the renewal time run out, ready to enter renewing state. */
            LOGI("DhcpRequestHandle() 333 the renewal time run out, ready to enter renewing state...");
            g_dhcp4State = DHCP_STATE_RENEWING;
            SetSocketMode(SOCKET_MODE_KERNEL);
        case DHCP_STATE_RENEWING:
            Renewing(timestamp);
            break;
        case DHCP_STATE_REBINDING:
            Rebinding(timestamp);
            break;
        case DHCP_STATE_INITREBOOT:
            g_dhcp4State = DHCP_STATE_INIT;
            break;
        case DHCP_STATE_RELEASED:
            /* Ensure that the function select() is always blocked and don't need to receive ip from dhcp server. */
            g_timeoutTimestamp = SIGNED_INTEGER_MAX;
            break;
        default:
            break;
    }
}

static void DhcpOfferPacketHandle(uint8_t type, const struct DhcpPacket *packet, time_t timestamp)
{
    if (type != DHCP_OFFER) {
        LOGE("DhcpOfferPacketHandle() type:%{public}d error!", type);
        return;
    }

    if (packet == NULL) {
        LOGE("DhcpOfferPacketHandle() type:%{public}d error, packet == NULL!", type);
        return;
    }

    uint32_t u32Data = 0;
    if (!GetDhcpOptionUint32(packet, SERVER_IDENTIFIER_OPTION, &u32Data)) {
        LOGE("DhcpOfferPacketHandle() type:%{public}d GetDhcpOptionUint32 SERVER_IDENTIFIER_OPTION failed!", type);
        return;
    }

    g_transID = packet->xid;
    g_requestedIp4 = packet->yiaddr;
    g_serverIp4 = htonl(u32Data);

    char *pReqIp = Ip4IntConToStr(g_requestedIp4, false);
    if (pReqIp != NULL) {
        LOGI(
            "DhcpOfferPacketHandle() receive DHCP_OFFER, xid:%{public}u, requestIp: host %{private}u->%{private}s.",
            g_transID,
            ntohl(g_requestedIp4),
            pReqIp);
        free(pReqIp);
        pReqIp = NULL;
    }
    char *pSerIp = Ip4IntConToStr(g_serverIp4, false);
    if (pSerIp != NULL) {
        LOGI("DhcpOfferPacketHandle() receive DHCP_OFFER, serverIp: host %{private}u->%{private}s.",
            ntohl(g_serverIp4),
            pSerIp);
        free(pSerIp);
        pSerIp = NULL;
    }

    /* Receive dhcp offer packet finished, next send dhcp request packet. */
    g_dhcp4State = DHCP_STATE_REQUESTING;
    g_sentPacketNum = 0;
    g_timeoutTimestamp = timestamp;
}

static void ParseOtherNetworkInfo(const struct DhcpPacket *packet, struct DhcpResult *result)
{
    if ((packet == NULL) || (result == NULL)) {
        LOGE("ParseOtherNetworkInfo() error, packet == NULL or result == NULL!");
        return;
    }

    uint32_t u32Data = 0;
    uint32_t u32Data2 = 0;
    if (GetDhcpOptionUint32n(packet, DOMAIN_NAME_SERVER_OPTION, &u32Data, &u32Data2)) {
        char *pDnsIp = Ip4IntConToStr(u32Data, true);
        if (pDnsIp != NULL) {
            LOGI("ParseOtherNetworkInfo() recv DHCP_ACK 6, dns1: %{private}u->%{private}s.", u32Data, pDnsIp);
            if (strncpy_s(result->strOptDns1, INET_ADDRSTRLEN, pDnsIp, INET_ADDRSTRLEN - 1) != EOK) {
                free(pDnsIp);
                pDnsIp = NULL;
                return;
            }
            free(pDnsIp);
            pDnsIp = NULL;
        }
        if ((u32Data2 > 0) && ((pDnsIp = Ip4IntConToStr(u32Data2, true)) != NULL)) {
            LOGI("ParseOtherNetworkInfo() recv DHCP_ACK 6, dns2: %{private}u->%{private}s.", u32Data2, pDnsIp);
            if (strncpy_s(result->strOptDns2, INET_ADDRSTRLEN, pDnsIp, INET_ADDRSTRLEN - 1) != EOK) {
                LOGE("ParseOtherNetworkInfo() strncpy_s Failed.");
            }
            free(pDnsIp);
            pDnsIp = NULL;
            return;
        }
    }
    return;
}

static void ParseNetworkInfo(const struct DhcpPacket *packet, struct DhcpResult *result)
{
    if ((packet == NULL) || (result == NULL)) {
        LOGE("ParseNetworkInfo() error, packet == NULL or result == NULL!");
        return;
    }

    char *pReqIp = Ip4IntConToStr(g_requestedIp4, false);
    if (pReqIp != NULL) {
        LOGI("ParseNetworkInfo() recv DHCP_ACK yiaddr: %{private}u->%{private}s.", ntohl(g_requestedIp4), pReqIp);
        if (strncpy_s(result->strYiaddr, INET_ADDRSTRLEN, pReqIp, INET_ADDRSTRLEN - 1) != EOK) {
            LOGI("ParseNetworkInfo() strncpy_s failed!");
            free(pReqIp);
            pReqIp = NULL;
            return;
        }
        free(pReqIp);
        pReqIp = NULL;
    }

    uint32_t u32Data = 0;
    if (GetDhcpOptionUint32(packet, SUBNET_MASK_OPTION, &u32Data)) {
        char *pSubIp = Ip4IntConToStr(u32Data, true);
        if (pSubIp != NULL) {
            LOGI("ParseNetworkInfo() recv DHCP_ACK 1, subnetmask: %{private}u->%{private}s.", u32Data, pSubIp);
            if (strncpy_s(result->strOptSubnet, INET_ADDRSTRLEN, pSubIp, INET_ADDRSTRLEN - 1) != EOK) {
                free(pSubIp);
                pSubIp = NULL;
                return;
            }
            free(pSubIp);
            pSubIp = NULL;
        }
    }

    u32Data = 0;
    uint32_t u32Data2 = 0;
    if (GetDhcpOptionUint32n(packet, ROUTER_OPTION, &u32Data, &u32Data2)) {
        char *pRouterIp = Ip4IntConToStr(u32Data, true);
        if (pRouterIp != NULL) {
            LOGI("ParseNetworkInfo() recv DHCP_ACK 3, router1: %{private}u->%{private}s.", u32Data, pRouterIp);
            if (strncpy_s(result->strOptRouter1, INET_ADDRSTRLEN, pRouterIp, INET_ADDRSTRLEN - 1) != EOK) {
                free(pRouterIp);
                pRouterIp = NULL;
                return;
            }
            free(pRouterIp);
            pRouterIp = NULL;
        }
        if ((u32Data2 > 0) && ((pRouterIp = Ip4IntConToStr(u32Data2, true)) != NULL)) {
            LOGI("ParseNetworkInfo() recv DHCP_ACK 3, router2: %{private}u->%{private}s.", u32Data2, pRouterIp);
            if (strncpy_s(result->strOptRouter2, INET_ADDRSTRLEN, pRouterIp, INET_ADDRSTRLEN - 1) != EOK) {
                free(pRouterIp);
                pRouterIp = NULL;
                return;
            }
            free(pRouterIp);
            pRouterIp = NULL;
        }
    }

    ParseOtherNetworkInfo(packet, result);
}

static void FormatString(struct DhcpResult *result)
{
    if (result == NULL) {
        LOGE("FormatString() error, result == NULL!");
        return;
    }

    if (strlen(result->strYiaddr) == 0) {
        if (strncpy_s(result->strYiaddr, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptServerId) == 0) {
        if (strncpy_s(result->strOptServerId, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptSubnet) == 0) {
        if (strncpy_s(result->strOptSubnet, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptDns1) == 0) {
        if (strncpy_s(result->strOptDns1, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptDns2) == 0) {
        if (strncpy_s(result->strOptDns2, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptRouter1) == 0) {
        if (strncpy_s(result->strOptRouter1, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptRouter2) == 0) {
        if (strncpy_s(result->strOptRouter2, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) != EOK) {
            return;
        }
    }
    if (strlen(result->strOptVendor) == 0) {
        if (strncpy_s(result->strOptVendor, DHCP_FILE_MAX_BYTES, "*", DHCP_FILE_MAX_BYTES - 1) != EOK) {
            return;
        }
    }
}

static int WriteDhcpResult(struct DhcpResult *result)
{
    if (result == NULL) {
        LOGE("WriteDhcpResult() error, result == NULL!");
        return DHCP_OPT_FAILED;
    }

    /* Format dhcp result. */
    FormatString(result);

    uint32_t curTime = (uint32_t)time(NULL);
    if ((time_t)curTime == (time_t)-1) {
        return DHCP_OPT_FAILED;
    }
    LOGI("WriteDhcpResult() "
         "result->strYiaddr:%{private}s,strOptServerId:%{private}s,strOptSubnet:%{private}s,uOptLeasetime:%{public}u,"
         " curTime:%{public}u.",
        result->strYiaddr, result->strOptServerId, result->strOptSubnet, result->uOptLeasetime, curTime);
    FILE *pFile = fopen(g_cltCnf->resultFile, "w+");
    if (pFile == NULL) {
        LOGE("WriteDhcpResult fopen %{public}s err:%{public}d!", g_cltCnf->resultFile, errno);
        return DHCP_OPT_FAILED;
    }

    /* Lock the writing file. */
    if (flock(fileno(pFile), LOCK_EX) != 0) {
        LOGE("WriteDhcpResult() flock file:%{public}s LOCK_EX failed, error:%{public}d!", g_cltCnf->resultFile, errno);
        fclose(pFile);
        return DHCP_OPT_FAILED;
    }

    /* Format: IP4 timestamp cliIp servIp subnet dns1 dns2 router1 router2 vendor lease. */
    int nBytes = fprintf(pFile,
        "IP4 %u %s %s %s %s %s %s %s %s %u\n",
        curTime, result->strYiaddr, result->strOptServerId, result->strOptSubnet, result->strOptDns1,
        result->strOptDns2, result->strOptRouter1, result->strOptRouter2, result->strOptVendor, result->uOptLeasetime);
    if (nBytes <= 0) {
        LOGE("WriteDhcpResult() fprintf %{public}s error:%{public}d!", g_cltCnf->resultFile, errno);
        fclose(pFile);
        return DHCP_OPT_FAILED;
    }

    /* Unlock the writing file. */
    if (flock(fileno(pFile), LOCK_UN) != 0) {
        LOGE("WriteDhcpResult() flock file:%{public}s LOCK_UN failed, error:%{public}d!", g_cltCnf->resultFile, errno);
        fclose(pFile);
        return DHCP_OPT_FAILED;
    }

    if (fclose(pFile) != 0) {
        LOGE("WriteDhcpResult() fclose %{public}s error:%{public}d!", g_cltCnf->resultFile, errno);
        return DHCP_OPT_FAILED;
    }
    LOGI("WriteDhcpResult() fprintf %{public}s success, nBytes:%{public}d.", g_cltCnf->resultFile, nBytes);
    return DHCP_OPT_SUCCESS;
}

static int SyncDhcpResult(const struct DhcpPacket *packet, struct DhcpResult *result)
{
    if ((packet == NULL) || (result == NULL)) {
        LOGE("SyncDhcpResult() error, packet == NULL or result == NULL!");
        return DHCP_OPT_FAILED;
    }

    char *pVendor = GetDhcpOptionString(packet, VENDOR_SPECIFIC_INFO_OPTION);
    if (pVendor == NULL) {
        LOGW("SyncDhcpResult() recv DHCP_ACK 43, pVendor is NULL!");
    } else {
        LOGI("SyncDhcpResult() recv DHCP_ACK 43, pVendor is %{public}s.", pVendor);
        if (strncpy_s(result->strOptVendor, DHCP_FILE_MAX_BYTES, pVendor, DHCP_FILE_MAX_BYTES - 1) != EOK) {
            LOGE("SyncDhcpResult() error, strncpy_s pVendor failed!");
            free(pVendor);
            pVendor = NULL;
            return DHCP_OPT_FAILED;
        }
        free(pVendor);
        pVendor = NULL;
    }

    /* Set the specified client process interface network info. */
    if (SetLocalInterface(g_cltCnf->ifaceName, ntohl(g_requestedIp4)) != DHCP_OPT_SUCCESS) {
        LOGE("SyncDhcpResult() error, SetLocalInterface yiaddr:%{private}s failed!", result->strYiaddr);
        return DHCP_OPT_FAILED;
    }

    /* Wirte to the file. */
    if (WriteDhcpResult(result) != DHCP_OPT_SUCCESS) {
        LOGE("SyncDhcpResult() error, WriteDhcpResult result failed!");
        return DHCP_OPT_FAILED;
    }

    /* Publish dhcp success result event. */
    if (PublishDhcpResultEvent(g_cltCnf->ifaceName, PUBLISH_CODE_SUCCESS, result) != DHCP_OPT_SUCCESS) {
        LOGE("SyncDhcpResult() error, PublishDhcpResultEvent result failed!");
        return DHCP_OPT_FAILED;
    }

    LOGI("SyncDhcpResult() handle dhcp result success, ifaceName:%{public}s.", g_cltCnf->ifaceName);
    return DHCP_OPT_SUCCESS;
}

static void ParseDhcpAckPacket(const struct DhcpPacket *packet, time_t timestamp)
{
    if (packet == NULL) {
        LOGE("ParseDhcpAckPacket() error, packet == NULL!");
        return;
    }

    struct DhcpResult dhcpResult;
    if (memset_s(&dhcpResult, sizeof(struct DhcpResult), 0, sizeof(struct DhcpResult)) != EOK) {
        LOGE("ParseDhcpAckPacket() error, memset_s failed!");
        return;
    }

    /* Set default leasetime. */
    g_leaseTime = LEASETIME_DEFAULT * ONE_HOURS_SEC;
    g_requestedIp4 = packet->yiaddr;
    uint32_t u32Data = 0;
    if (GetDhcpOptionUint32(packet, IP_ADDRESS_LEASE_TIME_OPTION, &u32Data)) {
        g_leaseTime = u32Data;
        LOGI("ParseDhcpAckPacket() recv DHCP_ACK 51, lease:%{public}u.", g_leaseTime);
    }
    g_renewalSec = g_leaseTime * RENEWAL_SEC_MULTIPLE;  /* First renewal seconds. */
    g_rebindSec  = g_leaseTime * REBIND_SEC_MULTIPLE;   /* Second rebind seconds. */
    g_renewalTimestamp = timestamp;   /* Record begin renewing or rebinding timestamp. */
    dhcpResult.uOptLeasetime = g_leaseTime;
    LOGI("Last get lease:%{public}u,renewal:%{public}u,rebind:%{public}u.", g_leaseTime, g_renewalSec, g_rebindSec);

    if (!GetDhcpOptionUint32(packet, SERVER_IDENTIFIER_OPTION, &u32Data)) {
        LOGE("ParseDhcpAckPacket() GetDhcpOptionUint32 SERVER_IDENTIFIER_OPTION failed!");
    } else {
        g_serverIp4 = htonl(u32Data);
        char *pSerIp = Ip4IntConToStr(g_serverIp4, false);
        if (pSerIp != NULL) {
            LOGI("ParseDhcpAckPacket() recv DHCP_ACK 54, serid: %{private}u->%{private}s.", u32Data, pSerIp);
            if (strncpy_s(dhcpResult.strOptServerId, INET_ADDRSTRLEN, pSerIp, INET_ADDRSTRLEN - 1) != EOK) {
                free(pSerIp);
                pSerIp = NULL;
                return;
            }
            free(pSerIp);
            pSerIp = NULL;
        }
    }

    /* Parse the specified client process interface network info. */
    ParseNetworkInfo(packet, &dhcpResult);

    /* Sync the specified client process interface network info to the file. */
    if (SyncDhcpResult(packet, &dhcpResult) != DHCP_OPT_SUCCESS) {
        /* Publish dhcp failed result event. */
        PublishDhcpResultEvent(g_cltCnf->ifaceName, PUBLISH_CODE_FAILED, NULL);
        return;
    }

    /* Receive dhcp ack packet finished, g_leaseTime * T1 later enter renewing state. */
    g_dhcp4State = DHCP_STATE_BOUND;
    SetSocketMode(SOCKET_MODE_INVALID);
    g_timeoutTimestamp = timestamp + g_renewalSec;
}

static void DhcpAckOrNakPacketHandle(uint8_t type, struct DhcpPacket *packet, time_t timestamp)
{
    if ((type != DHCP_ACK) && (type != DHCP_NAK)) {
        LOGE("DhcpAckOrNakPacketHandle() type:%{public}d error!", type);
        if (g_dhcp4State == DHCP_STATE_INITREBOOT) {
            g_dhcp4State = DHCP_STATE_INIT;
            g_timeoutTimestamp = timestamp;
        }
        return;
    }

    if (packet == NULL) {
        LOGE("DhcpAckOrNakPacketHandle() type:%{public}d error, packet == NULL!", type);
        return;
    }

    if (type == DHCP_NAK) {
        /* If receive dhcp nak packet, init g_dhcp4State, resend dhcp discover packet. */
        LOGI("DhcpAckOrNakPacketHandle() receive DHCP_NAK 53, init g_dhcp4State, resend dhcp discover packet!");
        g_dhcp4State = DHCP_STATE_INIT;
        SetSocketMode(SOCKET_MODE_RAW);
        g_requestedIp4 = 0;
        g_sentPacketNum = 0;
        g_timeoutTimestamp = timestamp;

        /* Avoid excessive network traffic. */
        LOGI("DhcpAckOrNakPacketHandle() receive DHCP_NAK 53, avoid excessive network traffic, need sleep!");
        sleep(NUMBER_THREE);
        return;
    }

    LOGI("DhcpAckOrNakPacketHandle() recv DHCP_ACK 53.");

    /* Parse received dhcp ack packet. */
    ParseDhcpAckPacket(packet, timestamp);

    /* ack packet success. */
    if (g_dhcp4State == DHCP_STATE_BOUND) {
        WriteLease(packet);
    }
}

static void DhcpResponseHandle(time_t timestamp)
{
    struct DhcpPacket packet;
    int getLen;
    uint8_t u8Message = 0;

    if (memset_s(&packet, sizeof(packet), 0, sizeof(packet)) != EOK) {
        LOGE("DhcpResponseHandle() memset_s packet failed!");
        return;
    }
    getLen = (g_socketMode == SOCKET_MODE_RAW) ? GetDhcpRawPacket(&packet, g_sockFd)
                                               : GetDhcpKernelPacket(&packet, g_sockFd);
    if (getLen < 0) {
        if ((getLen == SOCKET_OPT_ERROR) && (errno != EINTR)) {
            LOGE("DhcpResponseHandle() get packet read error, reopening socket!");
            /* Reopen g_sockFd. */
            SetSocketMode(g_socketMode);
        }
        LOGE("DhcpResponseHandle() get packet failed, error:%{public}d!", errno);
        if (g_dhcp4State == DHCP_STATE_INITREBOOT) {
            g_dhcp4State = DHCP_STATE_INIT;
            g_timeoutTimestamp = timestamp;
        }
        return;
    }
    LOGI("DhcpResponseHandle() get packet success, getLen:%{public}d.", getLen);

    /* Check packet data. */
    if (packet.xid != g_transID) {
        LOGW("DhcpResponseHandle() get xid:%{public}u and g_transID:%{public}u not same!", packet.xid, g_transID);
        return;
    }
    if (!GetDhcpOptionUint8(&packet, DHCP_MESSAGE_TYPE_OPTION, &u8Message)) {
        LOGE("DhcpResponseHandle() GetDhcpOptionUint8 DHCP_MESSAGE_TYPE_OPTION failed!");
        return;
    }

    switch (g_dhcp4State) {
        case DHCP_STATE_SELECTING:
            DhcpOfferPacketHandle(u8Message, &packet, timestamp);
            break;
        case DHCP_STATE_REQUESTING:
        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
        case DHCP_STATE_INITREBOOT:
        case DHCP_STATE_RENEWED:
            DhcpAckOrNakPacketHandle(u8Message, &packet, timestamp);
            break;
        case DHCP_STATE_BOUND:
        case DHCP_STATE_RELEASED:
            LOGW("DhcpResponseHandle() g_dhcp4State is BOUND or RELEASED, ignore all packets!");
            break;
        default:
            break;
    }
}

/* Receive signals. */
static void SignalReceiver(void)
{
    int signum;
    if (read(g_sigSockFds[0], &signum, sizeof(signum)) < 0) {
        LOGE("SignalReceiver() failed, g_sigSockFds[0]:%{public}d read error:%{public}d!", g_sigSockFds[0], errno);
        return;
    }

    switch (signum) {
        case SIGTERM:
            LOGW("SignalReceiver() SIGTERM!");
            SetSocketMode(SOCKET_MODE_INVALID);
            unlink(g_cltCnf->pidFile);
            unlink(g_cltCnf->resultFile);
            exit(EXIT_SUCCESS);
            break;
        case SIGUSR1:
            LOGW("SignalReceiver() SIGUSR1!");
            ExecDhcpRelease();
            break;
        case SIGUSR2:
            LOGW("SignalReceiver() SIGUSR2!");
            ExecDhcpRenew();
            break;
        default:
            break;
    }
}


/* Set dhcp ipv4 state. */
int SetIpv4State(int state)
{
    if (state < 0) {
        LOGE("SetIpv4State() failed, state:%{public}d!", state);
        return DHCP_OPT_FAILED;
    }

    g_dhcp4State = state;
    return DHCP_OPT_SUCCESS;
}

/* Init signal handle function. */
int InitSignalHandle(void)
{
    /* Create signal socket fd. */
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, g_sigSockFds) != 0) {
        LOGE("InitSignalHandle() failed, socketpair error str:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }

    /* Register signal handlers. */
    if (signal(SIGTERM, SignalHandler) == SIG_ERR) {
        LOGE("InitSignalHandle() failed, signal SIGTERM error str:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }

    if (signal(SIGUSR1, SignalHandler) == SIG_ERR) {
        LOGE("InitSignalHandle() failed, signal SIGUSR1 error str:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }

    if (signal(SIGUSR2, SignalHandler) == SIG_ERR) {
        LOGE("InitSignalHandle() failed, signal SIGUSR2 error str:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }

    return DHCP_OPT_SUCCESS;
}

extern bool PublishDhcpIpv4ResultEvent(const int code, const char *data, const char *ifname);
int PublishDhcpResultEvent(const char *ifname, const int code, struct DhcpResult *result)
{
    if (ifname == NULL) {
        LOGE("PublishDhcpResultEvent() failed, ifname == NULL!");
        return DHCP_OPT_FAILED;
    }
    if ((code != PUBLISH_CODE_SUCCESS) && (code != PUBLISH_CODE_FAILED)) {
        LOGE("PublishDhcpResultEvent() ifname:%{public}s failed, code:%{public}d error!", ifname, code);
        return DHCP_OPT_FAILED;
    }
    if ((code == PUBLISH_CODE_SUCCESS) && (result == NULL)) {
        LOGE("PublishDhcpResultEvent() ifname:%{public}s,code:%{public}d failed, result == NULL!", ifname, code);
        return DHCP_OPT_FAILED;
    }

    /* Data format - ipv4:ifname,time,cliIp,lease,servIp,subnet,dns1,dns2,router1,router2,vendor */
    int nBytes;
    uint32_t curTime = (uint32_t)time(NULL);
    char strData[STRING_MAX_LEN] = {0};
    if (code == PUBLISH_CODE_SUCCESS) {
        nBytes = snprintf_s(strData, STRING_MAX_LEN, STRING_MAX_LEN - 1, "ipv4:%s,%u,%s,%u,%s,%s,%s,%s,%s,%s,%s",
            ifname, curTime, result->strYiaddr, result->uOptLeasetime, result->strOptServerId, result->strOptSubnet,
            result->strOptDns1, result->strOptDns2, result->strOptRouter1, result->strOptRouter2, result->strOptVendor);
    } else {
        nBytes = snprintf_s(strData, STRING_MAX_LEN, STRING_MAX_LEN - 1, "ipv4:%s,%u,*,*,*,*,*,*,*,*,*",
            ifname, curTime);
    }
    if (nBytes < 0) {
        LOGE("PublishDhcpResultEvent() failed, snprintf_s %{public}s error:%{public}d!", ifname, errno);
        return DHCP_OPT_FAILED;
    }

    if (!PublishDhcpIpv4ResultEvent(code, strData, ifname)) {
        LOGE("PublishDhcpResultEvent() PublishDhcpIpv4ResultEvent %{public}d %{public}s failed!", code, strData);
        return DHCP_OPT_FAILED;
    }
    LOGI("PublishDhcpResultEvent() PublishDhcpIpv4ResultEvent %{public}d %{private}s success.", code, strData);
    return DHCP_OPT_SUCCESS;
}

int GetPacketHeaderInfo(struct DhcpPacket *packet, uint8_t type)
{
    if (packet == NULL) {
        LOGE("GetPacketHeaderInfo() failed, packet == NULL!");
        return DHCP_OPT_FAILED;
    }

    switch (type) {
        case DHCP_DISCOVER:
        case DHCP_REQUEST:
        case DHCP_DECLINE:
        case DHCP_RELEASE:
        case DHCP_INFORM:
            packet->op = BOOT_REQUEST;
            break;
        case DHCP_OFFER:
        case DHCP_ACK:
        case DHCP_NAK:
            packet->op = BOOT_REPLY;
            break;
        default:
            break;
    }
    packet->htype = ETHERNET_TYPE;
    packet->hlen = ETHERNET_LEN;
    packet->cookie = htonl(MAGIC_COOKIE);
    packet->options[0] = END_OPTION;
    AddOptValueToOpts(packet->options, DHCP_MESSAGE_TYPE_OPTION, type);

    return DHCP_OPT_SUCCESS;
}

int GetPacketCommonInfo(struct DhcpPacket *packet)
{
    if (packet == NULL) {
        LOGE("GetPacketCommonInfo() failed, packet == NULL!");
        return DHCP_OPT_FAILED;
    }

    /* Add packet client_cfg info. */
    if (memcpy_s(packet->chaddr, sizeof(packet->chaddr), g_cltCnf->ifaceMac, MAC_ADDR_LEN) != EOK) {
        LOGE("GetPacketCommonInfo() failed, memcpy_s error!");
        return DHCP_OPT_FAILED;
    }
    int nClientIdLen = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + g_cltCnf->pOptClientId[DHCP_OPT_LEN_INDEX];
    AddOptStrToOpts(packet->options, g_cltCnf->pOptClientId, nClientIdLen);

    /* Add packet vendor info, vendor format: pro-version. */
    char buf[VENDOR_MAX_LEN - DHCP_OPT_CODE_BYTES - DHCP_OPT_LEN_BYTES] = {0};
    unsigned char vendorId[VENDOR_MAX_LEN] = {0};
    unsigned char *pVendorId = vendorId;
    int nRes = snprintf_s(buf,
        VENDOR_MAX_LEN - DHCP_OPT_DATA_INDEX,
        VENDOR_MAX_LEN - DHCP_OPT_DATA_INDEX - 1,
        "%s-%s",
        DHCPC_NAME,
        DHCPC_VERSION);
    if (nRes < 0) {
        LOGE("GetPacketCommonInfo() failed, snprintf_s res:%{public}d error!", nRes);
        return DHCP_OPT_FAILED;
    }
    pVendorId[DHCP_OPT_CODE_INDEX] = VENDOR_SPECIFIC_INFO_OPTION;
    pVendorId[DHCP_OPT_LEN_INDEX] = strlen(buf);
    if (strncpy_s((char *)pVendorId + DHCP_OPT_DATA_INDEX, VENDOR_MAX_LEN - DHCP_OPT_DATA_INDEX, buf, strlen(buf)) !=
        EOK) {
        LOGE("GetPacketCommonInfo() failed, strncpy_s error!");
        return DHCP_OPT_FAILED;
    }
    if (strlen((char *)vendorId) > 0) {
        int nVendorIdLen = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pVendorId[DHCP_OPT_LEN_INDEX];
        AddOptStrToOpts(packet->options, vendorId, nVendorIdLen);
    }

    return DHCP_OPT_SUCCESS;
}

/* Broadcast dhcp discover packet, discover dhcp servers that can provide ip address. */
int DhcpDiscover(uint32_t transid, uint32_t requestip)
{
    LOGI("DhcpDiscover() enter, transid:%{public}u,requestip:%{private}u.", transid, requestip);

    struct DhcpPacket packet;
    if (memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) != EOK) {
        return -1;
    }

    /* Get packet header and common info. */
    if ((GetPacketHeaderInfo(&packet, DHCP_DISCOVER) != DHCP_OPT_SUCCESS) ||
        (GetPacketCommonInfo(&packet) != DHCP_OPT_SUCCESS)) {
        return -1;
    }

    /* Get packet not common info. */
    packet.xid = transid;
    if (requestip > 0) {
        AddOptValueToOpts(packet.options, REQUESTED_IP_ADDRESS_OPTION, requestip);
    }
    AddParamaterRequestList(&packet);

    /* Begin broadcast dhcp discover packet. */
    LOGI("DhcpDiscover() discover, begin broadcast discover packet...");
    return SendToDhcpPacket(&packet, INADDR_ANY, INADDR_BROADCAST, g_cltCnf->ifaceIndex, (uint8_t *)MAC_BCAST_ADDR);
}

/* Broadcast dhcp request packet, tell dhcp servers that which ip address to choose. */
int DhcpRequest(uint32_t transid, uint32_t reqip, uint32_t servip)
{
    LOGI("DhcpRequest() enter, transid:%{public}u,reqip:%{private}u.", transid, reqip);

    struct DhcpPacket packet;
    if (memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) != EOK) {
        return -1;
    }

    /* Get packet header and common info. */
    if ((GetPacketHeaderInfo(&packet, DHCP_REQUEST) != DHCP_OPT_SUCCESS) ||
        (GetPacketCommonInfo(&packet) != DHCP_OPT_SUCCESS)) {
        return -1;
    }

    /* Get packet not common info. */
    packet.xid = transid;
    AddOptValueToOpts(packet.options, REQUESTED_IP_ADDRESS_OPTION, reqip);
    AddOptValueToOpts(packet.options, SERVER_IDENTIFIER_OPTION, servip);
    AddParamaterRequestList(&packet);

    /* Begin broadcast dhcp request packet. */
    char *pReqIp = Ip4IntConToStr(reqip, false);
    if (pReqIp != NULL) {
        LOGI("DhcpRequest() broadcast req packet, reqip: host %{private}u->%{private}s.", ntohl(reqip), pReqIp);
        free(pReqIp);
        pReqIp = NULL;
    }
    char *pSerIp = Ip4IntConToStr(servip, false);
    if (pSerIp != NULL) {
        LOGI("DhcpRequest() broadcast req packet, servIp: host %{private}u->%{private}s.", ntohl(servip), pSerIp);
        free(pSerIp);
        pSerIp = NULL;
    }
    return SendToDhcpPacket(&packet, INADDR_ANY, INADDR_BROADCAST, g_cltCnf->ifaceIndex, (uint8_t *)MAC_BCAST_ADDR);
}

/* Unicast or broadcast dhcp request packet, request to extend the lease from the dhcp server. */
int DhcpRenew(uint32_t transid, uint32_t clientip, uint32_t serverip)
{
    LOGI("DhcpRenew() enter, transid:%{public}u,clientip:%{private}u.", transid, clientip);

    struct DhcpPacket packet;
    if (memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) != EOK) {
        return -1;
    }

    /* Get packet header and common info. */
    if ((GetPacketHeaderInfo(&packet, DHCP_REQUEST) != DHCP_OPT_SUCCESS) ||
        (GetPacketCommonInfo(&packet) != DHCP_OPT_SUCCESS)) {
        return -1;
    }

    /* Get packet not common info. */
    packet.xid = transid;
    packet.ciaddr = clientip;
    AddParamaterRequestList(&packet);

    /* Begin broadcast or unicast dhcp request packet. */
    if (serverip == 0) {
        LOGI("DhcpRenew() rebind, begin broadcast req packet");
        return SendToDhcpPacket(&packet, INADDR_ANY, INADDR_BROADCAST, g_cltCnf->ifaceIndex, (uint8_t *)MAC_BCAST_ADDR);
    }
    LOGI("DhcpRenew() renew, begin unicast request packet");
    return SendDhcpPacket(&packet, clientip, serverip);
}

/* Unicast dhcp release packet, releasing an ip address in Use from the dhcp server. */
int DhcpRelease(uint32_t clientip, uint32_t serverip)
{
    LOGI("DhcpRelease() enter, clientip:%{private}u.", clientip);

    struct DhcpPacket packet;
    if (memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) != EOK) {
        return -1;
    }

    /* Get packet header and common info. */
    if ((GetPacketHeaderInfo(&packet, DHCP_RELEASE) != DHCP_OPT_SUCCESS) ||
        (GetPacketCommonInfo(&packet) != DHCP_OPT_SUCCESS)) {
        return -1;
    }

    /* Get packet not common info. */
    packet.xid = GetTransId();
    AddOptValueToOpts(packet.options, REQUESTED_IP_ADDRESS_OPTION, clientip);
    AddOptValueToOpts(packet.options, SERVER_IDENTIFIER_OPTION, serverip);

    LOGI("DhcpRelease() release, begin unicast release packet, clientip:%{private}u, serverip:%{private}u", clientip,
        serverip);
    return SendDhcpPacket(&packet, clientip, serverip);
}

static void DhcpInit(void)
{
    g_cltCnf = GetDhcpClientCfg();

    /* Init dhcp ipv4 state. */
    g_dhcp4State = DHCP_STATE_INIT;
    SetSocketMode(SOCKET_MODE_RAW);

    InitSocketFd();

    time_t t = time(NULL);
    if (t == (time_t)-1) {
        return;
    }
    Reboot(t);
}

int StartIpv4(void)
{
    int nRet, nMaxFds;
    fd_set exceptfds;
    struct timeval timeout;
    time_t curTimestamp;

    DhcpInit();

    for (; ;) {
        if (g_cltCnf->timeoutExit) {
            LOGW("StartIpv4() send packet timed out, now break!");
            break;
        }

        FD_ZERO(&exceptfds);
        timeout.tv_sec = g_timeoutTimestamp - time(NULL);
        timeout.tv_usec = 0;

        InitSocketFd();

        if (g_sockFd >= 0) {
            FD_SET(g_sockFd, &exceptfds);
        }
        FD_SET(g_sigSockFds[0], &exceptfds);

        if (timeout.tv_sec <= 0) {
            LOGI("StartIpv4() already timed out, need send or resend packet...");
            nRet = 0;
        } else {
            LOGI("StartIpv4() waiting on select...");
            nMaxFds = (g_sigSockFds[0] > g_sockFd) ? g_sigSockFds[0] : g_sockFd;
            nRet = select(nMaxFds + 1, &exceptfds, NULL, NULL, &timeout);
        }
        if (nRet < 0) {
            if ((nRet == -1) && (errno == EINTR)) {
                LOGW("StartIpv4() select err:%{public}d, a signal was caught!", errno);
            } else {
                LOGE("StartIpv4() failed, select maxFds:%{public}d error:%{public}d!", nMaxFds, errno);
            }
            continue;
        }

        curTimestamp = time(NULL);
        if (nRet == 0) {
            DhcpRequestHandle(curTimestamp);
        } else if ((g_socketMode != SOCKET_MODE_INVALID) && FD_ISSET(g_sockFd, &exceptfds)) {
            DhcpResponseHandle(curTimestamp);
        } else if (FD_ISSET(g_sigSockFds[0], &exceptfds)) {
            SignalReceiver();
        } else {
            LOGW("StartIpv4() nRet:%{public}d, g_socketMode:%{public}d, continue select...", nRet, g_socketMode);
        }
    }
    return g_cltCnf->timeoutExit ? StopProcess(g_cltCnf->pidFile) : DHCP_OPT_SUCCESS;
}