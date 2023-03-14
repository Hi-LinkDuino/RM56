/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef GROUP_AUTH_DATA_OPERATION_H
#define GROUP_AUTH_DATA_OPERATION_H

#include "data_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

bool GaIsGroupAccessible(int32_t osAccountId, const char *groupId, const char *appId);
int32_t GaGetTrustedDeviceEntryById(int32_t osAccountId, const char *deviceId,
    bool isUdid, const char *groupId, TrustedDeviceEntry *returnDeviceEntry);
bool GaIsDeviceInGroup(int32_t groupType, int32_t osAccountId, const char *peerUdid, const char *peerAuthId,
    const char *groupId);

#ifdef __cplusplus
}
#endif
#endif
