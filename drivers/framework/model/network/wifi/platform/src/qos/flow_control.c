/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "flow_control.h"

#include "flow_control_task.h"
#include "securec.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "net_device.h"
#include "net_device_adapter.h"

#define HDF_LOG_TAG "WiFiFlowControl"
#define ETHER_TYPE_PROCESS_COUNT 8
#define TOS_TO_ID_COUNT 6
#define PROTOCOL_STANDARD_SHIFT_COUNT 2
static FlowControlQueueID g_tosToIdHash[TOS_TO_ID_COUNT] = {
    BE_QUEUE_ID, BK_QUEUE_ID, BK_QUEUE_ID, BE_QUEUE_ID, VI_QUEUE_ID, VI_QUEUE_ID
};

static FlowControlQueueID IpProcessFunc(const void *buff, uint32_t len);
static struct EtherProcessMap g_etherTypeProMap[ETHER_TYPE_PROCESS_COUNT] = {
    {ETHER_TYPE_IP, IpProcessFunc},
    {ETHER_TYPE_IPV6, NULL},
    {ETHER_TYPE_PAE, NULL},
    {ETHER_TYPE_TDLS, NULL},
    {ETHER_TYPE_PPP_DISC, NULL},
    {ETHER_TYPE_PPP_SES, NULL},
    {ETHER_TYPE_WAI, NULL},
    {ETHER_TYPE_VLAN, NULL},
};

static FlowControlQueueID TosToFcQueueId(uint8_t priority)
{
    if (priority >= TOS_TO_ID_COUNT) {
        return VO_QUEUE_ID;
    }
    return g_tosToIdHash[priority];
}

static bool IsDhcpPort(struct UdpHeader *udpHdr, uint32_t len)
{
    if (udpHdr == NULL) {
        HDF_LOGE("%s udpHdr:  = null!", __func__);
        return false;
    }
    if (len < sizeof(struct UdpHeader)) {
        HDF_LOGE("%s len not right!", __func__);
        return false;
    }
    if ((ntohs(udpHdr->dest) == DHCP_UDP_SRC_PORT) || (ntohs(udpHdr->dest) == DHCP_UDP_DES_PORT)) {
        return true;
    }
    return false;
}

/* TcpKeyFrame:ACK/SYN */
static bool IsTcpKeyFrame(struct IpHeader *ipHeader, uint32_t len)
{
    struct TcpHeader *tcpHdr = NULL;
    uint32_t ipPktLen;
    uint32_t tcpHdrLen;
    uint8_t ipHdrLen;
    uint8_t *ip = NULL;
    if (ipHeader == NULL) {
        HDF_LOGE("IsTcpAck Fail ipHeader = null");
        return false;
    }
    if (len < sizeof(struct IpHeader)) {
        HDF_LOGE("%s len not right!", __func__);
        return false;
    }
    if ((len - sizeof(struct IpHeader)) < sizeof(struct TcpHeader)) {
        HDF_LOGE("%s tcp hdr len not right!", __func__);
        return false;
    }

    tcpHdr = (struct TcpHeader *)(ipHeader + 1);
    ip = (uint8_t *)(ipHeader);
    ipPktLen = ntohs(ipHeader->totLen);
    ipHdrLen = ((*ip) & 0x0F) << PROTOCOL_STANDARD_SHIFT_COUNT;
    tcpHdrLen = (tcpHdr->offset & 0xF0) << PROTOCOL_STANDARD_SHIFT_COUNT;
    if (tcpHdrLen + ipHdrLen == ipPktLen) {
        return true;
    }
    return false;
}

static FlowControlQueueID IpProcessFunc(const void *buff, uint32_t len)
{
    struct IpHeader *ipHeader = NULL;
    struct UdpHeader *udpHdr = NULL;
    FlowControlQueueID id = NORMAL_QUEUE_ID;
    if (buff == NULL) {
        HDF_LOGE("%s fail: buff = null!", __func__);
        return QUEUE_ID_COUNT;
    }
    if (len < sizeof(struct IpHeader)) {
        HDF_LOGE("%s fail: IpHeader len not right!", __func__);
        return QUEUE_ID_COUNT;
    }
    ipHeader = (struct IpHeader *)buff;
    if (ipHeader->protocol == UDP_PROTOCOL) {
        /* from IP TOS find priority */
        /*
         * ----------------------------------------------------------------------
         *     tos define
         *  ----------------------------------------------------------------------
         * | bit7~bit5 | bit4  |  bit3      |  bit2       |   bit1            | bit0    |
         * | priority  | Delay | Throughput | reliability | Transmission cost | reserve |
         *  ----------------------------------------------------------------------
         */
        id = TosToFcQueueId(ipHeader->tos >> IP_PRI_SHIFT);
        if ((len - sizeof(struct IpHeader)) < sizeof(struct UdpHeader)) {
            HDF_LOGE("%s fail: UdpHeader len not right!", __func__);
            return QUEUE_ID_COUNT;
        }
        udpHdr = (struct UdpHeader *)(ipHeader + 1);
        if (((ntohs(ipHeader->fragInfo) & 0x1FFF) == 0) && IsDhcpPort(udpHdr, len - sizeof(struct IpHeader))) {
            id = VIP_QUEUE_ID;
        }
    } else if (ipHeader->protocol == TCP_PROTOCOL) {
        if (IsTcpKeyFrame(ipHeader, len)) {
            id = TCP_ACK_QUEUE_ID;
        } else {
            id = TCP_DATA_QUEUE_ID;
        }
    }
    return id;
}

static void FlowControlQueueInit(struct FlowControlModule *fcm)
{
    int32_t i, j;
    if (fcm == NULL) {
        HDF_LOGE("%s fail: fcm = null!", __func__);
        return;
    }
    for (i = 0; i < FLOW_DIR_COUNT; i++) {
        for (j = 0; j < QUEUE_ID_COUNT; j++) {
            NetBufQueueInit(&fcm->fcmQueue[i].queues[j].dataQueue);
            OsalSpinInit(&fcm->fcmQueue[i].queues[j].lock);
        }
    }
}

static void FlowControlQueueDeinit(struct FlowControlModule *fcm)
{
    int32_t i, j;
    if (fcm == NULL) {
        HDF_LOGE("%s fcm already free!", __func__);
        return;
    }

    for (i = 0; i < FLOW_DIR_COUNT; i++) {
        for (j = 0; j < QUEUE_ID_COUNT; j++) {
            NetBufQueueClear(&fcm->fcmQueue[i].queues[j].dataQueue);
            OsalSpinDestroy(&fcm->fcmQueue[i].queues[j].lock);
        }
    }
    return;
}

static int32_t SchedTransfer(struct FlowControlModule *fcm, FlowDir dir)
{
    if (fcm == NULL || dir >= FLOW_DIR_COUNT) {
        HDF_LOGE("%s : fcm = null dir=%d!", __func__, dir);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalSemPost(&fcm->sem[dir]);
    return HDF_SUCCESS;
}

static int32_t RegisterFlowControlOp(struct FlowControlModule *fcm, struct FlowControlOp *op)
{
    if ((fcm == NULL) || (op == NULL)) {
        HDF_LOGE("%s fail : fcm = null or op = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    fcm->op = op;
    return HDF_SUCCESS;
}

static bool IsValidSentToFCMPra(const struct FlowControlModule *fcm, uint32_t id, uint32_t dir)
{
    if (fcm == NULL) {
        HDF_LOGE("%s fail : fcm = null or buff = null!", __func__);
        return false;
    }
    if (id >= QUEUE_ID_COUNT || dir >= FLOW_DIR_COUNT) {
        HDF_LOGE("%s fail : id or dir not right!", __func__);
        return false;
    }
    return true;
}

static int32_t SetQueueThreshold(struct FlowControlModule *fcm, uint32_t queueThreshold, uint32_t id, uint32_t dir)
{
    struct FlowControlQueue *fcmQueue = NULL;
    if (!IsValidSentToFCMPra(fcm, id, dir)) {
        HDF_LOGE("%s fail IsValidSentToFCMPra FALSE!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    fcmQueue = &fcm->fcmQueue[dir].queues[id];
    fcmQueue->queueThreshold = queueThreshold;
    return HDF_SUCCESS;
}

static void FcmQueuePreProcess(struct FlowControlQueue *fcmQueue)
{
    NetBufQueue *dataQ = &fcmQueue->dataQueue;
    NetBuf *oldBuff = NULL;
    uint32_t threshold = fcmQueue->queueThreshold;
    uint32_t qLen = NetBufQueueSize(dataQ);
    if (threshold > 0 && threshold < qLen) {
        HDF_LOGE("%s abandon netbuff!", __func__);
        oldBuff = NetBufQueueDequeue(dataQ);
        if (oldBuff != NULL) {
            NetBufFree(oldBuff);
        }
    }
    return;
}

static int32_t SendBuffToFCM(struct FlowControlModule *fcm, NetBuf *buff, uint32_t id, uint32_t dir)
{
    struct FlowControlQueue *fcmQueue = NULL;
    NetBufQueue *dataQ = NULL;
    if ((buff == NULL) || (NetBufGetDataLen(buff) == 0)) {
        HDF_LOGE("%s fail : buff=null or len=0!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!IsValidSentToFCMPra(fcm, id, dir)) {
        HDF_LOGE("%s fail : IsValidSentToFCMPra FALSE!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    fcmQueue = &fcm->fcmQueue[dir].queues[id];
    dataQ = &fcmQueue->dataQueue;
    FcmQueuePreProcess(fcmQueue);
    if (NetBufQueueIsEmpty(dataQ)) {
        fcm->fcmQueue[dir].queues[id].pktCount = 0;
    }
    NetBufQueueEnqueue(dataQ, buff);
    fcm->fcmQueue[dir].queues[id].pktCount++;
    return HDF_SUCCESS;
}

static FlowControlQueueID GetQueueIdByEtherBuff(const NetBuf *buff)
{
    uint32_t len;
    struct EtherHeader *header = NULL;
    int i;
    uint16_t etherType = 0;
    if (buff == NULL) {
        HDF_LOGE("%s fail : buff = null!", __func__);
        return QUEUE_ID_COUNT;
    }
    len = NetBufGetDataLen(buff);
    if (len < sizeof(struct EtherHeader)) {
        HDF_LOGE("%s fail : buff->data_len not right!", __func__);
        return QUEUE_ID_COUNT;
    }
    header = (struct EtherHeader *)NetBufGetAddress(buff, E_DATA_BUF);
    etherType = ntohs(header->etherType);

    /* if processFun = null  return VIP_QUEUE_ID */
    for (i = 0; i < ETHER_TYPE_PROCESS_COUNT; i++) {
        if (g_etherTypeProMap[i].etherType == etherType) {
            if (g_etherTypeProMap[i].processFun != NULL) {
                return g_etherTypeProMap[i].processFun((void *)(header + 1), len - sizeof(struct EtherHeader));
            }
            return VIP_QUEUE_ID;
        }
    }
    return NORMAL_QUEUE_ID;
}

static struct FlowControlInterface g_fcInterface = {
    .setQueueThreshold = SetQueueThreshold,
    .getQueueIdByEtherBuff = GetQueueIdByEtherBuff,
    .sendBuffToFCM = SendBuffToFCM,
    .schedFCM = SchedTransfer,
    .registerFlowControlOp = RegisterFlowControlOp,
};

static struct FlowControlModule *g_fcm = NULL;

int32_t SendFlowControlQueue(struct FlowControlModule *fcm, uint32_t id, uint32_t dir)
{
    NetBufQueue *q = NULL;
    int32_t fwPriorityId = 0;
    int32_t rxPriorityId = 0;
    if (!IsValidSentToFCMPra(fcm, id, dir)) {
        HDF_LOGE("%s fail : IsValidSentToFCMPra FALSE!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    q = &fcm->fcmQueue[dir].queues[id].dataQueue;
    if (NetBufQueueIsEmpty(q)) {
        return HDF_SUCCESS;
    }
    if (dir == FLOW_TX) {
        if (fcm->op != NULL && fcm->op->getTxPriorityId != NULL) {
            fwPriorityId = fcm->op->getTxPriorityId(id);
        }
        if (fcm->op != NULL && fcm->op->txDataPacket != NULL) {
            fcm->op->txDataPacket(q, fcm->fcmPriv, fwPriorityId);
        } else {
            HDF_LOGE("%s fail : fcm->op->txDataPacket = null!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    if (dir == FLOW_RX) {
        if (fcm->op != NULL && fcm->op->getRxPriorityId != NULL) {
            rxPriorityId = fcm->op->getRxPriorityId(id);
        }
        if (fcm->op != NULL && fcm->op->rxDataPacket != NULL) {
            fcm->op->rxDataPacket(q, fcm->fcmPriv, rxPriorityId);
        } else {
            HDF_LOGE("%s fail : fcm->op->txDataPacket = null!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}

struct FlowControlModule *InitFlowControl(void *fcmPriv)
{
    struct FlowControlModule *fcm = NULL;
    int i;
    if (g_fcm != NULL) {
        HDF_LOGE("%s already init", __func__);
        return g_fcm;
    }
    fcm = (struct FlowControlModule *)OsalMemCalloc(sizeof(struct FlowControlModule));
    if (fcm == NULL) {
        HDF_LOGE("%s fail: malloc fail!", __func__);
        return NULL;
    }
    (void)memset_s(fcm, sizeof(struct FlowControlModule), 0, sizeof(struct FlowControlModule));

    /* init queue */
    FlowControlQueueInit(fcm);

    /* init wait */
    for (i = 0; i < FLOW_DIR_COUNT; i++) {
        if (OsalSemInit(&fcm->sem[i], 0) != HDF_SUCCESS) {
            HDF_LOGE("%s init osalsem fail!", __func__);
            if (i == FLOW_RX) {
                OsalSemDestroy(&fcm->sem[FLOW_TX]);
            }
            OsalMemFree(fcm);
            return NULL;
        }
    }

    /* init task */
    if (CreateFlowControlTask(fcm) != HDF_SUCCESS) {
        HDF_LOGE("%s fail: CreateFlowControlTask fail!", __func__);
        for (i = 0; i < FLOW_DIR_COUNT; i++) {
            (void)OsalSemDestroy(&fcm->sem[i]);
        }
        OsalMemFree(fcm);
        return NULL;
    }

    /* register interface */
    fcm->interface = &g_fcInterface;
    fcm->fcmPriv = fcmPriv;
    g_fcm = fcm;
    return fcm;
}

void DeInitFlowControl(struct FlowControlModule *fcm)
{
    int i;
    if (fcm == NULL) {
        HDF_LOGE("%s fail : fcm already been free.", __func__);
        return;
    }

    /* 1:Destroy task. 2:Destroy osalwait. 3:free NetBuff. */
    DestroyFlowControlTask(fcm);
    for (i = 0; i < FLOW_DIR_COUNT; i++) {
        OsalSemDestroy(&fcm->sem[i]);
    }
    FlowControlQueueDeinit(fcm);
    OsalMemFree(fcm);
    g_fcm = NULL;
    return;
}

struct FlowControlModule *GetFlowControlModule()
{
    return g_fcm;
}
