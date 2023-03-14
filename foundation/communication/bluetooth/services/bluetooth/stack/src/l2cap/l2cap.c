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

#include "l2cap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "l2cap_cmn.h"
#include "l2cap_core.h"
#include "l2cap_inst.h"
#include "l2cap_le.h"

int L2CAP_ConnectReq(const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, uint16_t *lcid)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    L2capPsm *psm = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lpsm = 0x%04X, rpsm = 0x%04X", __FUNCTION__, __LINE__, lpsm, rpsm);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if ((addr == NULL) || (lcid == NULL)) {
        return BT_BAD_PARAM;
    }

    psm = L2capGetPsm(lpsm);
    if (psm == NULL) {
        return BT_BAD_PARAM;
    }

    conn = L2capGetConnection2(addr);
    if (conn == NULL) {
        conn = L2capNewConnection(addr, 0);
    }

    chan = L2capNewChannel(conn, lpsm, rpsm);
    *lcid = chan->lcid;

    if (conn->state == L2CAP_CONNECTION_IDLE) {
        conn->state = L2CAP_CONNECTION_CONNECTING;
        if (L2capConnectBdr(addr) != BT_NO_ERROR) {
            L2capDeleteConnection(conn);
            return BT_OPERATION_FAILED;
        }

        return BT_NO_ERROR;
    }

    if ((conn->state == L2CAP_CONNECTION_CONNECTING) || (conn->state == L2CAP_CONNECTION_DISCONNECTING)) {
        return BT_NO_ERROR;
    }

    if (conn->info.state == L2CAP_INFO_STATE_NONE) {
        L2capSendInformationReq(conn, L2CAP_INFORMATION_TYPE_EXTENDED_FEATURE);
    } else if (conn->info.state == L2CAP_INFO_STATE_DONE) {
        L2capSendConnectionReq(conn, chan);
    }

    return BT_NO_ERROR;
}

int L2CAP_ConnectRsp(uint16_t lcid, uint8_t id, uint16_t result, uint16_t status)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X, id = %{public}d, result = %{public}d, status = %{public}d",
        __FUNCTION__,
        __LINE__,
        lcid,
        id,
        result,
        status);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECT_IN_REQ) {
        return BT_BAD_STATUS;
    }

    L2capSendConnectionRsp(conn, chan, id, result, status);

    if ((result != L2CAP_CONNECTION_SUCCESSFUL) && (result != L2CAP_CONNECTION_PENDING)) {
        L2capDeleteChannel(conn, chan, 0);
    }

    return BT_NO_ERROR;
}

int L2CAP_ConfigReq(uint16_t lcid, const L2capConfigInfo *cfg)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X", __FUNCTION__, __LINE__, lcid);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (cfg == NULL) {
        return BT_BAD_PARAM;
    }

    if ((cfg->mtu < L2CAP_MIN_MTU) || (cfg->flushTimeout == 0)) {
        return BT_BAD_PARAM;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if ((chan->state != L2CAP_CHANNEL_CONFIGING) && (chan->state != L2CAP_CHANNEL_CONNECTED)) {
        return BT_BAD_STATUS;
    }

    chan->lcfg.mtu = cfg->mtu;
    chan->lcfg.flushTimeout = cfg->flushTimeout;
    chan->lcfg.fcs = cfg->fcs;
    chan->lcfg.rfc.mode = cfg->rfc.mode;

    if (chan->lcfg.rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        if (cfg->rfc.rxWindowSize != 0) {
            chan->lcfg.rfc.rxWindowSize = cfg->rfc.rxWindowSize;
        }

        if (cfg->rfc.maxTransmit != 0) {
            chan->lcfg.rfc.maxTransmit = cfg->rfc.maxTransmit;
        }

        if (chan->lcfg.rfc.mps > cfg->mtu) {
            chan->lcfg.rfc.mps = cfg->mtu;
        }
    } else if (chan->lcfg.rfc.mode == L2CAP_STREAM_MODE) {
        chan->lcfg.rfc.maxTransmit = 0;
        chan->lcfg.rfc.retransmissionTimeout = 0;
        chan->lcfg.rfc.monitorTimeout = 0;
        chan->lcfg.rfc.txWindowSize = 0;
        chan->lcfg.rfc.rxWindowSize = 0;

        if (chan->lcfg.rfc.mps > cfg->mtu) {
            chan->lcfg.rfc.mps = cfg->mtu;
        }
    }

    if (cfg->rfc.mode == L2CAP_BASIC_MODE) {
        chan->lcfg.fcs = 0x01;
    }

    L2capSendConfigurationReq(conn, chan);
    return BT_NO_ERROR;
}

int L2CAP_ConfigRsp(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, uint16_t result)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X, id = %{public}d, result = %{public}d", __FUNCTION__, __LINE__, lcid, id, result);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (cfg == NULL) {
        return BT_BAD_PARAM;
    }

    if (cfg->mtu < L2CAP_MIN_MTU) {
        return BT_BAD_PARAM;
    }

    if (cfg->flushTimeout == 0) {
        return BT_BAD_PARAM;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if ((chan->state != L2CAP_CHANNEL_CONFIGING) && (chan->state != L2CAP_CHANNEL_CONNECTED)) {
        return BT_BAD_STATUS;
    }

    if (cfg->rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        chan->lcfg.rfc.txWindowSize = chan->rcfg.rfc.rxWindowSize;
        chan->lcfg.rfc.retransmissionTimeout = L2CAP_DEFAULT_RETRANSMISSION_TIMEOUT;
        chan->lcfg.rfc.monitorTimeout = L2CAP_DEFAULT_MONITOR_TIMEOUT;

        if (chan->rcfg.rfc.mps > (L2capGetTxBufferSize() - L2CAP_SIZE_10)) {
            chan->rcfg.rfc.mps = (L2capGetTxBufferSize() - L2CAP_SIZE_10);
        }
    } else if (chan->lcfg.rfc.mode == L2CAP_STREAM_MODE) {
        chan->lcfg.rfc.maxTransmit = 0;
        chan->lcfg.rfc.retransmissionTimeout = 0;
        chan->lcfg.rfc.monitorTimeout = 0;
        chan->lcfg.rfc.txWindowSize = 0;
        chan->lcfg.rfc.rxWindowSize = 0;

        if (chan->rcfg.rfc.mps > (L2capGetTxBufferSize() - L2CAP_SIZE_10)) {
            chan->rcfg.rfc.mps = (L2capGetTxBufferSize() - L2CAP_SIZE_10);
        }
    }

    L2capSendConfigurationRsp(conn, chan, id, result, cfg);
    return BT_NO_ERROR;
}

int L2CAP_DisconnectionReq(uint16_t lcid)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X", __FUNCTION__, __LINE__, lcid);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if ((chan->state != L2CAP_CHANNEL_CONNECTED) && (chan->state != L2CAP_CHANNEL_CONFIGING)) {
        return BT_BAD_STATUS;
    }

    L2capSendDisconnectionReq(conn, chan);
    return BT_NO_ERROR;
}

int L2CAP_DisconnectionRsp(uint16_t lcid, uint8_t id)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X, id = %{public}d", __FUNCTION__, __LINE__, lcid, id);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_DISCONNECT_IN_REQ) {
        return BT_BAD_STATUS;
    }

    L2capSendDisconnectionRsp(conn, chan, id);
    L2capDeleteChannel(conn, chan, 0);
    return BT_NO_ERROR;
}

int L2CAP_LocalBusy(uint16_t lcid, uint8_t isBusy)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X, isBusy = %{public}d", __FUNCTION__, __LINE__, lcid, isBusy);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->lcfg.rfc.mode != L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECTED) {
        return BT_BAD_STATUS;
    }

    if (isBusy > 0) {
        isBusy = L2CAP_BUSY_LOCAL_BUSY;
    }

    if ((chan->erfc.busyState & L2CAP_BUSY_LOCAL_BUSY) == isBusy) {
        return BT_BAD_STATUS;
    }

    if (isBusy) {
        chan->erfc.busyState |= L2CAP_BUSY_LOCAL_BUSY;
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_OFF, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RNR);
    } else {
        chan->erfc.busyState &= (~L2CAP_BUSY_LOCAL_BUSY);
        chan->erfc.busyState |= L2CAP_BUSY_WAIT_F;
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_ON, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RR);

        chan->erfc.retryCount = 1;
        L2capErfcStartMonitorTimer(chan);
    }

    return BT_NO_ERROR;
}

int L2CAP_SendData(uint16_t lcid, Packet *pkt)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    uint16_t length;

    if (pkt == NULL) {
        return BT_BAD_PARAM;
    }

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X, pktLength = %{public}d", __FUNCTION__, __LINE__, lcid, PacketSize(pkt));

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    length = PacketSize(pkt);
    if (length > chan->rcfg.mtu) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECTED) {
        return BT_BAD_STATUS;
    }

    if (chan->lcfg.rfc.mode == L2CAP_BASIC_MODE) {
        Packet *tpkt = NULL;
        uint8_t *header = NULL;

        tpkt = PacketInheritMalloc(pkt, L2CAP_HEADER_LENGTH, 0);
        header = BufferPtr(PacketHead(tpkt));

        L2capCpuToLe16(header + 0, length);
        L2capCpuToLe16(header + L2CAP_OFFSET_2, chan->rcid);

        L2capSendPacket(conn->aclHandle, chan->lcfg.flushTimeout, tpkt);
    } else {
        L2capSendIFrame(conn, chan, pkt);
    }

    return BT_NO_ERROR;
}

int L2CAP_RegisterEcho(const L2capEcho *echoCallback, void *context)
{
    L2capInstance *inst = NULL;

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (echoCallback == NULL) {
        return BT_BAD_PARAM;
    }

    inst = L2capGetInstance();
    inst->echo.ctx = context;
    (void)memcpy_s(&(inst->echo.cb), sizeof(L2capEcho), echoCallback, sizeof(L2capEcho));

    return BT_NO_ERROR;
}

int L2CAP_DeregisterEcho()
{
    L2capInstance *inst = NULL;

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    inst = L2capGetInstance();
    (void)memset_s(&(inst->echo), sizeof(L2capEcho), 0, sizeof(L2capEcho));
    return BT_NO_ERROR;
}

int L2CAP_EchoReq(uint16_t aclHandle, const uint8_t *data, uint16_t dataLen)
{
    L2capConnection *conn = NULL;

    LOG_INFO("%{public}s:%{public}d enter, aclHandle = %{public}d, dataLen = %{public}d", __FUNCTION__, __LINE__, aclHandle, dataLen);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if ((data == NULL) && (dataLen > 0)) {
        return BT_BAD_PARAM;
    }

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return BT_BAD_PARAM;
    }

    L2capSendEchoReq(conn, data, dataLen);
    return BT_NO_ERROR;
}

int L2CAP_EchoRsp(uint16_t aclHandle, uint8_t id, const uint8_t *data, uint16_t dataLen)
{
    L2capConnection *conn = NULL;

    LOG_INFO("%{public}s:%{public}d enter, aclHandle = %{public}d, id = %{public}d, dataLen = %{public}d", __FUNCTION__, __LINE__, aclHandle, id, dataLen);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if ((dataLen > 0) && (data == NULL)) {
        return BT_BAD_PARAM;
    }

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return BT_BAD_PARAM;
    }

    L2capSendEchoRsp(conn, id, data, dataLen);
    return BT_NO_ERROR;
}

int L2CAP_RegisterService(uint16_t lpsm, const L2capService *svc, void *context)
{
    L2capInstance *inst = NULL;
    L2capPsm *psm = NULL;

    LOG_INFO("%{public}s:%{public}d enter, psm = 0x%04X", __FUNCTION__, __LINE__, lpsm);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (svc == NULL) {
        return BT_BAD_PARAM;
    }

    // check whether the psm is valid
    if (!(lpsm & 0x0001) || (lpsm & 0x0100)) {
        return BT_BAD_PARAM;
    }

    psm = L2capGetPsm(lpsm);
    if (psm != NULL) {
        return BT_BAD_STATUS;
    }

    psm = L2capAlloc(sizeof(L2capPsm));
    if (psm == NULL) {
        return BT_NO_MEMORY;
    }

    inst = L2capGetInstance();

    psm->lpsm = lpsm;
    psm->ctx = context;
    (void)memcpy_s(&(psm->service), sizeof(L2capService), svc, sizeof(L2capService));
    ListAddFirst(inst->psmList, psm);

    return BT_NO_ERROR;
}

int L2CAP_DeregisterService(uint16_t lpsm)
{
    L2capInstance *inst = NULL;
    L2capPsm *psm = NULL;
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    ListNode *node = NULL;
    ListNode *nodeChan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, psm = 0x%04X", __FUNCTION__, __LINE__, lpsm);

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    psm = L2capGetPsm(lpsm);
    if (psm == NULL) {
        return BT_BAD_PARAM;
    }

    inst = L2capGetInstance();
    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        nodeChan = ListGetFirstNode(conn->chanList);
        while (nodeChan != NULL) {
            chan = ListGetNodeData(nodeChan);
            // if any channel used the psm, return error
            if (chan->lpsm == lpsm) {
                return BT_BAD_STATUS;
            }

            nodeChan = ListGetNextNode(nodeChan);
        }

        node = ListGetNextNode(node);
    }

    ListRemoveNode(inst->psmList, psm);
    L2capFree(psm);
    return BT_NO_ERROR;
}

void L2CAP_Initialize(int traceLevel)
{
    L2capInstance *inst = NULL;
    L2capBdrCallback cmnCallback = {0};

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    cmnCallback.aclConnected = L2capConnectComplete;
    cmnCallback.aclDisconnected = L2capDisconnectComplete;
    cmnCallback.recvL2capPacket = L2capReceivePacket;
    L2capRegisterBdr(&cmnCallback);

    inst = L2capGetInstance();
    inst->psmList = ListCreate(NULL);
    inst->connList = ListCreate(NULL);
    inst->nextLcid = L2CAP_MIN_CID;
    (void)memset_s(&(inst->echo), sizeof(L2capEcho), 0, sizeof(L2capEcho));

    L2CAP_LeInitialize(traceLevel);

    L2capCommonStartup();
    return;
}

void L2CAP_Finalize()
{
    L2capInstance *inst = NULL;
    ListNode *node = NULL;
    L2capConnection *conn = NULL;
    L2capPsm *psm = NULL;

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    L2capCommonShutdown();

    inst = L2capGetInstance();
    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        L2capDeleteConnection(conn);

        node = ListGetFirstNode(inst->connList);
    }

    ListDelete(inst->connList);
    inst->connList = NULL;

    node = ListGetFirstNode(inst->psmList);
    while (node != NULL) {
        psm = ListGetNodeData(node);
        ListRemoveNode(inst->psmList, psm);
        L2capFree(psm);

        node = ListGetFirstNode(inst->psmList);
    }

    ListDelete(inst->psmList);
    inst->psmList = NULL;

    (void)memset_s(&(inst->echo), sizeof(L2capEcho), 0, sizeof(L2capEcho));

    L2CAP_LeFinalize();
    return;
}
