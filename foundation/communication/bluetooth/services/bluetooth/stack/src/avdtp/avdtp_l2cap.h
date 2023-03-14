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

#ifndef AVDTP_L2CAP_H
#define AVDTP_L2CAP_H

#include "avdtp_int.h"
#include "gap_if.h"
#include "l2cap_if.h"

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConnectionInfo info;
    uint16_t psm;
    void *ctx;
} AvdtL2capConnectIndCallbackTskParam;

typedef struct {
    uint16_t lcid;
    L2capConnectionInfo info;
    uint16_t result;
    uint16_t status;
    void *ctx;
} AvdtL2capConnectCfmCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo cfg;
    void *ctx;
} AvdtL2capConfigIndCallbackTskParam;

typedef struct {
    uint16_t lcid;
    L2capConfigInfo cfg;
    uint16_t result;
    void *ctx;
} AvdtL2capConfigCfmCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void *ctx;
} AvdtL2capDisconnectIndCallbackTskParam;

typedef struct {
    uint16_t lcid;
    void *ctx;
} AvdtL2capDisconnectCfmCallbackTskParam;

typedef struct {
    uint16_t lcid;
    Packet *packet;
    void *ctx;
} AvdtL2capReadDataIndCallbackTskParam;

typedef struct {
    uint16_t lcid;
    uint8_t reason;
    void *ctx;
} AvdtL2capDisconnectAbnormalCallbackTskParam;

typedef struct {
    uint16_t result;
    GapServiceSecurityInfo security;
    void *context;
} AvdtRevSecurityCheckTskParam;
/**
 * Callback function declaration
 */
void AVDT_L2capConnectIndCallback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t psm, void *ctx);
void AvdtL2capConnectIndCallbackTsk(void *context);
void AvdtL2capConnectIndCallback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t psm, void *ctx);
void AVDT_L2capConnectCfmCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);
void AvdtL2capConnectCfmCallbackTsk(void *context);
void AvdtL2capConnectCfmCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);
void AVDT_L2capConfigIndCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);
void AvdtL2capConfigIndCallbackTsk(void *context);
void AvdtL2capConfigIndCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);
void AVDT_L2capConfigCfmCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);
void AvdtL2capConfigCfmCallbackTsk(void *context);
void AvdtL2capConfigCfmCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);
void AVDT_L2capDisconnectIndCallback(uint16_t lcid, uint8_t id, void *ctx);
void AvdtL2capDisconnectIndCallbackTsk(void *context);
void AvdtL2capDisconnectIndCallback(uint16_t lcid, uint8_t id, void *ctx);
void AVDT_L2capDisconnectCfmCallback(uint16_t lcid, void *ctx);
void AvdtL2capDisconnectCfmCallbackTsk(void *context);
void AvdtL2capDisconnectCfmCallback(uint16_t lcid, void *ctx);
void AVDT_L2capDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);
void AvdtL2capDisconnectAbnormalCallbackTsk(void *context);
void AvdtL2capDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);
void AVDT_L2capReadDataIndCallback(uint16_t lcid, Packet *packet, void *ctx);
void AvdtL2capReadDataIndCallbackTsk(void *context);
void AvdtL2capReadDataIndCallback(uint16_t lcid, Packet *packet, void *ctx);
void AVDT_L2capRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx);
void AvdtRevSecurityCheckCallback(uint16_t result, GapServiceSecurityInfo security, void *context);
void AvdtRevSecurityCheckTsk(void *context);

/**
 * Inner function declaration
 */
void AvdtRevSecurityCheck(uint16_t result, GapServiceSecurityInfo security, void *context);
void AvdtConfigureConfirm(AvdtTransChannel *transTable);
void AvdtConfigureIndication(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, AvdtTransChannel *transTable);
void AvdtConnectConfirmError(const AvdtTransChannel *transTable);
void AvdtConnectSignalingIndication(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info);
void AvdtConnectStreamIndication(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, AvdtSigCtrl *sigCtrl);
void AvdtStreamDataProc(AvdtStreamCtrl *streamCtrl, Packet *packet);
void AvdtConfigComplete(AvdtTransChannel *transTable);
void AvdtL2capDisconnectAbnormalSignle(const AvdtTransChannel *transTable, uint8_t reason);
void AvdtL2capDisconnectAbnormalStream(const AvdtTransChannel *transTable, uint8_t reason);

extern const L2capService G_AVDT_L2C_APPL;

#endif /* AVDTP_L2CAP_H */