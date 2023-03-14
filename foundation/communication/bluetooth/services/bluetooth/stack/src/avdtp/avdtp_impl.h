/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef AVDTP_IMPL_H
#define AVDTP_IMPL_H

#include "avdtp_int.h"
#include "gap_if.h"

typedef struct {
    uint16_t result;
    GapServiceSecurityInfo security;
    void *context;
} AvdtSecurityCheckTskParam;

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    int result;
    void *context;
} AvdtSignalConnectReqTskParam;

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    int result;
    void *context;
} AvdtStreamConnectReqTskParam;

typedef struct {
    uint16_t lcid;
    int result;
} AvdtStreamSendDataTskParam;

/* Signal  action functions */
extern uint16_t AvdtActConnReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActConnRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActConnInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActConnCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDisconnReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDisconnRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDisconnInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDisconnCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDiscoverReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDiscoverRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDiscoverInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActDiscoverCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetCapReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetCapRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetCapInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetCapCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetAllCapReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetAllCapRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetAllCapInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActGetAllCapCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
extern uint16_t AvdtActSndMsg(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);
/* Stream  action functions */
extern uint16_t AvdtActStreamOpenReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamOpenRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamOpenInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamOpenCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamCloseReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamCloseRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamCloseInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStreamCloseCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSetConfigReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSetConfigRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSetConfigInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSetConfigCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActGetConfigReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActGetConfigRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActGetConfigInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActGetConfigCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActReconfigReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActReconfigRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActReconfigInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActReconfigCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActOpenReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActOpenRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActOpenInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActOpenCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStartReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStartRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStartInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActStartCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActCloseReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActCloseRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActCloseInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActCloseCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSuspendReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSuspendRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSuspendInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActSuspendCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActDelayRptReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActDelayRptRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActDelayRptInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActDelayRptCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActAbortReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActAbortRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActAbortInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActAbortCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActWriteReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActWriteCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActReadInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
extern uint16_t AvdtActBadStateRej(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);

/**
 * Inner function declaration
 */
void AvdtSecurityCheckCallback(uint16_t result, GapServiceSecurityInfo security, void *context);
void AvdtSecurityCheckTsk(void *context);
void AvdtSecurityCheck(uint16_t result, GapServiceSecurityInfo security, void *context);
void AvdtSignalConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);
void AvdtSignalConnectReqTsk(void *context);
void AvdtSignalConnectReq(const BtAddr *addr, uint16_t lcid, int result, void *context);
void AvdtStreamConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);
void AvdtStreamConnectReqTsk(void *context);
void AvdtStreamConnectReq(const BtAddr *addr, uint16_t lcid, int result, void *context);
void AvdtStreamSendDataCallback(uint16_t lcid, int result);
void AvdtStreamSendDataTsk(void *context);
void AvdtStreamSendData(uint16_t lcid, int result);
AvdtSepConfig AvdtGetSepConfig(AvdtSigCtrl *sigCtrl, uint16_t handle);
AvdtSepConfig AvdtGetConfig(AvdtSigCtrl *sigCtrl, uint16_t handle);
#endif /* AVDTP_IMPL_H */