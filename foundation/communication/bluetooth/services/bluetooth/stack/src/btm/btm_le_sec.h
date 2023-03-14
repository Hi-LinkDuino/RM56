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

#ifndef BTM_LE_SEC_H
#define BTM_LE_SEC_H

#include "btstack.h"

#include "btm.h"

#ifdef __cplusplus
extern "C" {
#endif

int BTM_GetLocalIdentityResolvingKey(BtmKey *irk);

int BTM_GetRemoteIdentityResolvingKey(const BtAddr *addr, BtmKey *irk);
int BTM_GetRemoteIdentityAddress(const BtAddr *addr, BtAddr *identityAddress);
int BTM_GetPairdAddressFromRemoteIdentityAddress(const BtAddr *identityAddress, BtAddr *addr);

typedef struct {
    BtAddr addr;
    BtmKey irk;
} BtmIdentityResolvingKey;
int BTM_GetAllRemoteIdentityResolvingKey(BtmIdentityResolvingKey **irks, uint16_t *count);

int BTM_GetAllPairedDevices(BtmLePairedDevice **devices, uint16_t *count);

int BTM_UpdateCurrentRemoteAddress(const BtAddr *pairedAddr, const BtAddr *currentAddr);
int BTM_GetCurrentRemoteAddress(const BtAddr *pairedAddr, BtAddr *currentAddr);

bool BTM_IsDeviceInResolvingList(const BtAddr *addr);

int BTM_ConvertToPairedAddress(const BtAddr *addr, BtAddr *pairedAddress);

void BtmInitLeSecurity();
void BtmCloseLeSecurity();
void BtmStartLeSecurity();
void BtmStopLeSecurity();

#ifdef __cplusplus
}
#endif

#endif