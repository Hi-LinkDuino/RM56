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

#ifndef L2CAP_INST_H
#define L2CAP_INST_H

#include <stdint.h>

#include "l2cap_def.h"

#include "alarm.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define L2CAP_DEFAULT_RETRANSMISSION_TIMEOUT 2000  // 2s
#define L2CAP_DEFAULT_MONITOR_TIMEOUT 12000        // 12s
#define L2CAP_DEFAULT_TX_WINDOW 0x08
#define L2CAP_DEFAULT_MAX_TRANSMIT 0x03

// information_state
#define L2CAP_INFO_STATE_NONE 0x00
#define L2CAP_INFO_STATE_PROCESSING 0x01
#define L2CAP_INFO_STATE_DONE 0x02

// configuration state
#define L2CAP_CONFIG_STATE_IN_DONE 0x01
#define L2CAP_CONFIG_STATE_OUT_DONE 0x02

// busy state, bit used
#define L2CAP_BUSY_LOCAL_BUSY 0x01
#define L2CAP_BUSY_REMOTE_BUSY 0x02
#define L2CAP_BUSY_WAIT_F 0x40
#define L2CAP_BUSY_REMOTE_RNR 0x80

typedef struct {
    uint8_t state;
    uint8_t extendedFeature[4];
    uint8_t fixedChannel[8];
} L2capInformation;

typedef struct {
    uint16_t lpsm;
    L2capService service;

    void *ctx;
} L2capPsm;

typedef struct {
    uint8_t *options;
    uint16_t length;
} L2capOptions;

typedef struct {
    Packet *pkt;
    uint8_t retryCount;
} L2capErfcTxPacket;

typedef struct {
    // I-frames contain TxSeq, the send sequence number of the I-frame
    uint16_t txSeq;

    // the Sequence number to be used in the next new I-frame transmitted.
    uint16_t nextTxSeq;

    // the Sequence number of the next I-frame expected to be acknowledged by the receiving peer.
    uint16_t expectedAckSeq;

    // the value of TxSeq expected in the next I-frame.
    uint16_t expectedTxSeq;

    // When segmented I-frames are buffered this is used to delay acknowledgment of received
    // I-frame so that new I-frame transmissions do not cause buffer overflow.
    uint16_t bufferSeq;

    uint8_t busyState;
    uint8_t rejState;

    Alarm *retransmissionTimer;
    Alarm *monitorTimer;
    uint8_t retryCount;

    List *txList;
    Packet *rxSarPacket;
} L2capErfc;

typedef struct {
    // first octet
    uint8_t type : 1;
    uint8_t txSeq : 6;
    uint8_t fBit : 1;

    // second octet
    uint8_t reqSeq : 6;
    uint8_t sar : 2;
} L2capErfcIControl;

typedef struct {
    // first octet
    uint8_t type : 1;
    uint8_t reserved1 : 1;
    uint8_t sBit : 2;
    uint8_t pBit : 1;
    uint8_t reserved2 : 2;
    uint8_t fBit : 1;

    // second octet
    uint8_t reqSeq : 6;
    uint8_t reserved3 : 2;
} L2capErfcSControl;

typedef struct {
    uint16_t lcid;
    uint16_t rcid;

    uint16_t lpsm;
    uint16_t rpsm;

    uint8_t connIdentifier;
    uint8_t state;

    L2capOptions part;

    uint8_t cfgState;
    L2capConfigInfo lcfg;
    L2capConfigInfo rcfg;

    L2capErfc erfc;
} L2capChannel;

typedef struct {
    uint16_t aclHandle;
    BtAddr addr;

    uint8_t state;

    uint8_t nextIdentifier;

    L2capInformation info;

    Alarm *discTimer;

    List *chanList;  // Pack struct L2capChannel

    List *pendingList;  // Pack struct L2capPendingRequest
} L2capConnection;

typedef struct {
    L2capEcho cb;
    void *ctx;
} L2capEchoContext;

typedef struct {
    uint16_t nextLcid;

    List *psmList;   // Pack struct L2capPsm
    List *connList;  // Pack struct L2capConnection

    L2capEchoContext echo;
} L2capInstance;

int L2capInitialized();
L2capInstance *L2capGetInstance();

L2capPsm *L2capGetPsm(uint16_t lpsm);
L2capConnection *L2capGetConnection(uint16_t aclHandle);
L2capConnection *L2capGetConnection2(const BtAddr *addr);
L2capConnection *L2capGetConnection3(const L2capChannel *chan);

L2capChannel *L2capGetChannel(const L2capConnection *conn, int16_t lcid);
void L2capGetChannel2(uint16_t lcid, L2capConnection **conn, L2capChannel **chan);
void L2capGetChannel3(uint16_t aclHandle, uint16_t lcid, L2capConnection **conn, L2capChannel **chan);

L2capChannel *L2capNewChannel(L2capConnection *conn, uint16_t lpsm, uint16_t rpsm);
void L2capDestroyChannel(L2capChannel *chan);
void L2capDeleteChannel(L2capConnection *conn, L2capChannel *chan, uint16_t removeAcl);
L2capConnection *L2capNewConnection(const BtAddr *addr, uint16_t aclHandle);
void L2capDeleteConnection(L2capConnection *conn);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_INST_H
