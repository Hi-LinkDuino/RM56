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

#ifndef SMP_RECEIVE_H
#define SMP_RECEIVE_H

#include <stdint.h>

#include "packet.h"

#ifdef __cplusplus
extern "C" {
#endif

void SMP_ReceiveData(uint16_t handle, const Packet *pkt);
void SMP_Connected(const BtAddr *addr, uint16_t handle, uint8_t role, uint8_t status);
void SMP_Disconnected(uint16_t handle, uint8_t role, uint8_t reason);

#ifdef __cplusplus
}
#endif

#endif