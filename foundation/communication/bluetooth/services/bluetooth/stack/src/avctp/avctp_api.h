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
#ifndef AVCTP_API_H
#define AVCTP_API_H
#include "avctp_int.h"
#include "event.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t mtu;
    uint16_t mtuBr;
    uint16_t role;
    Event *event;
} AvctRegisterTskParam;

typedef struct {
    Event *event;
} AvctDeregisterTskParam;

typedef struct {
    uint8_t connId;
    AvctConnectParam connParam;
    BtAddr peerAddr;
    Event *event;
    uint16_t ret;
} AvctConnectReqTskParam;

typedef struct {
    uint8_t connId;
    Event *event;
    uint16_t ret;
} AvctDisconnectReqTskParam;

typedef struct {
    uint8_t connId;
    uint8_t label;
    uint8_t cr;
    Packet *msg;
    Event *event;
    uint16_t ret;
} AvctSendMsgReqTskParam;

typedef struct {
    uint8_t connId;
    uint16_t role;
    Event *event;
    uint16_t ret;
} AvctBrConnectReqTskParam;

typedef struct {
    uint8_t connId;
    Event *event;
    uint16_t ret;
} AvctBrDisconnectReqTskParam;

typedef struct {
    uint8_t connId;
    uint8_t label;
    uint8_t cr;
    Packet *msg;
    Event *event;
    uint16_t ret;
} AvctBrSendMsgReqTskParam;

/*****************************************************************************
 * Function
 ****************************************************************************/
uint16_t AvctConnectInitiate(AvctCbConn *cbConn, const AvctConnectParam *connParam, const BtAddr *peerAddr);
uint16_t AvctDisconnect(uint8_t connId, uint8_t type);
void AvctRegisterTsk(void *context);
void AvctRegister(uint16_t mtu, uint16_t mtuBr, uint16_t role);
void AvctDeregisterTsk(void *context);
void AvctDeregister(void);
void AvctConnectReqTsk(void *context);
uint16_t AvctConnectReq(uint8_t *connId, const AvctConnectParam *connParam, const BtAddr *peerAddr);
void AvctDisconnectReqTsk(void *context);
uint16_t AvctDisconnectReq(uint8_t connId);
void AvctSendMsgReqTsk(void *context);
uint16_t AvctSendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg);
void AvctGetPeerMtuTsk(void *context);
uint16_t AvctGetPeerMtu(uint8_t connId);
void AvctBrConnectReqTsk(void *context);
uint16_t AvctBrConnectReq(uint8_t connId, uint8_t role);
void AvctBrDisconnectReqTsk(void *context);
uint16_t AvctBrDisconnectReq(uint8_t connId);
void AvctBrSendMsgReqTsk(void *context);
uint16_t AvctBrSendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg);
void AvctBrGetPeerMtuTsk(void *context);
uint16_t AvctBrGetPeerMtu(uint8_t connId);
#ifdef __cplusplus
}
#endif

#endif /* AVCTP_API_H */