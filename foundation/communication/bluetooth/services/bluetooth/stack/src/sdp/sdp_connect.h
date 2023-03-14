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

#ifndef SDP_CONNECT_H
#define SDP_CONNECT_H

#include <stdint.h>

#include "packet.h"

#include "sdp_util.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t SdpGetTransactionId();
void SdpCreateConnectList();
void SdpDestroyConnectList();
void SdpRegisterToL2cap();
void SdpDeregisterToL2cap();
void SdpSendErrorResponse(uint16_t lcid, uint16_t transactionId, uint16_t errorCode);
void SdpSendSearchFragmentResponse(
    uint16_t lcid, uint16_t transactionId, uint16_t maxCount, const Packet *searchPacket);
void SdpSendSearchResponse(uint16_t lcid, uint16_t transactionId, uint16_t offset, uint8_t *buffer, uint16_t maxCount);
void SdpSendAttributeFragmentResponse(
    uint16_t lcid, SdpPduId pduId, uint16_t transactionId, uint16_t maxCount, const Packet *attributePacket);
void SdpSendAttributeResponse(
    uint16_t lcid, uint16_t transactionId, SdpPduId pduId, uint16_t maxCount, const Packet *packet);
void SdpSendRequest(uint16_t lcid, uint16_t transactionId, uint8_t continuationStateLen,
    const uint8_t *continuationState, Packet *packet);
int SdpClientConnect(SdpClientRequest *request);

#ifdef __cplusplus
}
#endif

#endif  // SDP_CONNECT_H