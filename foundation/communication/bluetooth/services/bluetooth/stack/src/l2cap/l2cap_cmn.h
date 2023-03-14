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

#ifndef L2CAP_CMN_H
#define L2CAP_CMN_H

#include <stdint.h>

#include "btstack.h"
#include "packet.h"
#include "securec.h"

#include "alarm.h"
#include "list.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define L2CAP_HEADER_LENGTH 0x04
#define L2CAP_SIGNAL_HEADER_LENGTH 0x04

// Packet Boundary Flag
#define L2CAP_FIRST_NON_AUTOMATICALLY_FLUSHABLE_PACKET 0x00
#define L2CAP_CONTINUING_FRAGMENT 0x01
#define L2CAP_FIRST_AUTOMATICALLY_FLUSHABLE_PACKET 0x02
#define L2CAP_A_COMPLETE_L2CAP_PDU 0x03

#define L2CAP_DEFAULT_RTX (30 * 1000)    // 30s
#define L2CAP_DEFAULT_ERTX (180 * 1000)  // 180s

#define L2CAP_DISCONNECTION_TIME_OUT 4000

// l2cap signal
#define L2CAP_COMMAND_REJECT 0x01
#define L2CAP_CONNECTION_REQUEST 0x02
#define L2CAP_CONNECTION_RESPONSE 0x03
#define L2CAP_CONFIGURATION_REQUEST 0x04
#define L2CAP_CONFIGURATION_RESPONSE 0x05
#define L2CAP_DISCONNECTION_REQUEST 0x06
#define L2CAP_DISCONNECTION_RESPONSE 0x07
#define L2CAP_ECHO_REQUEST 0x08
#define L2CAP_ECHO_RESPONSE 0x09
#define L2CAP_INFORMATION_REQUEST 0x0A
#define L2CAP_INFORMATION_RESPONSE 0x0B
#define L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST 0x12
#define L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE 0x13
#define L2CAP_LE_CREDIT_BASED_CONNECTION_REQUEST 0x14
#define L2CAP_LE_CREDIT_BASED_CONNECTION_RESPONSE 0x15
#define L2CAP_LE_FLOW_CONTROL_CREDIT 0x16

#define L2CAP_MIN_CID 0x0080
#define L2CAP_MAX_CID 0xFFFF
#define L2CAP_LE_MIN_CID 0x0040
#define L2CAP_LE_MAX_CID 0x007F

#define L2CAP_MIN_IDENTIFIER 0x01
#define L2CAP_MAX_IDENTIFIER 0xFF

#define L2CAP_SIGNALING_CHANNEL 0x0001
#define L2CAP_CONNECTIONLESS_CHANNEL 0x0002
#define L2CAP_AMP_MANAGER_PROTOCOL 0x0003
#define L2CAP_LE_ATTRIBUTE_PROTOCOL 0x0004
#define L2CAP_LE_SIGNALING_CHANNEL 0x0005
#define L2CAP_LE_SECURITY_MANAGER_PROTOCOL 0x0006
#define L2CAP_BREDR_SECURITY_MANAGER 0x0007

#define L2CAP_SIGNAL_MTU 672

#define L2CAP_MIN_MTU 48
#define L2CAP_LE_MIN_MTU 23

// Reason of Command Reject
#define L2CAP_COMMAND_NOT_UNDERSTOOD 0x0000
#define L2CAP_SIGNAL_MTU_EXCEEDED 0x0001
#define L2CAP_INVALID_CID_IN_REQUEST 0x0002

#define L2CAP_NONE_FLUSH_PACKET 0xFFFF

// channel state
#define L2CAP_CHANNEL_IDLE 0x00
#define L2CAP_CHANNEL_CONNECT_OUT_REQ 0x01
#define L2CAP_CHANNEL_CONNECT_IN_REQ 0x02
#define L2CAP_CHANNEL_CONFIGING 0x03
#define L2CAP_CHANNEL_CONNECTED 0x04
#define L2CAP_CHANNEL_DISCONNECT_OUT_REQ 0x05
#define L2CAP_CHANNEL_DISCONNECT_IN_REQ 0x06

// connection state
#define L2CAP_CONNECTION_IDLE 0x00
#define L2CAP_CONNECTION_CONNECTING 0x01
#define L2CAP_CONNECTION_CONNECTED 0x02
#define L2CAP_CONNECTION_DISCONNECTING 0x03

// offset
#define L2CAP_OFFSET_1 0x01
#define L2CAP_OFFSET_2 0x02
#define L2CAP_OFFSET_3 0x03
#define L2CAP_OFFSET_4 0x04
#define L2CAP_OFFSET_5 0x05
#define L2CAP_OFFSET_6 0x06
#define L2CAP_OFFSET_7 0x07
#define L2CAP_OFFSET_8 0x08
#define L2CAP_OFFSET_9 0x09
#define L2CAP_OFFSET_10 0x0A
#define L2CAP_OFFSET_12 0x0C

// size
#define L2CAP_SIZE_1 0x01
#define L2CAP_SIZE_2 0x02
#define L2CAP_SIZE_3 0x03
#define L2CAP_SIZE_4 0x04
#define L2CAP_SIZE_5 0x05
#define L2CAP_SIZE_6 0x06
#define L2CAP_SIZE_7 0x07
#define L2CAP_SIZE_8 0x08
#define L2CAP_SIZE_9 0x09
#define L2CAP_SIZE_10 0x0A
#define L2CAP_SIZE_11 0x0B
#define L2CAP_SIZE_12 0x0C
#define L2CAP_SIZE_22 0x16

// hint option
#define L2CAP_OPTION_HINT 0x80

typedef struct {
    uint8_t code;
    uint8_t identifier;
    uint16_t length;
} L2capSignalHeader;

typedef struct {
    uint16_t lcid;
    uint8_t code;
    uint8_t identifier;

    Alarm *timer;
} L2capPendingRequest;

typedef struct {
    int (*aclConnected)(const BtAddr *addr, uint16_t handle, uint8_t status);
    int (*aclDisconnected)(uint16_t handle, uint8_t status, uint8_t reason);
    int (*recvL2capPacket)(uint16_t handle, uint16_t cid, Packet *pkt);
} L2capBdrCallback;

typedef struct {
    int (*aclConnected)(const BtAddr *addr, uint16_t handle, uint8_t role, uint8_t status);
    int (*aclDisconnected)(uint16_t handle, uint8_t status, uint8_t reason);
    int (*recvL2capPacket)(uint16_t handle, uint16_t cid, Packet *pkt);
} L2capLeCallback;

void L2capCpuToLe16(uint8_t *dst, uint16_t src);

uint16_t L2capLe16ToCpu(const uint8_t *src);

void *L2capAlloc(int size);
void L2capFree(void *p);

int L2capAsynchronousProcess(void (*task)(const void *context), void (*destroy)(void *context), void *context);

Packet *L2capBuildSignalPacket(uint16_t cid, const L2capSignalHeader *signal, const uint8_t *data);
int L2capSendCommandReject(uint16_t handle, uint16_t cid, uint8_t ident, uint16_t reason, uint16_t data[2]);

void L2capCreatePendingRequest(
    List *pendingList, uint16_t lcid, const L2capSignalHeader *signal, uint64_t timeo, AlarmCallback timerExpired);
void L2capDestroyPendingRequest(List *pendingList, uint8_t identifier);
L2capPendingRequest *L2capGetPendingRequest(List *pendingList, uint8_t identifier);
L2capPendingRequest *L2capGetPendingRequest2(List *pendingList, const void *request);
void L2capClearPendingRequest(List *pendingList);

int L2capSendPacket(uint16_t handle, uint16_t flushTimeout, Packet *pkt);
int L2capSendPacketNoFree(uint16_t handle, uint16_t flushTimeout, Packet *pkt);
int L2capLeSendPacket(uint16_t handle, Packet *pkt);

uint16_t L2capGetTxBufferSize();
uint16_t L2capGetRxBufferSize();

uint16_t L2capLeGetTxBufferSize();

int L2capConnectBdr(const BtAddr *addr);
int L2capConnectLe(const BtAddr *addr);
int L2capConnectLeCancel(const BtAddr *addr);

int L2capAddConnectionRef(uint16_t handle);
int L2capDisconnect(uint16_t handle, uint8_t reason);

int L2capRegisterBdr(const L2capBdrCallback *cb);
int L2capRegisterLe(const L2capLeCallback *cb);

void L2capCommonStartup();
void L2capCommonShutdown();

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_CMN_H
