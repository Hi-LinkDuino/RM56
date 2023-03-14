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

#ifndef AVDTP_CTRL_H
#define AVDTP_CTRL_H

#include "avdtp_int.h"

/* Stream Manager function declarations */
extern void AvdtControlBlockInit(void);
extern void AvdtTransChDealloc(uint16_t lcid);
extern void AvdtTransChDeallocAll(void);
extern void AvdtSigDealloc(void);
extern void AvdtFreeSigHandle(uint16_t handle);
extern void AvdtFreeStreamHandle(uint16_t handle);
extern void AvdtFreeSigCtrlByHandle(uint16_t handle);
extern void AvdtStreamProcEvent(AvdtStreamCtrl *streamCtrl, uint8_t event, AvdtEventData *data);
extern void AvdtSigProcEvent(AvdtSigCtrl *sigCtrl, uint8_t event, AvdtEventData *data);
extern uint8_t AvdtCreateTransLabel(AvdtSigCtrl *sigCtrl);
extern uint16_t AvdtAssignSigHandle(void);
extern uint16_t AvdtAssignStreamHandle(void);
extern uint16_t AvdtCreateSequenceNo(AvdtStreamCtrl *avdtStreamCtrl);
extern AvdtStreamCtrl *AvdtGetStreamCtrlByHandle(uint16_t handle);
extern AvdtStreamCtrl *AvdtStreamCtrlAllocate(AvdtSigCtrl *sigCtrl, uint16_t codecIndex);
extern void AvdtStreamCtrlDeallocByHandle(AvdtSigCtrl *sigCtrl, uint16_t handle);
extern void AvdtStreamCtrlDeallocAll(AvdtSigCtrl *sigCtrl);
extern AvdtSigCtrl *AvdtGetSigCtrlByAddr(const BtAddr *bdAddr);
extern AvdtSigCtrl *AvdtGetSigCtrlByHandle(uint16_t handle);
extern AvdtSigCtrl *AvdtSigCtrlAllocate(const BtAddr *bdAddr, uint8_t role);
extern AvdtTransChannel *AvdtTransChTabAllocate(uint8_t type, AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl);
extern AvdtTransChannel *AvdtGetTransChTabByLcid(uint16_t lcid);
extern AvdtTransChannel *AvdtGetTransChTabByAddr(const BtAddr *bdAddr, uint8_t type);
extern AvdtTransChannel *AvdtGetTransChTabByHandle(uint8_t type, uint16_t handle);
extern void AvdtCtrlEvtCallback(
    AvdtSigCtrl *sigCtrl, uint16_t handle, const BtAddr *bdAddr, uint8_t event, const AvdtCtrlData *data, uint8_t role);
extern uint16_t AvdtCreateSEP(const BtAddr *bdAddr);
extern uint16_t AvdtCheckSepExited(uint16_t codecIndex);
extern uint16_t AvdtSepAlloc(AvdtStreamConfig *avdtStreamConfig);
extern AvdtStreamConfig *AvdtGetSepConfigByCodecIndex(uint16_t codecIndex);
extern uint16_t AvdtCheckSepIsUsed(AvdtSigCtrl *sigCtrl, uint16_t codecIndex);
extern uint8_t AvdtAsyncProcess(void (*callback)(void *context), void *context);
extern uint16_t AvdtGetMtu(void);
extern void AvdtRegister(const AvdtRegisterParam *reg);
extern uint16_t AvdtRegisterLocalSEP(AvdtStreamConfig *avdtStreamConfig, uint8_t number);
extern void AvdtIsEdr2MbMode(const BtAddr *bdAddr);
extern void AvdtIsEdr3MbMode(const BtAddr *bdAddr);
#endif /* AVDTP_CTRL_H */