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
#ifndef AVCTP_CTRL_ACT_H
#define AVCTP_CTRL_ACT_H
#include "avctp_int.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PKG_TYPE_MASK   3
#define MOVE_8BIT   8
#define MOVE_4BIT   4
#define MOVE_2BIT   2
#define MOVE_1BIT   1

extern uint16_t AvctCbCtrlChConn(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChBindFail(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChBind(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChConnFail(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChDisconn(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChUnbind(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChCheckDisconn(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChConnInd(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChCloseInd(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChCloseCfm(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlChBusyInd(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlDiscardMsg(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlSendMsg(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlRevMsg(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbCtrlDiscardRevMsg(AvctCbDev *cbDev, const AvctEvtData *data);

/* Frag/asmbl msg data IF */
uint16_t AvctSendSignleMsg(const AvctCbCh *cbCh, const AvctTxMsg txMsg);
uint16_t AvctSendFraMsg(const AvctCbCh *cbCh, const AvctTxMsg txMsg);
extern Packet *AvctMsgAsmbl(AvctCbCh *cbCh, Packet *pkt);

extern void AvctPktDataPrint(const Packet *pkt);

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_CTRL_ACT_H */