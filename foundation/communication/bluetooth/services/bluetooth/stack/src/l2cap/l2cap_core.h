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

#ifndef L2CAP_CORE_H
#define L2CAP_CORE_H

#include <stdint.h>

#include "alarm.h"
#include "list.h"

#include "l2cap_def.h"
#include "l2cap_inst.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// configuration option type
#define L2CAP_OPTION_MAXIMUM_TRANSMISSION_UNIT 0x01
#define L2CAP_OPTION_FLUSH_TIMEOUT 0x02
#define L2CAP_OPTION_QUALITY_OF_SERVICE 0x03
#define L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL 0x04
#define L2CAP_OPTION_FRAME_CHECK_SEQUENCE 0x05
#define L2CAP_OPTION_EXTENDED_FLOW_SPECIFICATION 0x06
#define L2CAP_OPTION_EXTENDED_WINDOW_SIZE 0x07

#define L2CAP_QOS_SERVICE_TYPE_BEST_EFFORT 0x01
#define L2CAP_QOS_SERVICE_TYPE_GUARANTEED 0x02

// for enhance mode
#define L2CAP_ERFC_UNSEGMENTED_SDU 0x00
#define L2CAP_ERFC_START_SDU 0x01
#define L2CAP_ERFC_END_SDU 0x02
#define L2CAP_ERFC_CONTINUATION_SDU 0x03

#define L2CAP_ERFC_RR 0x00
#define L2CAP_ERFC_REJ 0x01
#define L2CAP_ERFC_RNR 0x02
#define L2CAP_ERFC_SREJ 0x03

#define L2CAP_ERFC_PBIT_OFF 0x00
#define L2CAP_ERFC_PBIT_ON 0x01

#define L2CAP_ERFC_FBIT_OFF 0x00
#define L2CAP_ERFC_FBIT_ON 0x01

#define L2CAP_IFRAME 0x00
#define L2CAP_SFRAME 0x01

#define L2CAP_MAX_TX_WINDOW 64

// for callback
int L2capDisconnectComplete(uint16_t handle, uint8_t status, uint8_t reason);
int L2capConnectComplete(const BtAddr *addr, uint16_t handle, uint8_t status);
int L2capReceivePacket(uint16_t handle, uint16_t cid, Packet *pkt);

int L2capSendEchoReq(L2capConnection *conn, const uint8_t *data, uint16_t dataLen);
int L2capSendEchoRsp(L2capConnection *conn, uint8_t ident, const uint8_t *data, uint16_t dataLen);
int L2capSendConnectionReq(L2capConnection *conn, L2capChannel *chan);
int L2capSendConnectionRsp(L2capConnection *conn, L2capChannel *chan, uint8_t ident, uint16_t result, uint16_t status);
int L2capSendConfigurationReq(L2capConnection *conn, const L2capChannel *chan);
int L2capSendConfigurationRsp(
    const L2capConnection *conn, L2capChannel *chan, uint8_t ident, uint16_t result, const L2capConfigInfo *cfg);
int L2capSendDisconnectionReq(L2capConnection *conn, L2capChannel *chan);
int L2capSendDisconnectionRsp(const L2capConnection *conn, const L2capChannel *chan, uint8_t ident);
int L2capSendInformationReq(L2capConnection *conn, uint16_t type);
int L2capSendInformationRsp(const L2capConnection *conn, uint8_t ident, uint16_t infoType);

int L2capSendSFrame(const L2capConnection *conn, L2capChannel *chan, uint8_t pBit, uint8_t fBit, uint8_t sBit);
int L2capSendIFrame(L2capConnection *conn, L2capChannel *chan, Packet *pkt);

void L2capErfcStartRetransmissionTimer(L2capChannel *chan);
void L2capErfcStartMonitorTimer(L2capChannel *chan);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_CORE_H
