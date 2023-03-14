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

#ifndef BTM_ACL_H
#define BTM_ACL_H

#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

void BtmInitAcl();
void BtmCloseAcl();
void BtmStartAcl();
void BtmStopAcl();

int BtmGetAclHandleByAddress(const BtAddr *addr, uint16_t *handle);
int BtmGetLeAclHandleByAddress(const BtAddr *addr, uint16_t *handle);
int BtmGetAclAddressByHandle(const uint16_t handle, BtAddr *addr);

void BtmCloseAclConnectionByTransport(uint8_t transport);

int BtmStartAutoConnection();
int BtmStopAutoConnection();

#ifdef __cplusplus
}
#endif

#endif