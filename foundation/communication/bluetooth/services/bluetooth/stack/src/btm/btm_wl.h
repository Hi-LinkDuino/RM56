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

#ifndef BTM_WL_H
#define BTM_WL_H

#include <stdbool.h>
#include <stdint.h>

#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WHITE_LIST_ADDRESS_TYPE_PUBLIC 0x00
#define WHITE_LIST_ADDRESS_TYPE_RANDOM 0x01
#define WHITE_LIST_ADDRESS_TYPE_ANONYMOUS_ADVERTISEMENTS 0xff

int BtmAddDeviceToWhiteList(uint8_t addressType, const BtAddr *address);
void BtmRemoveDeviceFromWhiteList(uint8_t addressType, const BtAddr *address);
void BtmClearWhiteList();
int BtmGetDeviceCountInWhiteList();
bool BtmIsDeviceInWhiteList(uint8_t addressType, const BtAddr *address);

void BtmInitWhiteList();
void BtmCloseWhiteList();
void BtmStartWhiteList();
void BtmStopWhiteList();

#ifdef __cplusplus
}
#endif

#endif