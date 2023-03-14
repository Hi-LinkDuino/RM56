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

#ifndef SDP_CLIENT_PARSE_H
#define SDP_CLIENT_PARSE_H

#include "sdp.h"

#include "sdp_util.h"

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t SdpGetTransactionId();
void SdpCreateRequestList();
void SdpDestroyRequestList();
void SdpAddRequest(SdpClientRequest *request);
void SdpRemoveRequest(const BtAddr *addr);
SdpClientRequest *SdpFindRemainRequestByAddress(const BtAddr *addr, bool *flag);
SdpClientRequest *SdpFindRequestByAddress(const BtAddr *addr);
SdpClientRequest *SdpFindRequestByTransactionId(uint16_t transactionId);
void SdpRemoveRequestByAddress(const BtAddr *addr);
void SdpRemoveAllRequestByAddress(const BtAddr *addr);
void SdpParseServerResponse(const BtAddr *addr, uint16_t lcid, const Packet *data);

#ifdef __cplusplus
}
#endif

#endif  // SDP_CLIENT_PARSE_H