/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BROADCAST_MANAGER_H
#define BROADCAST_MANAGER_H

#include "data_manager.h"
#include "device_auth.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*postOnGroupCreated)(const TrustedGroupEntry *groupEntry);
    void (*postOnGroupDeleted)(const TrustedGroupEntry *groupEntry);
    void (*postOnDeviceBound)(const char *peerUdid, const TrustedGroupEntry *groupEntry);
    void (*postOnDeviceUnBound)(const char *peerUdid, const TrustedGroupEntry *groupEntry);
    void (*postOnDeviceNotTrusted)(const char *peerUdid);
    void (*postOnLastGroupDeleted)(const char *peerUdid, int groupType);
    void (*postOnTrustedDeviceNumChanged)(int curTrustedDeviceNum);
} Broadcaster;

bool IsBroadcastSupported(void);
int32_t InitBroadcastManager(void);
void DestroyBroadcastManager(void);
Broadcaster *GetBroadcaster(void);
int32_t AddListener(const char *appId, const DataChangeListener *listener);
int32_t RemoveListener(const char *appId);

#ifdef __cplusplus
}
#endif
#endif
