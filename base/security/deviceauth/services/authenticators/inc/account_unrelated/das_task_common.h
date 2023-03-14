/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DAS_COMMON_H
#define DAS_COMMON_H

#include "das_module_defines.h"
#include "hc_types.h"
#include "json_utils.h"
#include "string_util.h"

int32_t GenerateKeyAlias(const Uint8Buff *pkgName, const Uint8Buff *serviceType, const KeyAliasType keyType,
    const Uint8Buff *authId, Uint8Buff *outKeyAlias);
int32_t GetIdPeer(const CJson *in, const char *peerIdKey, const Uint8Buff *authIdSelf, Uint8Buff *authIdPeer);
int32_t GetAndCheckAuthIdPeer(const CJson *in, const Uint8Buff *authIdSelf, const Uint8Buff *authIdPeer);
int32_t GetAuthIdPeerFromPayload(const CJson *in, const Uint8Buff *authIdSelf, Uint8Buff *authIdPeer);
int32_t GetAndCheckKeyLenOnServer(const CJson *in, uint32_t *returnKeyLen);

void DasSendErrMsgToSelf(CJson *out, int errCode);
void DasSendErrorToOut(CJson *out, int errCode); // send error to self and peer

uint32_t ProtocolMessageIn(const CJson *in);
int ClientProtocolMessageOut(CJson *out, int opCode, uint32_t step);
int ServerProtocolMessageOut(CJson *out, int opCode, uint32_t step);

#endif
