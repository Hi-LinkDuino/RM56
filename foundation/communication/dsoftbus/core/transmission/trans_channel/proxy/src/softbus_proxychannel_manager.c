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
#include "softbus_proxychannel_manager.h"

#include <securec.h>
#include <string.h>

#include "bus_center_info_key.h"
#include "bus_center_manager.h"
#include "common_list.h"
#include "softbus_adapter_crypto.h"
#include "softbus_adapter_mem.h"
#include "softbus_adapter_thread.h"
#include "softbus_conn_interface.h"
#include "softbus_errcode.h"
#include "softbus_feature_config.h"
#include "softbus_log.h"
#include "softbus_proxychannel_callback.h"
#include "softbus_proxychannel_control.h"
#include "softbus_proxychannel_listener.h"
#include "softbus_proxychannel_message.h"
#include "softbus_proxychannel_session.h"
#include "softbus_proxychannel_transceiver.h"
#include "softbus_utils.h"
#include "trans_pending_pkt.h"

#define PROXY_CHANNEL_CONTROL_TIMEOUT 19
#define PROXY_CHANNEL_BT_IDLE_TIMEOUT 240 // 4min
#define PROXY_CHANNEL_IDLE_TIMEOUT 15 // 10800 = 3 hour
#define PROXY_CHANNEL_TCP_IDLE_TIMEOUT 43200 // tcp 24 hour

static SoftBusList *g_proxyChannelList = NULL;
static SoftBusMutex g_myIdLock;
static uint32_t g_authMaxByteBufSize;
static uint32_t g_authMaxMessageBufSize;

static int32_t MyIdIsValid(int16_t myId)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->myId == myId) {
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_ERR;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_OK;
}

static bool IsLittleEndianCPU(void)
{
#define CHECK_NUM 0x0100
#define CHECK_NUM_HIGH 0x01

    uint16_t num = CHECK_NUM;

    if (*((char *)&num) == CHECK_NUM_HIGH) {
        return false;
    }
    return true;
}

static uint16_t EndianSwap16(uint16_t num)
{
#define HIGH_MASK 0xFF00
#define LOW_MASK 0x00FF
#define ENDIAN_SHIFT 8

    if (!IsLittleEndianCPU()) {
        return num;
    }
    return (((num & HIGH_MASK) >> ENDIAN_SHIFT) | ((num & LOW_MASK) << ENDIAN_SHIFT));
}

int16_t TransProxyGetNewMyId(void)
{
#define MYID_MAX_NUM 100
    static uint16_t myId = 0;
    int32_t cnt = MYID_MAX_NUM;
    if (SoftBusMutexLock(&g_myIdLock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    while (cnt) {
        cnt--;
        myId++;
        uint16_t ret = myId % MYID_MAX_NUM + 1;
        ret = EndianSwap16(ret);
        if (MyIdIsValid((int16_t)ret) == SOFTBUS_OK) {
            SoftBusMutexUnlock(&g_myIdLock);
            return (int16_t)ret;
        }
    }
    SoftBusMutexUnlock(&g_myIdLock);
    return INVALID_CHANNEL_ID;
}

static int32_t ChanIsEqual(ProxyChannelInfo *a, ProxyChannelInfo *b)
{
    if ((a->myId == b->myId) &&
        (a->peerId == b->peerId) &&
        (strncmp(a->identity, b->identity, sizeof(a->identity)) == 0)) {
        return SOFTBUS_OK;
    }
    return SOFTBUS_ERR;
}

static int32_t ResetChanIsEqual(int status, ProxyChannelInfo *a, ProxyChannelInfo *b)
{
    if (status == PROXY_CHANNEL_STATUS_HANDSHAKEING) {
        if ((a->myId == b->myId) &&
            (strncmp(a->identity, b->identity, sizeof(a->identity)) == 0)) {
            return SOFTBUS_OK;
        }
    }

    if ((a->myId == b->myId) &&
        (a->peerId == b->peerId) &&
        (strncmp(a->identity, b->identity, sizeof(a->identity)) == 0)) {
        return SOFTBUS_OK;
    }
    return SOFTBUS_ERR;
}

static int32_t TransProxyUpdateAckInfo(ProxyChannelInfo *info)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "g_proxyChannelList or item is null");
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if ((item->myId == info->myId) && (strncmp(item->identity, info->identity, sizeof(item->identity)) == 0)) {
            item->peerId = info->peerId;
            item->status = PROXY_CHANNEL_STATUS_COMPLETED;
            item->timeout = 0;
            (void)memcpy_s(&(item->appInfo.peerData), sizeof(item->appInfo.peerData),
                           &(info->appInfo.peerData), sizeof(info->appInfo.peerData));
            (void)memcpy_s(info, sizeof(ProxyChannelInfo), item, sizeof(ProxyChannelInfo));
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

static int32_t TransProxyAddChanItem(ProxyChannelInfo *chan)
{
    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }
    ListAdd(&(g_proxyChannelList->list), &(chan->node));
    g_proxyChannelList->cnt++;
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_OK;
}

int32_t TransProxyGetChanByChanId(int32_t chanId, ProxyChannelInfo *chan)
{
    ProxyChannelInfo *item = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY_SAFE(item, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == chanId) {
            (void)memcpy_s(chan, sizeof(ProxyChannelInfo), item, sizeof(ProxyChannelInfo));
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

void TransProxyDelChanByReqId(int32_t reqId)
{
    ProxyChannelInfo *item = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (g_proxyChannelList == NULL) {
        return;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return;
    }

    LIST_FOR_EACH_ENTRY_SAFE(item, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if ((item->reqId == reqId) &&
            (item->status == PROXY_CHANNEL_STATUS_PYH_CONNECTING)) {
            ListDelete(&(item->node));
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "del item (%d)", item->channelId);
            TransProxyPostOpenFailMsgToLoop(item);
            g_proxyChannelList->cnt--;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return;
}

void TransProxyDelChanByChanId(int32_t chanlId)
{
    ProxyChannelInfo *item = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (g_proxyChannelList == NULL) {
        return;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return;
    }

    LIST_FOR_EACH_ENTRY_SAFE(item, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == chanlId) {
            ListDelete(&(item->node));
            SoftBusFree(item);
            g_proxyChannelList->cnt--;
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "del chan info!");
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return;
}

void TransProxyChanProcessByReqId(int32_t reqId, uint32_t connId)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->reqId == reqId && item->status == PROXY_CHANNEL_STATUS_PYH_CONNECTING) {
            item->status = PROXY_CHANNEL_STATUS_HANDSHAKEING;
            item->connId = connId;
            TransAddConnRefByConnId(connId);
            TransProxyPostHandshakeMsgToLoop(item->channelId);
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return;
}

void TransProxyDelByConnId(uint32_t connId)
{
    ProxyChannelInfo *removeNode = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (g_proxyChannelList == NULL) {
        return;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return;
    }

    LIST_FOR_EACH_ENTRY_SAFE(removeNode, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (removeNode->connId == connId) {
            if (removeNode->status == PROXY_CHANNEL_STATUS_HANDSHAKEING ||
                removeNode->status == PROXY_CHANNEL_STATUS_PYH_CONNECTING) {
                OnProxyChannelOpenFailed(removeNode->channelId, &(removeNode->appInfo));
            } else {
                OnProxyChannelClosed(removeNode->channelId, &(removeNode->appInfo));
            }
            ListDelete(&(removeNode->node));
            SoftBusFree(removeNode);
            g_proxyChannelList->cnt--;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return;
}

static int32_t TransProxyDelByChannelId(int32_t channelId, ProxyChannelInfo *channelInfo)
{
    ProxyChannelInfo *removeNode = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY_SAFE(removeNode, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (removeNode->channelId == channelId) {
            if (channelInfo != NULL) {
                (void)memcpy_s(channelInfo, sizeof(ProxyChannelInfo), removeNode, sizeof(ProxyChannelInfo));
            }
            ListDelete(&(removeNode->node));
            SoftBusFree(removeNode);
            g_proxyChannelList->cnt--;
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

static int32_t TransProxyResetChan(ProxyChannelInfo *chanInfo)
{
    ProxyChannelInfo *removeNode = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY_SAFE(removeNode, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (ResetChanIsEqual(removeNode->status, removeNode, chanInfo) == SOFTBUS_OK) {
            (void)memcpy_s(chanInfo, sizeof(ProxyChannelInfo), removeNode, sizeof(ProxyChannelInfo));
            ListDelete(&(removeNode->node));
            SoftBusFree(removeNode);
            g_proxyChannelList->cnt--;
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);

    return SOFTBUS_ERR;
}

static int32_t TransProxyGetRecvMsgChanInfo(int16_t myId, int16_t peerId, ProxyChannelInfo *chanInfo)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->myId == myId || item->peerId == peerId) {
            if (item->status == PROXY_CHANNEL_STATUS_COMPLETED) {
                item->timeout = 0;
            }
            (void)memcpy_s(chanInfo, sizeof(ProxyChannelInfo), item, sizeof(ProxyChannelInfo));
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

static int32_t TransProxyKeepAlvieChan(ProxyChannelInfo *chanInfo)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (ChanIsEqual(item, chanInfo) == SOFTBUS_OK) {
            if (item->status == PROXY_CHANNEL_STATUS_KEEPLIVEING || item->status == PROXY_CHANNEL_STATUS_COMPLETED) {
                item->timeout = 0;
                item->status = PROXY_CHANNEL_STATUS_COMPLETED;
            }
            (void)memcpy_s(chanInfo, sizeof(ProxyChannelInfo), item, sizeof(ProxyChannelInfo));
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

int32_t TransProxyGetSendMsgChanInfo(int32_t channelId, ProxyChannelInfo *chanInfo)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == channelId) {
            if (item->status == PROXY_CHANNEL_STATUS_COMPLETED) {
                item->timeout = 0;
            }
            (void)memcpy_s(chanInfo, sizeof(ProxyChannelInfo), item, sizeof(ProxyChannelInfo));
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

int32_t TransProxyGetNewChanSeq(int32_t channelId)
{
    ProxyChannelInfo *item = NULL;
    int32_t seq = 0;

    if (g_proxyChannelList == NULL) {
        return seq;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return seq;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == channelId) {
            seq = item->seq;
            item->seq++;
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return seq;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return seq;
}

int32_t TransProxySetChiperSide(int32_t channelId, int32_t side)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == channelId) {
            item->chiperSide = side;
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

int32_t TransProxyGetChiperSide(int32_t channelId, int32_t *side)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == channelId) {
            *side = item->chiperSide;
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

int32_t TransProxyGetSessionKeyByChanId(int32_t channelId, char *sessionKey, uint32_t sessionKeySize)
{
    ProxyChannelInfo *item = NULL;

    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return SOFTBUS_ERR;
    }

    LIST_FOR_EACH_ENTRY(item, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (item->channelId == channelId) {
            if (item->status == PROXY_CHANNEL_STATUS_COMPLETED) {
                item->timeout = 0;
            }
            if (memcpy_s(sessionKey, sessionKeySize, item->appInfo.sessionKey,
                sizeof(item->appInfo.sessionKey)) != EOK) {
                SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "memcpy_s fail!");
                (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
                return SOFTBUS_ERR;
            }
            (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    return SOFTBUS_ERR;
}

void TransProxyProcessHandshakeAckMsg(const ProxyMessage *msg)
{
    ProxyChannelInfo *info = SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        return;
    }

    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "recv ack msg");
    if (TransProxyUnpackHandshakeAckMsg(msg->data, info) != SOFTBUS_OK) {
        SoftBusFree(info);
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "UnpackHandshakeAckMsg fail");
        return;
    }

    info->myId = msg->msgHead.myId;
    info->peerId = msg->msgHead.peerId;
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO,
        "recv Handshake ack myid %d peerid %d identity %s", info->myId, info->peerId, info->identity);
    if (TransProxyUpdateAckInfo(info) != SOFTBUS_OK) {
        SoftBusFree(info);
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "UpdateAckInfo fail");
        return;
    }
    (void)OnProxyChannelOpened(info->channelId, &(info->appInfo), 0);
    SoftBusFree(info);
}
static int TransProxyGetLocalInfo(ProxyChannelInfo *chan)
{
    if (chan->appInfo.appType != APP_TYPE_INNER) {
        int32_t ret = TransProxyGetPkgName(chan->appInfo.myData.sessionName,
                                           chan->appInfo.myData.pkgName, sizeof(chan->appInfo.myData.pkgName));
        if (ret != SOFTBUS_OK) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "proc handshake get pkg name fail");
            return SOFTBUS_ERR;
        }
    }

    if (LnnGetLocalStrInfo(STRING_KEY_UUID, chan->appInfo.myData.deviceId,
                           sizeof(chan->appInfo.myData.deviceId)) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Handshake get local info fail");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

void TransProxyProcessHandshakeMsg(const ProxyMessage *msg)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO,
        "recv Handshake myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
    ProxyChannelInfo *chan = (ProxyChannelInfo *)SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (chan == NULL) {
        return;
    }

    if (TransProxyUnpackHandshakeMsg(msg->data, chan) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "UnpackHandshakeMsg fail");
        SoftBusFree(chan);
        return;
    }

    if (TransProxyGetLocalInfo(chan) != SOFTBUS_OK) {
        SoftBusFree(chan);
        return;
    }
    ConnectionInfo info = {0};
    if (ConnGetConnectionInfo(msg->connId, &info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "GetConnectionInfo fail connectionId %u", msg->connId);
        SoftBusFree(chan);
        return;
    }

    int16_t newChanId = TransProxyGetNewMyId();
    int32_t ret = OnProxyChannelOpened(newChanId, &(chan->appInfo), 1);
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "OnProxyChannelOpened  fail");
        SoftBusFree(chan);
        return;
    }

    chan->isServer = 1;
    chan->status = PROXY_CHANNEL_STATUS_COMPLETED;
    chan->connId = msg->connId;
    chan->myId = newChanId;
    chan->channelId = newChanId;
    chan->peerId = msg->msgHead.peerId;
    chan->chiperSide = msg->chiperSide;
    chan->type = info.type;
    if (TransProxyAddChanItem(chan) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "AddChanItem fail");
        OnProxyChannelClosed(newChanId, &(chan->appInfo));
        SoftBusFree(chan);
        return;
    }
    if (TransProxyAckHandshake(msg->connId, chan) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "AckHandshake fail");
        OnProxyChannelClosed(newChanId, &(chan->appInfo));
        TransProxyDelChanByChanId(newChanId);
        return;
    }
    TransCreateConnByConnId(msg->connId);
}

void TransProxyProcessResetMsg(const ProxyMessage *msg)
{
    ProxyChannelInfo *info = SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        return;
    }

    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO,
        "recv reset myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
    if (TransProxyUnpackIdentity(msg->data, info->identity, sizeof(info->identity)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "reset identity fail");
        SoftBusFree(info);
        return;
    }

    info->peerId = msg->msgHead.peerId;
    info->myId = msg->msgHead.myId;

    if (TransProxyResetChan(info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR,
            "reset chan fail myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
        SoftBusFree(info);
        return;
    }

    if (info->status == PROXY_CHANNEL_STATUS_HANDSHAKEING) {
        OnProxyChannelOpenFailed(info->channelId, &(info->appInfo));
    } else {
        OnProxyChannelClosed(info->channelId, &(info->appInfo));
    }
    if ((info->type == CONNECT_BR || info->type == CONNECT_BLE) &&
        info->status == PROXY_CHANNEL_STATUS_COMPLETED) {
        (void)TransProxyCloseConnChannelReset(msg->connId, false);
    } else {
        (void)TransProxyCloseConnChannel(msg->connId);
    }
    SoftBusFree(info);
}

void TransProxyProcessKeepAlive(const ProxyMessage *msg)
{
    ProxyChannelInfo *info = SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        return;
    }

    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO,
        "recv keepalive myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
    if (TransProxyUnpackIdentity(msg->data, info->identity, sizeof(info->identity)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "keep alive unpack identity fail");
        SoftBusFree(info);
        return;
    }
    info->peerId = msg->msgHead.peerId;
    info->myId = msg->msgHead.myId;

    if (TransProxyKeepAlvieChan(info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR,
            "reset keep alive proc fail myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
        SoftBusFree(info);
        return;
    }

    TransProxyAckKeepalive(info);
    SoftBusFree(info);
}

void TransProxyProcessKeepAliveAck(const ProxyMessage *msg)
{
    ProxyChannelInfo *info = SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        return;
    }

    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO,
        "recv keepalive ack myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
    if (TransProxyUnpackIdentity(msg->data, info->identity, sizeof(info->identity)) != SOFTBUS_OK) {
        SoftBusFree(info);
        return;
    }
    info->peerId = msg->msgHead.peerId;
    info->myId = msg->msgHead.myId;

    if (TransProxyKeepAlvieChan(info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR,
            "reset keep alive ack proc fail myid %d peerid %d", msg->msgHead.myId, msg->msgHead.peerId);
        SoftBusFree(info);
        return;
    }
    SoftBusFree(info);
}

void TransProxyProcessDataRecv(const ProxyMessage *msg)
{
    ProxyChannelInfo *info = (ProxyChannelInfo *)SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        return;
    }

    if (TransProxyGetRecvMsgChanInfo(msg->msgHead.myId, msg->msgHead.peerId, info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR,
            "data recv get info fail mid %d pid %d", msg->msgHead.myId, msg->msgHead.peerId);
        SoftBusFree(info);
        return;
    }

    OnProxyChannelMsgReceived(info->channelId, &(info->appInfo), msg->data, msg->dateLen);
    SoftBusFree(info);
}

void TransProxyonMessageReceived(const ProxyMessage *msg)
{
    switch (msg->msgHead.type) {
        case PROXYCHANNEL_MSG_TYPE_HANDSHAKE: {
            TransProxyProcessHandshakeMsg(msg);
            break;
        }
        case PROXYCHANNEL_MSG_TYPE_HANDSHAKE_ACK: {
            TransProxyProcessHandshakeAckMsg(msg);
            break;
        }
        case PROXYCHANNEL_MSG_TYPE_RESET: {
            TransProxyProcessResetMsg(msg);
            break;
        }
        case PROXYCHANNEL_MSG_TYPE_KEEPALIVE: {
            TransProxyProcessKeepAlive(msg);
            break;
        }
        case PROXYCHANNEL_MSG_TYPE_KEEPALIVE_ACK: {
            TransProxyProcessKeepAliveAck(msg);
            break;
        }
        case PROXYCHANNEL_MSG_TYPE_NORMAL: {
            TransProxyProcessDataRecv(msg);
            break;
        }
        default: {
            break;
        }
    }
}

int32_t TransProxyCreateChanInfo(ProxyChannelInfo *chan, int32_t channelId, const AppInfo *appInfo)
{
    chan->myId = channelId;
    chan->channelId = channelId;
    if (GenerateRandomStr(chan->identity, sizeof(chan->identity)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "GenerateRandomStr err");
        return SOFTBUS_ERR;
    }

    if (appInfo->appType != APP_TYPE_AUTH) {
        if (SoftBusGenerateRandomArray((unsigned char *)appInfo->sessionKey, sizeof(appInfo->sessionKey))
            != SOFTBUS_OK) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "GenerateRandomArray err");
            return SOFTBUS_ERR;
        }
    }

    (void)memcpy_s(&(chan->appInfo), sizeof(chan->appInfo), appInfo, sizeof(AppInfo));
    if (TransProxyAddChanItem(chan) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "AddChanItem fail");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

void TransProxyOpenProxyChannelSuccess(int32_t chanId)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "send handshake msg");
    ProxyChannelInfo *chan = (ProxyChannelInfo *)SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (chan == NULL) {
        return;
    }

    if (TransProxyGetChanByChanId(chanId, chan) != SOFTBUS_OK) {
        (void)TransProxyCloseConnChannel(chan->connId);
        SoftBusFree(chan);
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "disconnect device chanId %d", chanId);
        return;
    }

    if (TransProxyHandshake(chan) == SOFTBUS_ERR) {
        (void)TransProxyCloseConnChannel(chan->connId);
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "shake hand err");
        OnProxyChannelOpenFailed(chan->channelId, &(chan->appInfo));
        TransProxyDelChanByChanId(chanId);
    }
    SoftBusFree(chan);
    return;
}

void TransProxyOpenProxyChannelFail(int32_t channelId, const AppInfo *appInfo)
{
    (void)OnProxyChannelOpenFailed(channelId, appInfo);
}

int32_t TransProxyOpenProxyChannel(const AppInfo *appInfo, const ConnectOption *connInfo, int32_t *channelId)
{
    if (appInfo == NULL || connInfo == NULL || channelId == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "open normal channel: invalid para");
        return SOFTBUS_ERR;
    }

    return TransProxyOpenConnChannel(appInfo, connInfo, channelId);
}

static void TransProxyCloseProxyOtherRes(int32_t channelId, const ProxyChannelInfo *info)
{
    if (TransProxyDelSliceProcessorByChannelId(channelId) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "del channel err %d", channelId);
    }

    if (DelPendingPacket(channelId, PENDING_TYPE_PROXY) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "del pending pkt err %d", channelId);
    }

    uint32_t connId = info->connId;
    TransProxyPostResetPeerMsgToLoop(info);
    TransProxyPostDisConnectMsgToLoop(connId);
}

int32_t TransProxyCloseProxyChannel(int32_t channelId)
{
    ProxyChannelInfo *info = (ProxyChannelInfo *)SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        return SOFTBUS_MALLOC_ERR;
    }

    if (TransProxyDelByChannelId(channelId, info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "del channel err %d", channelId);
        SoftBusFree(info);
        return SOFTBUS_TRANS_PROXY_DEL_CHANNELID_INVALID;
    }

    TransProxyCloseProxyOtherRes(channelId, info);
    return SOFTBUS_OK;
}

int32_t TransProxySendMsg(int32_t channelId, const char *data, uint32_t dataLen, int32_t priority)
{
    int32_t ret;
    ProxyChannelInfo *info = (ProxyChannelInfo *)SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (info == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "malloc in trans proxy send message.id[%d]", channelId);
        return SOFTBUS_MALLOC_ERR;
    }

    if (TransProxyGetSendMsgChanInfo(channelId, info) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get channelId err %d", channelId);
        SoftBusFree(info);
        return SOFTBUS_TRANS_PROXY_SEND_CHANNELID_INVALID;
    }

    if (info->status != PROXY_CHANNEL_STATUS_COMPLETED && info->status != PROXY_CHANNEL_STATUS_KEEPLIVEING) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "status is err %d", info->status);
        SoftBusFree(info);
        return SOFTBUS_TRANS_PROXY_CHANNLE_STATUS_INVALID;
    }

    ret = TransProxySendMessage(info, (char *)data, dataLen, priority);
    SoftBusFree(info);
    return ret;
}
void TransProxyTimerItemProc(const ListNode *proxyProcList)
{
    ProxyChannelInfo *removeNode = NULL;
    ProxyChannelInfo *nextNode = NULL;
    uint32_t connId;

    LIST_FOR_EACH_ENTRY_SAFE(removeNode, nextNode, proxyProcList, ProxyChannelInfo, node) {
        ListDelete(&(removeNode->node));
        if (removeNode->status == PROXY_CHANNEL_STATUS_TIMEOUT) {
            connId = removeNode->connId;
            ProxyChannelInfo *resetMsg = SoftBusMalloc(sizeof(ProxyChannelInfo));
            if (resetMsg != NULL) {
                (void)memcpy_s(resetMsg, sizeof(ProxyChannelInfo), removeNode, sizeof(ProxyChannelInfo));
                TransProxyPostResetPeerMsgToLoop(resetMsg);
            }
            TransProxyPostOpenClosedMsgToLoop(removeNode);
            TransProxyPostDisConnectMsgToLoop(connId);
        }
        if (removeNode->status == PROXY_CHANNEL_STATUS_HANDSHAKE_TIMEOUT) {
            connId = removeNode->connId;
            TransProxyPostOpenFailMsgToLoop(removeNode);
            TransProxyPostDisConnectMsgToLoop(connId);
        }
        if (removeNode->status == PROXY_CHANNEL_STATUS_KEEPLIVEING) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "send keepalive channel %d ", removeNode->myId);
            TransProxyPostKeepAliveMsgToLoop(removeNode);
        }
    }
}

void TransProxyTimerProc(void)
{
    ProxyChannelInfo *removeNode = NULL;
    ProxyChannelInfo *nextNode = NULL;
    ListNode proxyProcList;

    if (g_proxyChannelList == 0 || g_proxyChannelList->cnt == 0) {
        return;
    }
    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return;
    }

    ListInit(&proxyProcList);
    LIST_FOR_EACH_ENTRY_SAFE(removeNode, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        removeNode->timeout++;
        if (removeNode->status == PROXY_CHANNEL_STATUS_HANDSHAKEING ||
            removeNode->status == PROXY_CHANNEL_STATUS_PYH_CONNECTING) {
            if (removeNode->timeout >= PROXY_CHANNEL_CONTROL_TIMEOUT) {
                removeNode->status = PROXY_CHANNEL_STATUS_HANDSHAKE_TIMEOUT;
                SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "channel (%d) handshake is timeout", removeNode->myId);
                ListDelete(&(removeNode->node));
                ListAdd(&proxyProcList, &(removeNode->node));
                g_proxyChannelList->cnt--;
            }
        }
        if (removeNode->status == PROXY_CHANNEL_STATUS_KEEPLIVEING) {
            if (removeNode->timeout >= PROXY_CHANNEL_CONTROL_TIMEOUT) {
                removeNode->status = PROXY_CHANNEL_STATUS_TIMEOUT;
                SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "channel (%d) keepalvie is timeout", removeNode->myId);
                ListDelete(&(removeNode->node));
                ListAdd(&proxyProcList, &(removeNode->node));
                g_proxyChannelList->cnt--;
            }
        }
        if (removeNode->status == PROXY_CHANNEL_STATUS_COMPLETED) {
            if (removeNode->timeout >= PROXY_CHANNEL_BT_IDLE_TIMEOUT) {
                removeNode->status = PROXY_CHANNEL_STATUS_TIMEOUT;
                ListDelete(&(removeNode->node));
                ListAdd(&proxyProcList, &(removeNode->node));
                SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "channel (%d) is idle", removeNode->myId);
                g_proxyChannelList->cnt--;
            }
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
    TransProxyTimerItemProc(&proxyProcList);
}

int32_t TransProxyAuthSessionDataLenCheck(uint32_t dataLen, int32_t type)
{
    switch (type) {
        case PROXY_FLAG_MESSAGE:
        case PROXY_FLAG_ASYNC_MESSAGE: {
            if (dataLen > g_authMaxMessageBufSize) {
                return SOFTBUS_ERR;
            }
            break;
        }
        case PROXY_FLAG_BYTES: {
            if (dataLen > g_authMaxByteBufSize) {
                return SOFTBUS_ERR;
            }
            break;
        }
        default: {
            return SOFTBUS_OK;
        }
    }
    return SOFTBUS_OK;
}

int32_t TransProxyManagerInit(const IServerChannelCallBack *cb)
{
    if (SoftBusMutexInit(&g_myIdLock, NULL) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "init lock failed");
        return SOFTBUS_ERR;
    }

    if (TransProxySetCallBack(cb) != SOFTBUS_OK) {
        return SOFTBUS_ERR;
    }

    if (TransProxyTransInit() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "TransProxyTransInit fail");
        return SOFTBUS_ERR;
    }

    g_proxyChannelList = CreateSoftBusList();
    if (g_proxyChannelList == NULL) {
        return SOFTBUS_ERR;
    }

    if (PendingInit(PENDING_TYPE_PROXY) == SOFTBUS_ERR) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans proxy pending init failed.");
        return SOFTBUS_ERR;
    }

    if (RegisterTimeoutCallback(SOFTBUS_PROXYCHANNEL_TIMER_FUN, TransProxyTimerProc) != SOFTBUS_OK) {
        DestroySoftBusList(g_proxyChannelList);
        return SOFTBUS_ERR;
    }

    if (TransSliceManagerInit() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_CONN, SOFTBUS_LOG_ERROR, "Trans slice manager init failed");
    }

    if (SoftbusGetConfig(SOFTBUS_INT_AUTH_MAX_BYTES_LENGTH,
        (unsigned char*)&g_authMaxByteBufSize, sizeof(g_authMaxByteBufSize)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_CONN, SOFTBUS_LOG_ERROR, "get auth proxy channel max bytes length fail");
    }

    if (SoftbusGetConfig(SOFTBUS_INT_AUTH_MAX_MESSAGE_LENGTH,
        (unsigned char*)&g_authMaxMessageBufSize, sizeof(g_authMaxMessageBufSize)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_CONN, SOFTBUS_LOG_ERROR, "get auth proxy channel max message length fail");
    }

    SoftBusLog(SOFTBUS_LOG_CONN, SOFTBUS_LOG_INFO, "proxy auth byteSize[%u], messageSize[%u]",
        g_authMaxByteBufSize, g_authMaxMessageBufSize);
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "proxy channel init ok");
    return SOFTBUS_OK;
}

int32_t TransProxyGetNameByChanId(int32_t chanId, char *pkgName, char *sessionName,
    uint16_t pkgLen, uint16_t sessionLen)
{
    if (pkgName == NULL || sessionName == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }
    ProxyChannelInfo *chan = (ProxyChannelInfo *)SoftBusCalloc(sizeof(ProxyChannelInfo));
    if (chan == NULL) {
        return SOFTBUS_MALLOC_ERR;
    }
    if (TransProxyGetChanByChanId(chanId, chan) != SOFTBUS_OK) {
        SoftBusFree(chan);
        return SOFTBUS_ERR;
    }
    if (TransProxyGetPkgName(chan->appInfo.myData.sessionName, pkgName, pkgLen) != SOFTBUS_OK) {
        SoftBusFree(chan);
        return SOFTBUS_ERR;
    }
    if (strcpy_s(sessionName, sessionLen, chan->appInfo.myData.sessionName) != EOK) {
        SoftBusFree(chan);
        return SOFTBUS_MEM_ERR;
    }
    SoftBusFree(chan);
    return SOFTBUS_OK;
}

void TransProxyManagerDeinit(void)
{
    TransSliceManagerDeInit();
    (void)RegisterTimeoutCallback(SOFTBUS_PROXYCHANNEL_TIMER_FUN, NULL);
    PendingDeinit(PENDING_TYPE_PROXY);
}

void TransProxyDeathCallback(const char *pkgName)
{
    if (g_proxyChannelList == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get proxy info error, info list is null.");
        return;
    }
    ProxyChannelInfo *item = NULL;
    ProxyChannelInfo *nextNode = NULL;

    if (SoftBusMutexLock(&g_proxyChannelList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock mutex fail!");
        return;
    }
    LIST_FOR_EACH_ENTRY_SAFE(item, nextNode, &g_proxyChannelList->list, ProxyChannelInfo, node) {
        if (strcmp(item->appInfo.myData.pkgName, pkgName) == 0) {
            TransProxyResetPeer(item);
            (void)TransProxyCloseConnChannel(item->connId);
            ListDelete(&(item->node));
            SoftBusFree(item);
            g_proxyChannelList->cnt--;
            continue;
        }
    }
    (void)SoftBusMutexUnlock(&g_proxyChannelList->lock);
}
