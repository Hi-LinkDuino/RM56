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

#ifndef SDP_CLIENT_H
#define SDP_CLIENT_H

#include "sdp.h"

#include "btstack.h"
#include "packet.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize sdp client
void SdpInitializeClient();
// Finalize sdp client
void SdpFinalizeClient();
int SdpServiceSearch(const BtAddr *addr, const SdpUuid *uuidArray, void *context,
    void (*serviceSearchCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context));
int SdpServiceAttribute(const BtAddr *addr, uint32_t handle, SdpAttributeIdList attributeIdList, void *context,
    void (*serviceAttributeCb)(const BtAddr *addr, const SdpService *service, void *context));
int SdpServiceSearchAttribute(const BtAddr *addr, const SdpUuid *uuidArray, SdpAttributeIdList attributeIdList,
    void *context,
    void (*searchAttributeCb)(const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context));
int SdpServiceBrowse(const BtAddr *addr, void *context,
    void (*serviceBrowseCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context));

#ifdef __cplusplus
}
#endif

#endif  // SDP_CLIENT_H