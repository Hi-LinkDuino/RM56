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
#ifndef AVCTP_CTRL_L2CAP_H
#define AVCTP_CTRL_L2CAP_H
#include "avctp_int.h"
#include "l2cap_if.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConnectionInfo info;
    uint16_t lpsm;
    void *ctx;
} AvctRecvConnectionReqCallbackTskParam;

typedef struct {
    uint16_t lcid;
    L2capConnectionInfo info;
    uint16_t result;
    uint16_t status;
    void *ctx;
} AvctRecvConnectionRspCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo cfg;
    void *ctx;
} AvctRecvConfigReqCallbackTskParam;

typedef struct {
    uint16_t lcid;
    L2capConfigInfo cfg;
    uint16_t result;
    void *ctx;
} AvctRecvConfigRspCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void *ctx;
} AvctRecvDisconnectionReqCallbackTskParam;

typedef struct {
    uint16_t lcid;
    void *ctx;
} AvctRecvDisconnectionRspCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t reason;
    void *ctx;
} AvctDisconnectAbnormalCallbackTskParam;

typedef struct {
    uint16_t lcid;
    Packet *pkt;
    void *ctx;
} AvctRecvDataCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t isBusy;
    void *ctx;
} AvctRemoteBusyCallbackTskParam;

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    int result;
    void *context;
} AvctCtrlL2CIFConnectReqCallbackTskParam;

/* Connection Request packets received */
void AvctRecvConnectionReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);
void AvctRecvConnectionReqCallbackTsk(void *context);
void AvctRecvConnectionReqCBack(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);
/* Connection Response packet received */
void AvctRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);
void AvctRecvConnectionRspCallbackTsk(void *context);
void AvctRecvConnectionRspCBack(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);
/* Configuration Request packet received */
void AvctRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);
void AvctRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);
void AvctRecvConfigReqCallbackTsk(void *context);
void AvctRecvConfigReqCBack(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);
/* Configuration Response packet received */
void AvctRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx);
void AvctRecvDisconnectionReqCallbackTsk(void *context);
void AvctRecvConfigRspCallbackTsk(void *context);
void AvctRecvConfigRspCBack(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);
/* Disconnection Request packet received */
void AvctRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx);
void AvctRecvDisconnectionReqCallbackTsk(void *context);
void AvctRecvDisconnectionReqCBack(uint16_t lcid, uint8_t id, void *ctx);
/* Disconnection Response packet received */
void AvctRecvDisconnectionRspCallback(uint16_t lcid, void *ctx);
void AvctRecvDisconnectionRspCallbackTsk(void *context);
void AvctRecvDisconnectionRspCBack(uint16_t lcid, void *ctx);
/* Disconnected abnormal, such as acl disconnected or link loss */
void AvctDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);
void AvctDisconnectAbnormalCallbackTsk(void *context);
void AvctDisconnectAbnormalCBack(uint16_t lcid, uint8_t reason, void *ctx);
/* L2cap data packet received */
void AvctRecvDataCallback(uint16_t lcid, Packet *pkt, void *ctx);
void AvctRecvDataCallbackTsk(void *context);
void AvctRecvDataCBack(uint16_t lcid, Packet *pkt, void *ctx);
/* In Enhanced Retransmission mode, when RNR received or tx_window overflow */
void AvctRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx);
void AvctRemoteBusyCallbackTsk(void *context);
void AvctRemoteBusyCBack(uint16_t lcid, uint8_t isBusy, void *ctx);

void AvctCtrlL2CIFConnectReqCallbackTsk(void *context);
void AvctCtrlConnectResult(const BtAddr *addr, uint16_t lcid, int result, void *context);
extern void AvctRecvConnectionReqAct(const BtAddr *addr, uint16_t aclHandle, uint16_t lcid, uint8_t id, uint16_t psm);
extern void AvctCtrlL2CIFConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);

extern const L2capService G_AVCT_L2C_SVR;

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_CTRL_L2CAP_H */