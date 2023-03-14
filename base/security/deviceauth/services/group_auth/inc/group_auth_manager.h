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

#ifndef GROUP_AUTH_MANAGER_H
#define GROUP_AUTH_MANAGER_H

#include "group_auth_common_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
int32_t GetAuthState(int64_t authReqId, const char *groupId, const char *peerUdid, uint8_t *out, uint32_t *outLen);
void InformDeviceDisconnection(const char *udid);
bool IsTrustedDevice(const char *udid);
int32_t QueryTrustedDeviceNum(void);
void DoAuthDevice(HcTaskBase *task);
void DoProcessAuthData(HcTaskBase *task);
#ifdef __cplusplus
}
#endif
#endif
