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
#ifndef AVCTP_GAP_H
#define AVCTP_GAP_H
#include "avctp_int.h"
#include "gap_if.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t result;
    GapServiceSecurityInfo serviceInfo;
    void *context;
} AvctSecurityResultCallbackTskParam;

typedef struct {
    uint16_t result;
    GapServiceSecurityInfo serviceInfo;
    void *context;
} AvctRevConnSecurityResultCallbackTskParam;

extern void AvctRequestSecurity(const AvctCbDev *cbDev, GAP_Service serviceId, uint16_t psm);
extern void AvctRequestSecurityBy(
    const uint16_t aclHandle, const BtAddr addr, const uint16_t lcid, const uint8_t id, const uint16_t lpsm);

void AvctSecurityResultCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);
void AvctSecurityResultCallbackTsk(void *context);
void AvctSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);

void AvctRevConnSecurityResultCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);
void AvctRevConnSecurityResultCallbackTsk(void *context);
void AvctRevConnSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_GAP_H */