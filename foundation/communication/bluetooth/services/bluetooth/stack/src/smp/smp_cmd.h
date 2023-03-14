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

#ifndef SMP_CMD_H
#define SMP_CMD_H

#include <stdint.h>

#include "btstack.h"
#include "packet.h"
#include "smp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SMP_SendDataCb)(uint16_t aclHandle, int result);

int SMP_SendPairingRequest(uint16_t handle, const SMP_PairParam *pairReqParam, SMP_SendDataCb cb);
int SMP_SendPairingResponse(uint16_t handle, const SMP_PairParam *pairRspParam, SMP_SendDataCb cb);
int SMP_SendPairingConfirm(uint16_t handle, const uint8_t *confirm, SMP_SendDataCb cb);
int SMP_SendPairingRandom(uint16_t handle, const uint8_t *random, SMP_SendDataCb cb);
int SMP_SendPairingFailed(uint16_t handle, uint8_t reason, SMP_SendDataCb cb);
int SMP_SendPairingPublicKey(uint16_t handle, const uint8_t *publicKey, SMP_SendDataCb cb);
int SMP_SendPairingDHKeyCheck(uint16_t handle, const uint8_t *dhKeyCheck, SMP_SendDataCb cb);
int SMP_SendEncryptionInformation(uint16_t handle, const uint8_t *ltk, SMP_SendDataCb cb);
int SMP_SendMasterIdentification(uint16_t handle, uint16_t ediv, const uint8_t *rand, SMP_SendDataCb cb);
int SMP_SendIdentityInformation(uint16_t handle, const uint8_t *irk, SMP_SendDataCb cb);
int SMP_SendIdentityAddressInformation(uint16_t handle, const BtAddr *addr, SMP_SendDataCb cb);
int SMP_SendSigningInformation(uint16_t handle, const uint8_t *csrk, SMP_SendDataCb cb);
int SMP_SendSecurityRequest(uint16_t handle, uint8_t authReq, SMP_SendDataCb cb);

#ifdef __cplusplus
}
#endif

#endif