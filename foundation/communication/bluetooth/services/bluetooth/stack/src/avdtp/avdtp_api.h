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

#ifndef AVDTP_API_H
#define AVDTP_API_H

#include "avdtp_int.h"
#include "event.h"

typedef struct {
    const AvdtRegisterParam *reg;
    Event *event;
} AvdtRegisterTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint16_t *handle;
    uint16_t codecIndex;
    Event *event;
    uint16_t ret;
} AvdtCreateStreamTskParam;

typedef struct {
    AvdtStreamConfig *avdtStreamConfig;
    uint8_t number;
    Event *event;
    uint16_t ret;
} AvdtRegisterLocalSEPTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint8_t maxSeps;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtDiscoverReqTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint8_t acpSeid;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtGetCapReqTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint8_t transLabel;
    uint8_t errCode;
    AvdtSepConfig *sepCap;
    Event *event;
    uint16_t ret;
} AvdtGetCapRspTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint8_t acpSeid;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtGetAllCapReqTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint8_t transLabel;
    uint8_t errCode;
    AvdtSepConfig *sepCap;
    Event *event;
    uint16_t ret;
} AvdtGetAllCapRspTskParam;

typedef struct {
    uint16_t handle;
    uint8_t *transLabel;
    uint16_t delayValue;
    Event *event;
    uint16_t ret;
} AvdtDelayReqTskParam;

typedef struct {
    uint16_t handle;
    uint8_t transLabel;
    uint8_t errCode;
    Event *event;
    uint16_t ret;
} AvdtDelayRspTskParam;

typedef struct {
    uint16_t handle;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtOpenReqTskParam;

typedef struct {
    uint16_t handle;
    uint8_t seid;
    AvdtSepConfig *sepConfig;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtSetConfigReqTskParam;

typedef struct {
    uint16_t handle;
    uint8_t transLabel;
    AvdtCatetory category;
    Event *event;
    uint16_t ret;
} AvdtSetConfigRspTskParam;

typedef struct {
    uint16_t handle;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtGetConfigReqTskParam;

typedef struct {
    const uint16_t *handles;
    uint8_t numHandles;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtStartReqTskParam;

typedef struct {
    uint16_t firstHandle;
    uint8_t transLabel;
    uint16_t firstFailHandle;
    uint8_t errCode;
    Event *event;
    uint16_t ret;
} AvdtStartRspTskParam;

typedef struct {
    const uint16_t *handles;
    uint8_t numHandles;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtSuspendReqTskParam;

typedef struct {
    uint16_t firstHandle;
    uint8_t transLabel;
    uint16_t firstFailHandle;
    uint8_t errCode;
    Event *event;
    uint16_t ret;
} AvdtSuspendRspTskParam;

typedef struct {
    uint16_t handle;
    Event *event;
    uint16_t ret;
} AvdtCloseReqTskParam;

typedef struct {
    uint16_t handle;
    uint8_t transLabel;
    uint8_t errCode;
    Event *event;
    uint16_t ret;
} AvdtCloseRspTskParam;

typedef struct {
    uint16_t handle;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtAbortReqTskParam;

typedef struct {
    uint16_t handle;
    uint8_t transLabel;
    uint8_t errCode;
    Event *event;
    uint16_t ret;
} AvdtAbortRspTskParam;

typedef struct {
    uint16_t handle;
    AvdtSepConfig *cfg;
    uint8_t *transLabel;
    Event *event;
    uint16_t ret;
} AvdtReconfigReqTskParam;

typedef struct {
    uint16_t handle;
    uint8_t transLabel;
    AvdtCatetory category;
    Event *event;
    uint16_t ret;
} AvdtReconfigRspTskParam;

typedef struct {
    uint16_t handle;
    Packet *pkt;
    uint32_t timeStamp;
    uint8_t payloadType;
    uint16_t marker;
    Event *event;
    uint16_t ret;
} AvdtWriteReqTskParam;

typedef struct {
    const BtAddr *bdAddr;
    uint8_t role;
    Event *event;
    uint16_t ret;
} AvdtConnectReqTskParam;

typedef struct {
    const BtAddr *bdAddr;
    Event *event;
    uint16_t ret;
} AvdtDisconnectReqTskParam;

void AvdtRegisterTsk(void *context);
void AvdtCreateStreamTsk(void *context);
void AvdtRegisterLocalSEPTsk(void *context);
void AvdtDiscoverReqTsk(void *context);
void AvdtGetCapReqTsk(void *context);
void AvdtGetCapRspTsk(void *context);
void AvdtGetAllCapReqTsk(void *context);
void AvdtGetAllCapRspTsk(void *context);
void AvdtDelayReqTsk(void *context);
void AvdtDelayRspTsk(void *context);
void AvdtOpenReqTsk(void *context);
void AvdtSetConfigReqTsk(void *context);
void AvdtSetConfigRspTsk(void *context);
void AvdtGetConfigReqTsk(void *context);
void AvdtStartReqTsk(void *context);
void AvdtStartRspTsk(void *context);
void AvdtSuspendReqTsk(void *context);
void AvdtSuspendRspTsk(void *context);
void AvdtCloseReqTsk(void *context);
void AvdtCloseRspTsk(void *context);
void AvdtAbortReqTsk(void *context);
void AvdtAbortRspTsk(void *context);
void AvdtReconfigReqTsk(void *context);
void AvdtReconfigRspTsk(void *context);
void AvdtWriteReqTsk(void *context);
void AvdtConnectReqTsk(void *context);
void AvdtDisconnectReqTsk(void *context);

extern uint16_t AvdtCreateStream(const BtAddr *bdAddr, uint16_t *handle, uint16_t codecIndex);
extern uint16_t AvdtDiscoverReq(const BtAddr *bdAddr, uint8_t maxSeps, uint8_t *transLabel);
extern uint16_t AvdtGetCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel);
extern uint16_t AvdtGetCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap);
extern uint16_t AvdtGetAllCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel);
extern uint16_t AvdtGetAllCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap);
extern uint16_t AvdtDelayReq(uint16_t handle, uint8_t *transLabel, uint16_t delayValue);
extern uint16_t AvdtDelayRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);
extern uint16_t AvdtOpenReq(uint16_t handle, uint8_t *transLabel);
extern uint16_t AvdtSetConfigReq(uint16_t handle, uint8_t seid, AvdtSepConfig *sepConfig, uint8_t *transLabel);
extern uint16_t AvdtSetConfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category);
extern uint16_t AvdtGetConfigReq(uint16_t handle, uint8_t *transLabel);
extern uint16_t AvdtStartReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel);
extern uint16_t AvdtStartRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode);
extern uint16_t AvdtSuspendReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel);
extern uint16_t AvdtSuspendRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode);
extern uint16_t AvdtCloseReq(uint16_t handle);
extern uint16_t AvdtCloseRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);
extern uint16_t AvdtAbortReq(uint16_t handle, uint8_t *transLabel);
extern uint16_t AvdtAbortRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);
extern uint16_t AvdtReconfigReq(uint16_t handle, AvdtSepConfig *cfg, uint8_t *transLabel);
extern uint16_t AvdtReconfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category);
extern uint16_t AvdtWriteReq(uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t payloadType, uint16_t marker);
extern uint16_t AvdtConnectReq(const BtAddr *bdAddr, uint8_t role);
extern uint16_t AvdtDisconnectReq(const BtAddr *bdAddr);

#endif /* AVDTP_API_H */