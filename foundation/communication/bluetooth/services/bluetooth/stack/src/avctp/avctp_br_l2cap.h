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
#ifndef AVCTP_BR_L2CAP_H
#define AVCTP_BR_L2CAP_H
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
} AvctBrRecvConnectionReqCallbackTskParam;

typedef struct {
    uint16_t lcid;
    L2capConnectionInfo info;
    uint16_t result;
    uint16_t status;
    void *ctx;
} AvctBrRecvConnectionRspCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo cfg;
    void *ctx;
} AvctBrRecvConfigReqCallbackTskParam;

typedef struct {
    uint16_t lcid;
    L2capConfigInfo cfg;
    uint16_t result;
    void *ctx;
} AvctBrRecvConfigRspCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void *ctx;
} AvctBrRecvDisconnectionReqCallbackTskParam;

typedef struct {
    uint16_t lcid;
    void *ctx;
} AvctBrRecvDisconnectionRspCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t reason;
    void *ctx;
} AvctBrDisconnectAbnormalCallbackTskParam;

typedef struct {
    uint16_t lcid;
    Packet *pkt;
    void *ctx;
} AvctBrRecvDataCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t isBusy;
    void *ctx;
} AvctBrRemoteBusyCallbackTskParam;

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    int result;
    void *context;
} AvctBrwL2CIFConnectReqCallbackTskParam;

/* Connection Request packets received */
void AvctBrRecvConnectionReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);
void AvctBrRecvConnectionReqCallbackTsk(void *context);
void AvctBrRecvConnectionReqCBack(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);
/* Connection Response packet received */
void AvctBrRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);
void AvctBrRecvConnectionRspCallbackTsk(void *context);
void AvctBrRecvConnectionRspCBack(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);
/* Configuration Request packet received */
void AvctBrRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);
void AvctBrRecvConfigReqCallbackTsk(void *context);
void AvctBrRecvConfigReqCBack(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);
/* Configuration Response packet received */
void AvctBrRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);
void AvctBrRecvConfigRspCallbackTsk(void *context);
void AvctBrRecvConfigRspCBack(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);
/* Disconnection Request packet received */
void AvctBrRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx);
void AvctBrRecvDisconnectionReqCallbackTsk(void *context);
void AvctBrRecvDisconnectionReqCBack(uint16_t lcid, uint8_t id, void *ctx);
/* Disconnection Response packet received */
void AvctBrRecvDisconnectionRspCallback(uint16_t lcid, void *ctx);
void AvctBrRecvDisconnectionRspCallbackTsk(void *context);
void AvctBrRecvDisconnectionRspCBack(uint16_t lcid, void *ctx);
/* Disconnected abnormal, such as acl disconnected or link loss */
void AvctBrDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);
void AvctBrDisconnectAbnormalCallbackTsk(void *context);
void AvctBrDisconnectAbnormalCBack(uint16_t lcid, uint8_t reason, void *ctx);
/* L2cap data packet received */
void AvctBrRecvDataCallback(uint16_t lcid, const Packet *pkt, void *ctx);
void AvctBrRecvDataCallbackTsk(void *context);
void AvctBrRecvDataCBack(uint16_t lcid, Packet *pkt, void *ctx);
/* In Enhanced Retransmission mode, when RNR received or tx_window overflow */
void AvctBrRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx);
void AvctBrRemoteBusyCallbackTsk(void *context);
void AvctBrRemoteBusyCBack(uint16_t lcid, uint8_t isBusy, void *ctx);

extern void AvctBrRecvConnectionReqAct(const BtAddr *addr, uint16_t aclHandle, uint16_t lcid, uint8_t id, uint16_t psm);
extern void AvctBrwL2CIFConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);

void AvctBrwL2CIFConnectReqCallbackTsk(void *context);
void AvctBrwL2CIFConnectResult(const BtAddr *addr, uint16_t lcid, int result, void *context);

extern const L2capService G_AVCT_BR_L2C_SVR;

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_BR_L2CAP_H */